#include <iostream>
#include <sstream>

#include "overworld/OverWorldScene.h"
#include "Tileset.h"
#include "entities/NPC.h"
#include "entities/Collectible.h"
#include "entities/GatewayZC.h"
#include "entities/LightEntity.h"
#include "overworld/OverWorldTone.h"
#include "TilePlane.h"
#include "TileAnimator.h"
#include "Overlay.h"
#include "entities/PlayerCharacter.h"
#include "entities/Projectile.h"

#include "utils/SetUnionIterator.h"
#include "utils/FrameAllocator.h"

#include "MapEntitiesLoader.h"
#include "GameResource.h"
#include "ZoneContainerData.h"
#include "Map.h"
#include "DetourCrowd.h"

bool z_orderer (const Entity* lhs, const Entity* rhs) {
	return lhs->getPosition().y < rhs->getPosition().y;
};


OverWorldScene::OverWorldScene(const MetaGameData& metaGameData, GameResource& gr) :
	metaGameData(metaGameData),
	camera(metaGameData.resolution),
	gameClock(
	metaGameData.start_time_hours*3600 + metaGameData.start_time_minutes*60, 
	metaGameData.clock_speed_factor),
	myTotalTime(0),
	myTotalTimeAlways(0),
	gameResources(gr),
	debug_key_pressed(false),
	myState(OverworldSceneState::TRANSITIONING_IN),
	navMeshGenerator(&buildContext)
{ }

OverWorldScene::~OverWorldScene() {
	gameResources.releaseZoneContainer(ZC->getData().dataPath);
}

void OverWorldScene::bindContentToClock() {

	for(auto anim : ZC->getTileset().get_tile_animators()) {
		using namespace std::placeholders;
		auto changeAnimatedTileFrame = std::bind(&TileAnimator::process_frame, anim, _1, _2);

		//next value of myTotalTime multiple of 400 :
		int next = 400*(myTotalTime/400 + 1);

		anim->createPeriodicCallback(next, 400, callbackSystemAlways, changeAnimatedTileFrame);
	}
}

void OverWorldScene::unbindContentToClock() {

	for(auto anim : ZC->getTileset().get_tile_animators()) {
		anim->removePendingCallbacks();
	}
}

void OverWorldScene::loadEntities(bool already_created) {
	if(!already_created) {
		generateEntityFromFile(ZC->getData().entitiesDataPath , *ZC, gameResources, ticks, owStateChangeRequest);
	}
}

void OverWorldScene::onInit() {
	pagedVectorStatic.resetForNewFrame();

	owDisplay.init(metaGameData);
	owCommandsState.init();
	OverWorldTone::init();

	PC_moved = false;

	overlay = new Overlay(*App);
	
	ZC = &gameResources.getZoneContainer(metaGameData.firstZone);

	bindContentToClock();

	loadEntities(false);

	auto& anim = gameResources.getMoveAnimation("../../ressources/male_walkcycle.png");
	
	PC = new PlayerCharacter(ZC->getData().startingPos, *ZC, ticks, anim, *overlay);
	torchLight = new LightEntity(PC->getSpriteCenter(), *ZC, 300, 20, sf::Color(10,10,10,250));

	owTransition.time_remaining_ms = owTransition.total_time_ms;

	navMeshGenerator.handleBuild(ZC->getCollisionArray());
	crowdTool.init(&navMeshGenerator);
	crowdToolState.init(&navMeshGenerator);
	float pos[3] = { 200, 0, 200};
	float pos2[3] = { 300, 0, 200};
	float target[3] = { 500, 0, 500};
	
	crowdToolState.addAgent(pos);
	crowdToolState.addAgent(pos2);
	crowdToolState.setMoveTarget(target, false);
}


struct UpdateMapGraphics {

	UpdateMapGraphics(OverWorldCamera& camera, bool checkAnimatedTilesUpdate, int deltaTime) :
		camera(camera), checkAnimatedTilesUpdate(checkAnimatedTilesUpdate), deltaTime(deltaTime)
	{ }

	void visit_first(Map* map) {
		map->loadTilesFromNothing(camera);
		sf::Vector2i v = map->getMapCoords();
		std::cout << "Loaded map <"<<  v.x << "," << v.y <<">\n";
	}

	void visit_second(Map* map) {
		sf::Vector2i v = map->getMapCoords();
		std::cout << "Unloading map <"<<  v.x << "," << v.y <<">\n";
		map->unloadAll();
	}

	void visit_both(Map* map, Map*) {
		map->updateGraphics(camera, checkAnimatedTilesUpdate, deltaTime);
	}

	OverWorldCamera& camera;
	bool checkAnimatedTilesUpdate;
	int deltaTime;
};

const float update_margin_x = 300;
const float update_margin_y = 300;

sf::FloatRect getUpdateRect(const sf::FloatRect& rect) {
	auto rect2 = rect;
	rect2.left -= update_margin_x;
	rect2.top -= update_margin_y;
	rect2.width += 2*update_margin_x;
	rect2.height += 2*update_margin_y;
	return rect2;
}

inline bool isTooFarForUpdate(const Entity* ent, const sf::FloatRect& updateRect) {

	return !updateRect.contains(ent->getPosition());

}

std::string getGameStateName(OverworldSceneState e) {
	if(e == OverworldSceneState::NORMAL) {
		return "normal";
	}
	if(e == OverworldSceneState::PAUSED) {
		return "paused";
	}
	if(e == OverworldSceneState::TRANSITIONING_IN) {
		return "transition (in)";
	}
	if(e == OverworldSceneState::TRANSITIONING_OUT) {
		return "transition (out)";
	}
	return "unknown";
}

void OverWorldScene::update(int deltaTime) {

	pagedVectorStatic.resetForNewFrame();
	pause_state.beginNewFrame();

	if(myState == OverworldSceneState::TRANSITIONING_IN && !owTransition.isActive()) {
		myState = OverworldSceneState::NORMAL;
	}
	else if(myState == OverworldSceneState::TRANSITIONING_OUT && !owTransition.isActive()) {
		myState = OverworldSceneState::TRANSITIONING_IN;
		this->changeZone(owTransition.ZoneId);
		deltaTime = 1;
		owTransition.time_remaining_ms = owTransition.total_time_ms;
	}
	else if(myState == OverworldSceneState::NORMAL) {

		if(pause_state) {
			myState = OverworldSceneState::PAUSED;
		}
		else {
			if(ChangeZCRequest* changeZoneRequest = owStateChangeRequest.popZoneChangeRequest()) {
				owTransition.ZoneId  = changeZoneRequest->newZC;
				owTransition.time_remaining_ms = owTransition.total_time_ms;
				myState = OverworldSceneState::TRANSITIONING_OUT;
				delete changeZoneRequest;
			}
		}
	}
	else if(myState == OverworldSceneState::PAUSED) {
		if(!pause_state) {
			myState = OverworldSceneState::NORMAL;
		}
	}

	owCommands.pollComands(*App);

	if (owCommands.isActive(OVERWORLD_COMMANDS::EXIT) ) {
		close();
		return;
	}

	if(owCommands.isActive(OVERWORLD_COMMANDS::DEBUG)) {
		debug_key_pressed = !debug_key_pressed;
	}


	if ( (myState == OverworldSceneState::NORMAL || myState == OverworldSceneState::PAUSED)
		&& owCommands.isActive(OVERWORLD_COMMANDS::PAUSE) 
		) {
		if(pause_state) {
			pause_state.unpauseOnNextFrame();
		} else {
			pause_state.pauseOnNextFrame();
		}
	}

	myDeltaTimeAlways = deltaTime;
	myTotalTimeAlways += deltaTime;

	ZC->getTileset().setNeedUpdating(false);
	callbackSystemAlways.callAllUpToTime(myTotalTimeAlways);

	//end the update loop here if gameplay is paused :
	if(myState == OverworldSceneState::PAUSED) {
		camera.cameraSetForFrame();
		for(auto& map : visibleMaps) {
			map->updateGraphics(camera, ZC->getTileset().getNeedUpdating(), myDeltaTimeAlways);
		}
		myDeltaTime = 0;
		gameClock.update(deltaTime, true);
		return;
	}

	if(myState == OverworldSceneState::TRANSITIONING_IN || myState == OverworldSceneState::TRANSITIONING_OUT) {
		owTransition.time_remaining_ms -= deltaTime;
	}

	camera.newFrame();
	ticks.update(deltaTime);

	myDeltaTime = deltaTime;
	myTotalTime += deltaTime;
	gameClock.update(deltaTime, false);
	

	callbackSystem.callAllUpToTime(myTotalTime);


	if(owCommands.isActive(OVERWORLD_COMMANDS::FLASHLIGHT)) {
		owCommandsState.flashlight_on = !owCommandsState.flashlight_on;
		torchLight->setOn(owCommandsState.flashlight_on) ;
	}

	if(owCommands.isActive(OVERWORLD_COMMANDS::ACTIVATE)) {
		new Projectile(PC->getPosition(), *ZC, PC->getFacingDir());
	}

	PC_moved = false;

	bool debug_move_through = owCommands.isActive(OVERWORLD_COMMANDS::DEBUG_NO_COLLIDE);

	if (owCommands.isActive(OVERWORLD_COMMANDS::MOVE_UP) ) {  
		PC->tryMoving(deltaTime, DIRECTION::UP, 0, !owCommandsState.already_pressed_u, debug_move_through);
		PC_moved = true;
		owCommandsState.already_pressed_u = true;
	} else {
		owCommandsState.already_pressed_u = false;
	}

	if (owCommands.isActive(OVERWORLD_COMMANDS::MOVE_DOWN) )  {
		PC->tryMoving(deltaTime, DIRECTION::DOWN, 0, !owCommandsState.already_pressed_d, debug_move_through);
		PC_moved = true;
		owCommandsState.already_pressed_d= true;
	}else {
		owCommandsState.already_pressed_d = false;
	}

	if (owCommands.isActive(OVERWORLD_COMMANDS::MOVE_LEFT) )  {
		PC->tryMoving(deltaTime, DIRECTION::LEFT, 0, !owCommandsState.already_pressed_l, debug_move_through);
		owCommandsState.already_pressed_l= true;
		PC_moved = true;
	}else {
		owCommandsState.already_pressed_l = false;
	}

	if (owCommands.isActive(OVERWORLD_COMMANDS::MOVE_RIGHT) )  { 
		PC->tryMoving(deltaTime, DIRECTION::RIGHT, 0, !owCommandsState.already_pressed_r, debug_move_through);
		PC_moved = true;
		owCommandsState.already_pressed_r= true;
	}else {
		owCommandsState.already_pressed_r = false;
	}

	if (owCommands.isActive(OVERWORLD_COMMANDS::MOVE_UP_FAST) )  {  
		PC->tryMoving(deltaTime*10, DIRECTION::UP, 0, owCommandsState.already_pressed_u, debug_move_through);
	}

	if (owCommands.isActive(OVERWORLD_COMMANDS::MOVE_DOWN_FAST))  {
		PC->tryMoving(deltaTime*10, DIRECTION::DOWN, 0, owCommandsState.already_pressed_u, debug_move_through);
	}

	if (owCommands.isActive(OVERWORLD_COMMANDS::MOVE_LEFT_FAST))  {
		PC->tryMoving(deltaTime*10, DIRECTION::LEFT, 0, owCommandsState.already_pressed_u, debug_move_through);
	}

	if (owCommands.isActive(OVERWORLD_COMMANDS::MOVE_RIGHT_FAST))  { 
		PC->tryMoving(deltaTime*10, DIRECTION::RIGHT, 0,owCommandsState.already_pressed_u, debug_move_through);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
	{
		owDisplay.myWaveParameters.wave_amplitude += 0.1;
		owDisplay.changeWaterParameters(owDisplay.myWaveParameters);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
	{
		owDisplay.myWaveParameters.wave_amplitude -= 0.1;
		owDisplay.changeWaterParameters(owDisplay.myWaveParameters);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Y))
	{
		owDisplay.myWaveParameters.wave_angle += 0.005;
		owDisplay.changeWaterParameters(owDisplay.myWaveParameters);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::H))
	{
		owDisplay.myWaveParameters.wave_angle -= 0.005;
		owDisplay.changeWaterParameters(owDisplay.myWaveParameters);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::U))
	{
		owDisplay.myWaveParameters.wave_spatial_period += 0.1;
		owDisplay.changeWaterParameters(owDisplay.myWaveParameters);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::J))
	{
		owDisplay.myWaveParameters.wave_spatial_period -= 0.1;
		owDisplay.changeWaterParameters(owDisplay.myWaveParameters);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::I))
	{
		owDisplay.myWaveParameters.wave_time_period -= 0.1;
		owDisplay.changeWaterParameters(owDisplay.myWaveParameters);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::K))
	{
		owDisplay.myWaveParameters.wave_time_period += 0.1;
		owDisplay.changeWaterParameters(owDisplay.myWaveParameters);
	}

	if(torchLight->isOn_()) {
		torchLight->setPosition(PC->getSpriteCenter());
	}

	if(!PC_moved)
		PC->isMoving = false;

	if(owCommands.isActive(OVERWORLD_COMMANDS::ZOOM_RESET)) {
		camera.resetSize();
	}

	camera.setCenter(PC->getSpriteCenter());

	if (owCommands.isActive(OVERWORLD_COMMANDS::ZOOM_IN))      
		camera.zoom(1.002f);
	else if (owCommands.isActive(OVERWORLD_COMMANDS::ZOOM_OUT)) 
		camera.zoom(0.998f);
	else if (owCommands.isActive(OVERWORLD_COMMANDS::ZOOM_IN_FAST)) 
		camera.zoom(0.9f);
	else if (owCommands.isActive(OVERWORLD_COMMANDS::ZOOM_OUT_FAST)) 
		camera.zoom(1.1f);


	camera.cameraSetForFrame();

	sf::Vector2f cameraCenter = camera.getCenter();
	sf::FloatRect viewRect = camera.getViewRect();
	sf::FloatRect entityUpdateRect = getUpdateRect(viewRect);

	//update list of visible maps :
	std::set<Map*> newVisibleMaps = ZC->getCollidingMaps(viewRect);
	std::set<Map*> newEntityUpdateMaps = ZC->getCollidingMaps(entityUpdateRect);
	UpdateMapGraphics updater(camera, ZC->getTileset().getNeedUpdating(), myDeltaTimeAlways);
	iterate_over_union(newVisibleMaps, visibleMaps, updater);
	visibleMaps = std::move(newVisibleMaps);

	//manage entities:
	ZC->deleteElements();

	entities_visible.clear();

	clock_t start;

	start = clock();
	part1_total = 0;
	part2_total = 0;

	clock_t part1_start;
	clock_t part2_start;

	std::set<Entity*, std::less<Entity*>, FrameAllocator<Entity*>> entities_updated;

	part2_start = clock();
	for (auto map = visibleMaps.begin() ; map != visibleMaps.end(); ++map ) {
		std::set<EntitySet*> res;
		(*map)->getCollidingVisibilitySets(viewRect, res, true);

		for(auto& entitySet : res) {
			
			//this set can have been emptied due to entity update from within the loop
			if(!entitySet->has_entities()) {
				continue;
			}

			std::set<Entity*>& set = entitySet->entities();

			for(auto ent = set.begin();  entitySet->has_entities() && ent != set.end(); ) {

				if(!viewRect.intersects((*ent)->getVisibilityRectangle())) {
					//entity is in the visibility set but not in view
					++ent;
					continue;
				}



				//visible entities have to be updated :
				if(entities_updated.count(*ent) != 0) {
					++ent;
					continue;
				}

				part1_start = clock();
				Entity* current_ent = *ent;
				(*ent++)->update(myDeltaTime, true);
				entities_updated.insert(current_ent);

				if(!current_ent->isMarkedForDeletion()) { //TODO : remove if not visible now due to update
					entities_visible.insert(current_ent);
				}

				part1_total += clock() - part1_start;
			}
		}
	}

	part2_total += clock() - part2_start;

	//handle update of entities that are going to be drawn:

	//std::cout <<  "\nstart entity loop\n";
	for (auto map = newEntityUpdateMaps.begin() ; map != newEntityUpdateMaps.end(); ++map ) {
		//std::cout <<  "\n\n";
		for(auto ent = (*map)->entities_list().begin();  ent != (*map)->entities_list().end(); ) {
			//std::cout << *ent << '\t';
			if(isTooFarForUpdate(*ent, entityUpdateRect)) {
				++ent;
				continue;
			}

			if(entities_updated.count(*ent) != 0) {
				++ent;
				continue;
			}

			part1_start = clock();
			Entity* current_ent = *ent;
			(*ent++)->update(myDeltaTime, false);
			entities_updated.insert(current_ent);

			//TODO : add to entities_visible if visible now

			part1_total += clock() - part1_start;
		}
	}

	auto crowd = this->navMeshGenerator.getCrowd();
	dtCrowdAgentDebugInfo m_agentDebug;
	crowd->update(myDeltaTime/1000.f, 0);// &m_agentDebug);


	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		
		sf::Vector2i localPosition = sf::Mouse::getPosition(*App);
		float target[3] = { 0, 0, 0};
		auto rect = camera.getViewRect();
		target[0] = localPosition.x * camera.getZoom() + rect.left;
		target[2] = localPosition.y * camera.getZoom() + rect.top;
		crowdToolState.setMoveTarget(target, false);
	} else if (sf::Mouse::isButtonPressed(sf::Mouse::Right) ) {
		sf::Vector2i localPosition = sf::Mouse::getPosition(*App);
		float target[3] = { 0, 0, 0};
		auto rect = camera.getViewRect();
		target[0] = localPosition.x * camera.getZoom() + rect.left;
		target[2] = localPosition.y * camera.getZoom() + rect.top;
		static int i = 2;
		++i;
		std::cout << "Agents : " << i <<'\n';
		crowdToolState.addAgent(target);
	}
}


void OverWorldScene::draw() {  

	App->clear();

	// --- overlay info 
	if(ticks.getTicks(TICKS::_100MS) > 0) {
		std::stringstream text;
		text << EntitySet::entitySetStorage.size_in_use() << " entities set in use, "<< EntitySet::entitySetStorage.size_available()<< " extra available\n";
	
		text << visibleMaps.size() << " maps visible\n";
		if (visibleMaps.size() < 20) {
			for (auto& map : visibleMaps) {
				sf::Vector2i v = map->getMapCoords();
				text << "map <"<< v.x <<","<< v.y <<"> in view\n";
			}
		}

		overlay->MapInfosText.setString(text.str()+'\n'+gameClock.getCurrentTimeOfDayStr());
	}
	// --- overlay info 


	owDisplay.clearAndSetView(camera.getView());
	owDisplay.updateWaterParameters(myTotalTimeAlways);


	//draw the world:
	clock_t world_draw = clock();
	for (auto map = visibleMaps.begin() ; map != visibleMaps.end(); ++map ) {
		(*map)->drawLayer(camera.getView(), owDisplay, -1);
	}
	for (auto map = visibleMaps.begin() ; map != visibleMaps.end(); ++map ) {
		(*map)->drawLayer(camera.getView(), owDisplay, 0);
	}
	world_draw = clock() - world_draw;

	//draw the entities:
	clock_t entities_draw = clock();

	std::vector<Entity*, FrameAllocator<Entity*>> entities_y_sorted;
	entities_y_sorted.reserve(entities_visible.size());

	for(auto& ent : entities_visible) {
		entities_y_sorted.push_back(ent);
	}

	std::sort(entities_y_sorted.begin(), entities_y_sorted.end(), z_orderer);

	for(auto& ent : entities_y_sorted) {
		ent->draw(owDisplay);
	}
	entities_draw = clock() - entities_draw;

	if(debug_key_pressed) {
		for(auto& ent : entities_visible) {
			ent->drawDebugInfo(owDisplay);
		}

		drawNavMesh();
	}


	//
	clock_t shader_draw = clock();
	Tone t = ZC->getData().isOutside ? 
		OverWorldTone::getToneAt( gameClock.getCurrentTimeOfDay()/3600.f) :
		(ZC->getData().isDark ? OverWorldTone::getDarknessTone() : Tone());

	owDisplay.updateToneParameters(t);

	crowdToolState.handleRender(owDisplay);

	owDisplay.draw(*App);

	if(myState == OverworldSceneState::TRANSITIONING_IN || myState == OverworldSceneState::TRANSITIONING_OUT) {
		sf::RectangleShape shape(sf::Vector2f(metaGameData.resolution.x, metaGameData.resolution.y));
		shape.setPosition(0,0);

		float opacity = myState == OverworldSceneState::TRANSITIONING_OUT ?
			255.f*(1.f - float(owTransition.time_remaining_ms) / float(owTransition.total_time_ms))
			:
		255.f*( float(owTransition.time_remaining_ms) / float(owTransition.total_time_ms));

		sf::Uint8 o = 0;
		if(opacity > 0 && opacity <= 255.f) {
			o = static_cast<sf::Uint8>(opacity);
		} else if(opacity > 255.f) {
			o = 255;
		}
		shape.setFillColor(sf::Color(0,0,0,o));
		App->draw(shape); 
	}

	shader_draw = clock() - shader_draw;


	//infos : 
	if(myState == OverworldSceneState::PAUSED || ticks.getTicks(TICKS::_100MS) > 0) {
		std::stringstream oss;
		int FPS = int(1000/myDeltaTimeAlways);
		oss << "FPS: " << FPS<<  " ent update: " << 1000.f*part1_total/CLOCKS_PER_SEC  << "ms, ent z sort: " << 1000.f*part2_total/CLOCKS_PER_SEC <<"ms";
		oss << "\nmap drawing time: " << 1000.f*world_draw/CLOCKS_PER_SEC<< "ms\tent draw time:" << 1000.f*entities_draw/CLOCKS_PER_SEC << "ms\tfinal shader drawing: "<<1000.f*shader_draw/CLOCKS_PER_SEC;
		oss << "\n" << ZC->getData().name << "\t(" << getGameStateName(myState)<<")";
		overlay->FPStext.setString(oss.str());
	}
	overlay->draw(false);

	//



	App->display();
}

void OverWorldScene::changeZone(const std::string& newZC) {

	bool zone_already_loaded = false;
	ZoneContainer& newZone = gameResources.getZoneContainer(newZC, zone_already_loaded);

	PC->teleportTo(newZone.getData().startingPos, &newZone);
	unbindContentToClock();
	gameResources.releaseZoneContainer(ZC->getData().dataPath);
	ZC = &newZone;
	torchLight = new LightEntity(PC->getSpriteCenter(), *ZC, 300, 20, sf::Color(10,10,10,250));
	visibleMaps.clear();
	loadEntities(zone_already_loaded);
	bindContentToClock();
}

void drawMeshTile(sf::RenderTexture& rt, const dtNavMesh&, const dtMeshTile*);

void OverWorldScene::drawNavMesh() {

	const dtNavMesh* mesh = navMeshGenerator.getNavMesh();

	if(!mesh)
		return;



	for (int i = 0; i < mesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh->getTile(i);
		if (!tile->header) continue;
		drawMeshTile(owDisplay.debug_texture, *mesh, tile);
	}
}

static float distancePtLine2d(const float* pt, const float* p, const float* q)
{
	float pqx = q[0] - p[0];
	float pqz = q[2] - p[2];
	float dx = pt[0] - p[0];
	float dz = pt[2] - p[2];
	float d = pqx*pqx + pqz*pqz;
	float t = pqx*dx + pqz*dz;
	if (d != 0) t /= d;
	dx = p[0] + t*pqx - pt[0];
	dz = p[2] + t*pqz - pt[2];
	return dx*dx + dz*dz;
}

void drawMeshTile(sf::RenderTexture& rt, const dtNavMesh& mesh, const dtMeshTile* tile)  {

	dtPolyRef base = mesh.getPolyRefBase(tile);

	int tileNum = mesh.decodePolyIdTile(base);
	
	sf::VertexArray v0;
	v0.setPrimitiveType(sf::PrimitiveType::Triangles);
	for (int i = 0; i < tile->header->polyCount; ++i)
	{
		const dtPoly* p = &tile->polys[i];
		if (p->getType() == DT_POLYTYPE_OFFMESH_CONNECTION)	// Skip off-mesh links.
			continue;

		const dtPolyDetail* pd = &tile->detailMeshes[i];

		sf::Color col;

		if (p->getArea() == 0) // Treat zero area type as default.
			col = sf::Color(0,192,255,64);
		else
			col = sf::Color(100,192,255, 64);


		
		for (int j = 0; j < pd->triCount; ++j)
		{
			const unsigned char* t = &tile->detailTris[(pd->triBase+j)*4];
			for (int k = 0; k < 3; ++k)
			{
				if (t[k] < p->vertCount)
					v0.append(sf::Vertex(sf::Vector2f(tile->verts[p->verts[t[k]]*3],tile->verts[p->verts[t[k]]*3 +2]) , col));
					//dd->vertex(&tile->verts[p->verts[t[k]]*3], col);
				else
					v0.append(sf::Vertex(sf::Vector2f(tile->detailVerts[(pd->vertBase+t[k]-p->vertCount)*3], tile->detailVerts[(pd->vertBase+t[k]-p->vertCount)*3+2] ), col));
			}
		}
	}
	
	rt.draw(v0);

	///////////////--------------------------------
	sf::VertexArray v;
	v.setPrimitiveType(sf::PrimitiveType::Lines);

	static const float thr = 0.1f*0.1f;

	sf::Color col = sf::Color::White;

	for (int i = 0; i < tile->header->polyCount; ++i)
	{
		const dtPoly* p = &tile->polys[i];

		if (p->getType() == DT_POLYTYPE_OFFMESH_CONNECTION) continue;

		const dtPolyDetail* pd = &tile->detailMeshes[i];

		for (int j = 0, nj = (int)p->vertCount; j < nj; ++j)
		{
			sf::Color c = col;

			if (p->neis[j] & DT_EXT_LINK)
			{
				bool con = false;
				for (unsigned int k = p->firstLink; k != DT_NULL_LINK; k = tile->links[k].next)
				{
					if (tile->links[k].edge == j)
					{
						con = true;
						break;
					}
				}
				if (con)
					c = sf::Color(255,255,255,255);
				else
					c = sf::Color(0,0,0,255);
			}
			else
				c = sf::Color(0,48,64,255);



			const float* v0 = &tile->verts[p->verts[j]*3];
			const float* v1 = &tile->verts[p->verts[(j+1) % nj]*3];

			// Draw detail mesh edges which align with the actual poly edge.
			// This is really slow.
			for (int k = 0; k < pd->triCount; ++k)
			{
				const unsigned char* t = &tile->detailTris[(pd->triBase+k)*4];
				const float* tv[3];
				for (int m = 0; m < 3; ++m)
				{
					if (t[m] < p->vertCount)
						tv[m] = &tile->verts[p->verts[t[m]]*3];
					else
						tv[m] = &tile->detailVerts[(pd->vertBase+(t[m]-p->vertCount))*3];
				}
				for (int m = 0, n = 2; m < 3; n=m++)
				{
					if (((t[3] >> (n*2)) & 0x3) == 0) continue;	// Skip inner detail edges.
					if (true)
						//distancePtLine2d(tv[n],v0,v1) < thr &&
						//distancePtLine2d(tv[m],v0,v1) < thr )
					{
						v.append(sf::Vertex(sf::Vector2f(tv[n][0]*1,tv[n][2]*1) , col));
						v.append(sf::Vertex(sf::Vector2f(tv[m][0]*1,tv[m][2]*1) , col));
					}
				}
			}
		}
	}
	
	
	rt.draw(v);
}

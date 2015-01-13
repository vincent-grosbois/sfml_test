#include <iostream>
#include <sstream>

#include "overworld/OverworldScene.h"
#include "Tileset.h"
#include "entities/NPC.h"
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


OverworldScene::OverworldScene(const MetaGameData& metaGameData, GameResource& gr) :
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

OverworldScene::~OverworldScene() {
	gameResources.releaseZoneContainer(ZC->getData().dataPath);
}

void OverworldScene::bindContentToClock() {

	for(auto anim : ZC->getTileset().get_tile_animators()) {
		using namespace std::placeholders;
		auto changeAnimatedTileFrame = std::bind(&TileAnimator::process_frame, anim, _1, _2);

		int frame_duration = anim->frame_duration;

		//next value of myTotalTime multiple of frame_duration:
		int next_event = frame_duration*(myTotalTime/frame_duration + 1);

		anim->createPeriodicCallback(next_event, frame_duration, callbackSystemAlways, changeAnimatedTileFrame);
	}
}

void OverworldScene::unbindContentToClock() {
	for(auto anim : ZC->getTileset().get_tile_animators()) {
		anim->removePendingCallbacks();
	}
}

void OverworldScene::loadEntities(bool already_created) {
	if(!already_created) {
		generateEntityFromFile(ZC->getData().entitiesDataPath , *ZC, gameResources, ticks, owStateChangeRequest);
	}
}

void OverworldScene::onInit() {
	memoryPagesStatic.resetForNewFrame();

	owDisplay.init(metaGameData);

	overlay = new Overlay(*App);
	
	ZC = &gameResources.getZoneContainer(metaGameData.firstZone);

	bindContentToClock();

	loadEntities(false);

	MoveAnimation& PC_walkAnim = gameResources.getMoveAnimation("../../ressources/male_walkcycle.png");
	
	PC = new PlayerCharacter(ZC->getData().startingPos, *ZC, ticks, PC_walkAnim, *overlay);
	torchLight = new LightEntity(PC->getSpriteCenter(), *ZC, 300, 20, sf::Color(10,10,10,250));

	myState = OverworldSceneState::TRANSITIONING_IN;
	owTransition.time_remaining_ms = owTransition.total_time_ms;

	navMeshGenerator.handleBuild(ZC->getCollisionArray());
	crowdTool.init(&navMeshGenerator);
	crowdToolState.init(&navMeshGenerator);
}


struct UpdateMapGraphics {

	UpdateMapGraphics(OverworldCamera& camera, bool checkAnimatedTilesUpdate, int deltaTime) :
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

	OverworldCamera& camera;
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

void OverworldScene::handleStateChange() {

	switch(myState) {
	case OverworldSceneState::NORMAL :
		if(owStateChangeRequest.hasPauseRequest()) {
			if(owStateChangeRequest.getPauseRequest().pause) {
				myState = OverworldSceneState::PAUSED;
			}
		}
		else if(owStateChangeRequest.hasZoneChangeRequest()) {
			owTransition.ZoneId  = owStateChangeRequest.getZoneChangeRequest().newZC;
			owTransition.time_remaining_ms = owTransition.total_time_ms;
			myState = OverworldSceneState::TRANSITIONING_OUT;
		}

		break;
	case OverworldSceneState::PAUSED:
		if(owStateChangeRequest.hasPauseRequest()) {
			if(!owStateChangeRequest.getPauseRequest().pause) {
				myState = OverworldSceneState::NORMAL;
			}
		}
		break;
	case OverworldSceneState::TRANSITIONING_IN:
		if(!owTransition.isActive()) {
			myState = OverworldSceneState::NORMAL;
		}
		break;
	case OverworldSceneState::TRANSITIONING_OUT:
		if(!owTransition.isActive()) {
			myState = OverworldSceneState::TRANSITIONING_IN;
			this->changeZone(owTransition.ZoneId);
			//deltaTime = 1;
			owTransition.time_remaining_ms = owTransition.total_time_ms;
		}
		break;
	}

	owStateChangeRequest.clear();
}

void OverworldScene::update(int deltaTime) {

	//reset memory allocator
	memoryPagesStatic.resetForNewFrame();

	handleStateChange();

	owCommands.pollComands(*App);

	if (owCommands.isActive(OVERWORLD_COMMANDS::EXIT) ) {
		close();
		return;
	}

	if(owCommands.isActive(OVERWORLD_COMMANDS::DEBUG)) {
		debug_key_pressed = !debug_key_pressed;
	}


	if ( (myState == OverworldSceneState::NORMAL || myState == OverworldSceneState::PAUSED)
		&& owCommands.isActive(OVERWORLD_COMMANDS::PAUSE) ) 
	{
		owStateChangeRequest.pushPauseRequest(myState == OverworldSceneState::NORMAL);
	}

	myDeltaTimeAlways = deltaTime;
	myTotalTimeAlways += deltaTime;

	ZC->getTileset().setNeedUpdating(false);
	callbackSystemAlways.callAllUpToTime(myTotalTimeAlways);
	owDisplay.updateWaterMovement(myTotalTimeAlways);

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
		torchLight->setOn(!torchLight->isOn()) ;
	}

	if(owCommands.isActive(OVERWORLD_COMMANDS::ACTIVATE)) {
		new Projectile(PC->getPosition(), *ZC, PC->getFacingDir());
	}


	bool debug_move_through = owCommands.isActive(OVERWORLD_COMMANDS::DEBUG_NO_COLLIDE);

	if (owCommands.isActive(OVERWORLD_COMMANDS::MOVE_UP) ) {  
		PC->moveRequest(DIRECTION::UP, 1.f, debug_move_through);
	} 

	if (owCommands.isActive(OVERWORLD_COMMANDS::MOVE_DOWN) )  {
		PC->moveRequest(DIRECTION::DOWN, 1.f, debug_move_through);
	}

	if (owCommands.isActive(OVERWORLD_COMMANDS::MOVE_LEFT) )  {
		PC->moveRequest(DIRECTION::LEFT, 1.f, debug_move_through);
	}

	if (owCommands.isActive(OVERWORLD_COMMANDS::MOVE_RIGHT) )  { 
		PC->moveRequest(DIRECTION::RIGHT, 1.f, debug_move_through);
	}

	if (owCommands.isActive(OVERWORLD_COMMANDS::MOVE_UP_FAST) )  {  
		PC->moveRequest(DIRECTION::UP, 10.f, debug_move_through);
	}

	if (owCommands.isActive(OVERWORLD_COMMANDS::MOVE_DOWN_FAST))  {
		PC->moveRequest(DIRECTION::DOWN, 10.f, debug_move_through);
	}

	if (owCommands.isActive(OVERWORLD_COMMANDS::MOVE_LEFT_FAST))  {
		PC->moveRequest(DIRECTION::LEFT, 10.f, debug_move_through);
	}

	if (owCommands.isActive(OVERWORLD_COMMANDS::MOVE_RIGHT_FAST))  { 
		PC->moveRequest(DIRECTION::RIGHT, 10.f, debug_move_through);
	}

	/*if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
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
	}*/

	if(torchLight->isOn()) {
		torchLight->setPosition(PC->getSpriteCenter());
	}

	if(owCommands.isActive(OVERWORLD_COMMANDS::ZOOM_RESET)) {
		camera.resetSize();
	}

	camera.setCenter(PC->getSpriteCenter());

	if (owCommands.isActive(OVERWORLD_COMMANDS::ZOOM_IN))      
		camera.zoom(1.002f);
	else if (owCommands.isActive(OVERWORLD_COMMANDS::ZOOM_OUT)) 
		camera.zoom(0.998f);
	else if (owCommands.isActive(OVERWORLD_COMMANDS::ZOOM_IN_FAST)) 
		camera.zoom(1.1f);
	else if (owCommands.isActive(OVERWORLD_COMMANDS::ZOOM_OUT_FAST)) 
		camera.zoom(0.9f);


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

	part1_total = sf::Time::Zero;
	part2_total = sf::Time::Zero;



	std::set<Entity*, std::less<Entity*>, FrameAllocator<Entity*>> entities_updated;

	sf::Clock part2_start;
	sf::Clock part1_start;

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

				//"ent" is in the visibility set but may not be in view
				if(!viewRect.intersects((*ent)->getVisibilityRectangle())) {
					++ent;
					continue;
				}

				//update visible entities and put them in the visible list
				if(entities_updated.count(*ent) != 0) {
					++ent;
					continue;
				}

				part1_start.restart();
				Entity* current_ent = *ent;
				(*ent++)->update(myDeltaTime, true);
				entities_updated.insert(current_ent);

				if(!current_ent->isMarkedForDeletion()) { //TODO : remove if not visible now due to update
					entities_visible.insert(current_ent);
				}

				part1_total += part1_start.restart();
			}
		}
	}

	part2_total += part2_start.restart();

	//handle update of entities that are not drawn:

	for (auto map = newEntityUpdateMaps.begin() ; map != newEntityUpdateMaps.end(); ++map ) {
		
		for(auto ent = (*map)->entities_list().begin();  ent != (*map)->entities_list().end(); ) {
			
			if(isTooFarForUpdate(*ent, entityUpdateRect)) {
				++ent;
				continue;
			}

			if(entities_updated.count(*ent) != 0) {
				++ent;
				continue;
			}

			part1_start.restart();
			Entity* current_ent = *ent;
			(*ent++)->update(myDeltaTime, false);
			entities_updated.insert(current_ent);

			//TODO : add to entities_visible if visible now

			part1_total += part1_start.restart();
		}
	}

	dtCrowd* crowd = navMeshGenerator.getCrowd();
	crowd->update(myDeltaTime/1000.f, &m_agentDebug);


	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		float target[3] = { 0, 0, 0};
		auto v = getMouseWorldPosition();
		target[0] = v.x;
		target[2] = v.y;
		crowdToolState.setMoveTarget(target, false);
	} else if (sf::Mouse::isButtonPressed(sf::Mouse::Right) ) {
		float target[3] = { 0, 0, 0};
		auto v = getMouseWorldPosition();
		target[0] = v.x;
		target[2] = v.y;
		static int i = 2;
		++i;
		std::cout << "Agents: " << i <<'\n';
		crowdToolState.addAgent(target);
	}
}


sf::Vector2f OverworldScene::getMouseWorldPosition() const {
	sf::Vector2i localPosition = sf::Mouse::getPosition(*App);
	auto rect = camera.getViewRect();
	sf::Vector2f v;
	v.x = localPosition.x * camera.getZoom() + rect.left;
	v.y = localPosition.y * camera.getZoom() + rect.top;
	return v;
}

void OverworldScene::draw() {  

	App->clear();

	owDisplay.clearAndSetView(camera.getView());

	//draw the world:
	sf::Clock world_draw;
	for (auto& map : visibleMaps) {
		map->drawLayer(camera.getView(), owDisplay, -1);
	}
	for (auto& map : visibleMaps) {
		map->drawLayer(camera.getView(), owDisplay, 0);
	}

	sf::Time world_draw_time = world_draw.getElapsedTime();

	//draw the entities:
	sf::Clock entities_draw;

	std::vector<Entity*, FrameAllocator<Entity*>> entities_y_sorted;
	entities_y_sorted.reserve(entities_visible.size());

	for(auto& ent : entities_visible) {
		entities_y_sorted.push_back(ent);
	}

	std::sort(entities_y_sorted.begin(), entities_y_sorted.end(), z_orderer);

	for(auto& ent : entities_y_sorted) {
		ent->draw(owDisplay);
	}
	sf::Time entities_draw_time = entities_draw.getElapsedTime();

	if(debug_key_pressed) {
		for(auto& ent : entities_visible) {
			ent->drawDebugInfo(owDisplay);
		}
		drawNavMesh();
	}


	//
	sf::Clock shader_draw;
	Tone t = ZC->getData().isOutside ? 
		OverWorldTone::getToneAt( gameClock.getCurrentTimeOfDay()/3600.f) :
		(ZC->getData().isDark ? OverWorldTone::getDarknessTone() : Tone());

	owDisplay.updateToneParameters(t);

	crowdToolState.handleRender(owDisplay);

	owDisplay.draw(*App);

	if(myState == OverworldSceneState::TRANSITIONING_IN || myState == OverworldSceneState::TRANSITIONING_OUT) {
		sf::RectangleShape shape(sf::Vector2f(metaGameData.resolution.x, metaGameData.resolution.y));
		shape.setPosition(0,0);

		float opacity = 255.f*( float(owTransition.time_remaining_ms) / float(owTransition.total_time_ms));

		if(myState == OverworldSceneState::TRANSITIONING_OUT) {
			opacity = 255.f - opacity;
		}

		sf::Uint8 o = 0;
		if(opacity > 0 && opacity <= 255.f) {
			o = static_cast<sf::Uint8>(opacity);
		} else if(opacity > 255.f) {
			o = 255;
		}
		shape.setFillColor(sf::Color(0,0,0,o));
		App->draw(shape); 
	}

	sf::Time shader_draw_time = shader_draw.restart();


	//infos : 

	const bool redrawOverlayNow = myState == OverworldSceneState::PAUSED || ticks.getTicks(TICKS::_100MS) > 0;

	if(redrawOverlayNow) {
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

		std::stringstream oss;
		int FPS = int(1000/myDeltaTimeAlways);
		oss << "FPS: " << FPS<<  " ent update: " << part1_total.asMilliseconds()  << "ms, ent z sort: " << part2_total.asMilliseconds() <<"ms";
		oss << "\nmap drawing time: " <<  world_draw_time.asMilliseconds() << "ms\tent draw time:" << entities_draw_time.asMilliseconds() << "ms\tfinal shader drawing: "<< shader_draw_time.asMilliseconds();
		oss << "\n" << ZC->getData().name << "\t(" << getGameStateName(myState)<<")";
		overlay->FPStext.setString(oss.str());
	}
	overlay->draw(false);

	//


	App->display();
}

void OverworldScene::changeZone(const std::string& newZC) {

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

void OverworldScene::drawNavMesh() {

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

#include <iostream>
#include <sstream>

#include "OverWorldScene.h"
#include "Tileset.h"
#include "entities/NPC.h"
#include "entities/Collectible.h"
#include "entities/GatewayZC.h"
#include "entities/LightEntity.h"
#include "OverWorldTone.h"
#include "TilePlane.h"
#include "TileAnimator.h"
#include "Overlay.h"
#include "entities/PlayerCharacter.h"
#include "entities/Projectile.h"

#include "utils/SetUnionIterator.h"
#include "utils/FramePagedMemory.h"

#include "MapEntitiesLoader.h"
#include "GameResource.h"
#include "ZoneContainerData.h"
#include "Map.h"

bool z_orderer (const Entity* lhs, const Entity* rhs) {
	return lhs->getPosition().y < rhs->getPosition().y;
}

void OverWorldDisplay::init(const MetaGameData& metaGameData) {

	colorizeShader.loadFromFile(metaGameData.basePath+"shader/colorize.sfx", sf::Shader::Fragment);

	overWorld_texture.create(metaGameData.resolution.x, metaGameData.resolution.y);
	light_texture.create(metaGameData.resolution.x, metaGameData.resolution.y);

	colorizeShader.setParameter("world_tex", overWorld_texture.getTexture());
	colorizeShader.setParameter("light_tex", light_texture.getTexture());

	world_sprite.setTexture(overWorld_texture.getTexture());
	shader_render_state = sf::RenderStates(&colorizeShader);
}

void OverWorldDisplay::draw(sf::RenderWindow& app) {
	overWorld_texture.display();
	light_texture.display();
	app.draw(world_sprite, shader_render_state);
}

void OverWorldDisplay::updateToneParameters(const Tone& t) {
	colorizeShader.setParameter("Alpha", 1 - t.gray);
	colorizeShader.setParameter("ToneColor", t.red, t.green, t.blue);
}

void OverWorldDisplay::clearAndSetView(const sf::View& view) {
	overWorld_texture.clear(sf::Color::Red);
	light_texture.clear(sf::Color(0,0,0,0));

	overWorld_texture.setView(view);
	light_texture.setView(view);
}

OverWorldScene::OverWorldScene(const MetaGameData& metaGameData, GameResource& gr) :
	metaGameData(metaGameData),
	camera(sf::Vector2f(float(metaGameData.resolution.x),float(metaGameData.resolution.y))),
	gameClock(
	metaGameData.start_time_hours*3600 + metaGameData.start_time_minutes*60, 
	metaGameData.clock_speed_factor),
	myTotalTime(0),
	myTotalTimeAlways(0),
	gameResources(gr),
	debug_key_pressed(false)
{ }

OverWorldScene::~OverWorldScene() {
	delete ZC;
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

void OverWorldScene::loadEntities() {
	generateEntityFromFile(ZC->getData().entitiesDataPath , *ZC, gameResources, ticks);
}

void OverWorldScene::onInit() {
	pagedVectorStatic.resetForNewFrame();

	owDisplay.init(metaGameData);
	owCommandsState.init();
	OverWorldTone::init();

	drawMap = false;
	PC_moved = false;

	overlay = new Overlay(*App);

	ZC = new ZoneContainer(metaGameData.firstZonePath, gameResources);

	bindContentToClock();

	loadEntities();

	auto& anim = gameResources.getMoveAnimation("../../ressources/male_walkcycle.png");
	
	PC = new PlayerCharacter(ZC->getData().startingPos, *ZC, ticks, anim, *overlay);
	torchLight = new LightEntity(PC->getSpriteCenter(), *ZC, 300, 20, sf::Color(10,10,10,250));

	lights_updated = 0;
	entities_visible = 0;
}


struct UpdateMapGraphics {

	UpdateMapGraphics(OverWorldCamera& camera, bool checkAnimatedTilesUpdate) :
		camera(camera), checkAnimatedTilesUpdate(checkAnimatedTilesUpdate)
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
		map->updateGraphics(camera, checkAnimatedTilesUpdate);
	}

	OverWorldCamera& camera;
	bool checkAnimatedTilesUpdate;
};



void OverWorldScene::update(int deltaTime) {

	pagedVectorStatic.resetForNewFrame();

	pause_state.beginNewFrame();

	owCommands.pollComands(*App);

	debug_key_pressed = owCommands.isActive(OVERWORLD_COMMANDS::DEBUG) ? !debug_key_pressed : debug_key_pressed;

	if (owCommands.isActive(OVERWORLD_COMMANDS::EXIT) ) {
		close();
	}
	if (owCommands.isActive(OVERWORLD_COMMANDS::PAUSE) ) {
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
	if(pause_state) {
		camera.cameraSetForFrame();
		for(auto& map : loadedMaps) {
			map->updateGraphics(camera, ZC->getTileset().getNeedUpdating());
		}
		myDeltaTime = 0;
		return;
	}
	
	//this part will be updated only if the gameplay isn't paused:

	camera.newFrame();
	ticks.update(deltaTime);

	myDeltaTime = deltaTime;
	myTotalTime += deltaTime;
	gameClock.update(deltaTime);
	

	callbackSystem.callAllUpToTime(myTotalTime);


	if(owCommands.isActive(OVERWORLD_COMMANDS::FLASHLIGHT)) {
		owCommandsState.flashlight_on = !owCommandsState.flashlight_on;
		torchLight->setOn(owCommandsState.flashlight_on) ;
	}

	if(owCommands.isActive(OVERWORLD_COMMANDS::ACTIVATE)) {
		new Projectile(PC->getPosition(), *ZC);
		new Projectile(PC->getPosition(), *ZC);
		new Projectile(PC->getPosition(), *ZC);
		new Projectile(PC->getPosition(), *ZC);
		new Projectile(PC->getPosition(), *ZC);
		new Projectile(PC->getPosition(), *ZC);
	}

	if(owCommands.isActive(OVERWORLD_COMMANDS::ZOOM_RESET)) {
		camera.resetSize();
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

	if(torchLight->isOn_()) {
		torchLight->setPosition(PC->getSpriteCenter());
	}

	if(!PC_moved)
		PC->isMoving = false;

	camera.setCenter(PC->getSpriteCenter());

	if (owCommands.isActive(OVERWORLD_COMMANDS::ZOOM_IN))      
		camera.zoom(1.002f);
	else if (owCommands.isActive(OVERWORLD_COMMANDS::ZOOM_OUT)) 
		camera.zoom(0.998f);
	else if (owCommands.isActive(OVERWORLD_COMMANDS::ZOOM_IN_FAST)) 
		camera.zoom(0.9f);
	else if (owCommands.isActive(OVERWORLD_COMMANDS::ZOOM_OUT_FAST)) 
		camera.zoom(1.1f);

	if(owCommands.isActive(OVERWORLD_COMMANDS::DISPLAY_MAP)) 
		drawMap = true;


	camera.cameraSetForFrame();

	//update list of visible maps :
	std::set<Map*> newVisibleMaps = ZC->getCollidingMaps(camera.getViewRect());
	UpdateMapGraphics updater(camera, ZC->getTileset().getNeedUpdating());
	iterate_over_union(newVisibleMaps, loadedMaps, updater);
	loadedMaps = std::move(newVisibleMaps);

	//manage entities:
	ZC->deleteElements();
	delete lights_updated;
	lights_updated = new std::set<LightEntity*,  std::less<LightEntity*>,  FramePagedMemory<LightEntity*>>();
	delete entities_visible;
	entities_visible = new std::vector<Entity* ,  FramePagedMemory<Entity*>>();

	clock_t start;

	start = clock();
	part1_total = 0;
	part2_total = 0;

	clock_t part1_start;
	clock_t part2_start;

	std::set<Entity* , std::less<Entity*>,  FramePagedMemory<Entity*>> entities_updated;

	//std::cout <<  "\nstart entity loop\n";
	for (auto map = loadedMaps.begin() ; map != loadedMaps.end(); ++map ) {
		//std::cout <<  "\n\n";
		for(auto ent = (*map)->entities_list().begin();  ent != (*map)->entities_list().end(); ) {
			//std::cout << *ent << '\t';
			if(entities_updated.count(*ent) == 0) { 
				assert((*ent)->getType() != EntityType::LIGHT);
				part1_start = clock();
				auto current_ent = *ent;
				(*ent++)->update(myDeltaTime);
				entities_updated.insert(current_ent);
				part1_total += clock() - part1_start;
				part2_start = clock();
				if( camera.getViewRect().intersects(current_ent->getVisibilityRectangle())) {
					entities_visible->push_back(current_ent);
				}
				part2_total += clock() - part2_start;
			}
			else {
				++ent;
			}
		}
	}
	//std::cout <<  "\nend entity loop\n";
	part2_start = clock();
	std::sort(entities_visible->begin(), entities_visible->end(), z_orderer);
	part2_total += clock() - part2_start;

	for (auto map = loadedMaps.begin() ; map != loadedMaps.end(); ++map ) {

		for(std::set<LightEntity*>::iterator it_ent = (*map)->lights_list().begin(); it_ent != (*map)->lights_list().end(); ) {

			if(lights_updated->count((*it_ent)) == 0) { 
				LightEntity* entity_ptr = *it_ent;

				assert(entity_ptr->getType() == EntityType::LIGHT);
				part1_start = clock();
				(*it_ent++)->update(myDeltaTime);

				if( camera.getViewRect().intersects(entity_ptr->getVisibilityRectangle())) {
					lights_updated->insert(entity_ptr);
				}
				part1_total += clock() - part1_start;
			}
			else   ++it_ent;
		}
	}
}


void OverWorldScene::draw() {  

	App->clear();

	// --- overlay info 
	if(ticks.getTicks(TICKS::_100MS) > 0) {
		std::stringstream text;
		text << EntitySet::entitySetStorage.size_in_use() << " entities set in use, "<< EntitySet::entitySetStorage.size_available()<< " extra available\n";
	
		text << loadedMaps.size() << " maps in view\n";
		if (loadedMaps.size() < 20) {
			for (auto map= loadedMaps.begin() ; map != loadedMaps.end(); ++map ) {
				sf::Vector2i v = (*map)->getMapCoords();
				text << "map <"<< v.x <<","<< v.y <<"> in view\n";
			}
		}

		overlay->MapInfosText.setString(text.str()+'\n'+gameClock.getCurrentTimeOfDayStr());
	}
	// --- overlay info 


	owDisplay.clearAndSetView(camera.getView());
	//draw the world:
	clock_t world_draw = clock();
	for (auto map = loadedMaps.begin() ; map != loadedMaps.end(); ++map ) {
		(*map)->drawLayer(camera.getView(), owDisplay, 0);
	}
	world_draw = clock() - world_draw;

	//draw the entities:
	clock_t entities_draw = clock();
	for(auto& ent : *entities_visible) {
		ent->draw(owDisplay);
	}

	for(auto& light : *lights_updated) {
		light->draw(owDisplay);
	}

	entities_draw = clock() - entities_draw;

#ifdef _DEBUG
	if(debug_key_pressed) {
		for(auto& ent : *entities_visible) {
			ent->drawDebugInfo(owDisplay);
		}
	}
#endif


	//
	clock_t shader_draw = clock();
	Tone t = ZC->getData().isOutside ? 
		OverWorldTone::getToneAt( gameClock.getCurrentTimeOfDay()/3600.f) :
		(ZC->getData().isDark ? OverWorldTone::getDarknessTone() : Tone());

	owDisplay.updateToneParameters(t);

	owDisplay.draw(*App);
	shader_draw = clock() - shader_draw;


	//infos : 
	if(true || ticks.getTicks(TICKS::_100MS) > 0) {
		std::stringstream oss;
		int FPS = int(1000/myDeltaTimeAlways);
		oss << "FPS: " << FPS<<  " ent update: " << 1000.f*(float)part1_total/CLOCKS_PER_SEC  << "ms, ent z sort: " << 1000.f*(float)part2_total/CLOCKS_PER_SEC <<"ms";
		oss << "\nmap drawing time: " << 1000.f*(float)world_draw/CLOCKS_PER_SEC<< "ms\tent draw time:" << 1000.f*(float)entities_draw/CLOCKS_PER_SEC << "ms\tfinal shader drawing: "<<1000.f*(float)shader_draw/CLOCKS_PER_SEC;
		oss << "\n" << ZC->getData().name;
		overlay->FPStext.setString(oss.str());
	}
	overlay->draw(drawMap);
	drawMap = false;

	//

	App->display();
}

void OverWorldScene::changeZone(const std::string& newZC) {
	
	auto ZC2 = new ZoneContainer(newZC, gameResources);
	PC->teleportTo(ZC2->getData().startingPos, ZC2);
	unbindContentToClock();
	delete ZC;
	ZC = ZC2;
	loadedMaps.clear();
	loadEntities();
	bindContentToClock();
}
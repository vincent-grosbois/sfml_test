#include <iostream>
#include <sstream>



#include "OverWorldScene.h"
#include "Tileset.h"
#include "NPC.h"
#include "Collectible.h"
#include "GatewayZC.h"
#include "LightEntity.h"
#include "OverWorldTone.h"
#include "TilePlane.h"
#include "TileAnimator.h"
#include "Overlay.h"
#include "PlayerCharacter.h"

#include "utils/SetUnionIterator.h"

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
	gameResources(gr)
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

		anim->createPeriodicCallback(next, 400, callbackSystem, changeAnimatedTileFrame);
	}
}

void OverWorldScene::unbindContentToClock() {

	for(auto anim : ZC->getTileset().get_tile_animators()) {
		anim->removePendingCallbacks();
	}
}

void OverWorldScene::loadEntities() {
	generateEntityFromFile(ZC->getData().entitiesDataPath , *ZC, gameResources);
}

void OverWorldScene::onInit() {

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
	PC = new PlayerCharacter(ZC->getData().startingPos, *ZC, anim, *overlay);
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

	myDeltaTime = deltaTime;
	myTotalTime += deltaTime;

	ZC->getTileset().setNeedUpdating(false);
	camera.newFrame();

	gameClock.update(deltaTime);
	ticks.update(deltaTime);

	callbackSystem.callAllUpToTime(myTotalTime);

	bool debug = owCommands.isActive(OVERWORLD_COMMANDS::DEBUG);

	owCommands.update(*App);

	if (owCommands.isActive(OVERWORLD_COMMANDS::EXIT) ) {
		close();
	}

	if(owCommands.isActive(OVERWORLD_COMMANDS::FLASHLIGHT)) {
		owCommandsState.flashlight_on = !owCommandsState.flashlight_on;
	}

	if(owCommands.isActive(OVERWORLD_COMMANDS::ACTIVATE)) {
		PC->activateThings();
	}

	if(owCommands.isActive(OVERWORLD_COMMANDS::ZOOM_RESET)) {
		camera.resetSize();
	}

	PC_moved = false;

	if (owCommands.isActive(OVERWORLD_COMMANDS::MOVE_UP) ) {  
		PC->draw(deltaTime, DIRECTION::UP, 0, !owCommandsState.already_pressed_u, debug);
		PC_moved = true;
		owCommandsState.already_pressed_u = true;
	} else {
		owCommandsState.already_pressed_u = false;
	}

	if (owCommands.isActive(OVERWORLD_COMMANDS::MOVE_DOWN) )  {
		PC->draw(deltaTime, DIRECTION::DOWN, 0, !owCommandsState.already_pressed_d, debug);
		PC_moved = true;
		owCommandsState.already_pressed_d= true;
	}else {
		owCommandsState.already_pressed_d = false;
	}

	if (owCommands.isActive(OVERWORLD_COMMANDS::MOVE_LEFT) )  {
		PC->draw(deltaTime, DIRECTION::LEFT, 0, !owCommandsState.already_pressed_l, debug);
		owCommandsState.already_pressed_l= true;
		PC_moved = true;
	}else {
		owCommandsState.already_pressed_l = false;
	}

	if (owCommands.isActive(OVERWORLD_COMMANDS::MOVE_RIGHT) )  { 
		PC->draw(deltaTime, DIRECTION::RIGHT, 0, !owCommandsState.already_pressed_r, debug);
		PC_moved = true;
		owCommandsState.already_pressed_r= true;
	}else {
		owCommandsState.already_pressed_r = false;
	}

	if (owCommands.isActive(OVERWORLD_COMMANDS::MOVE_UP_FAST) )  {  
		PC->draw(deltaTime*10, DIRECTION::UP, 0, owCommandsState.already_pressed_u, debug);
	}

	if (owCommands.isActive(OVERWORLD_COMMANDS::MOVE_DOWN_FAST))  {
		PC->draw(deltaTime*10, DIRECTION::DOWN, 0, owCommandsState.already_pressed_u, debug);
	}

	if (owCommands.isActive(OVERWORLD_COMMANDS::MOVE_LEFT_FAST))  {
		PC->draw(deltaTime*10, DIRECTION::LEFT, 0, owCommandsState.already_pressed_u, debug);
	}

	if (owCommands.isActive(OVERWORLD_COMMANDS::MOVE_RIGHT_FAST))  { 
		PC->draw(deltaTime*10, DIRECTION::RIGHT, 0,owCommandsState.already_pressed_u, debug);
	}

	/*if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))  { 
		ZC->dumpLoadedTiles();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))  { 
		changeZoneContainer("../../ressources/overworld.txt");
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))  { 
		changeZoneContainer("../../ressources/cave.txt");
	}*/

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

	ZC->deleteElements();

	clock_t start, end, end1;

	start = clock();
	clock_t part1_total = 0;
	clock_t part2_total = 0;

	clock_t part1_start;
	clock_t part2_start;
	std::set<Entity*> entities_updated;
	std::set<LightEntity*> lights_updated;
	std::vector<Entity*> entities_v;

	for (auto map = loadedMaps.begin() ; map != loadedMaps.end(); ++map ) {

		for(std::set<Entity*>::iterator it_ent = (*map)->entities_list().begin(); it_ent != (*map)->entities_list().end(); ) {

			if(entities_updated.count((*it_ent)) == 0) { 
				Entity* entity_ptr = *it_ent;

				assert(entity_ptr->getType() != EntityType::LIGHT);
				part1_start = clock();
				(*it_ent++)->update(myDeltaTime);
				entities_updated.insert(entity_ptr);
				part1_total += clock() - part1_start;
				part2_start = clock();
				if( camera.getViewRect().intersects(entity_ptr->getVisibilityRectangle())) {
					entities_v.push_back(entity_ptr);
				}
				part2_total += clock() - part2_start;
			}
			else   ++it_ent;
		}
	}

	std::sort(entities_v.begin(), entities_v.end(), z_orderer);

	for (auto map = loadedMaps.begin() ; map != loadedMaps.end(); ++map ) {

		for(std::set<LightEntity*>::iterator it_ent = (*map)->lights_list().begin(); it_ent != (*map)->lights_list().end(); ) {

			if(lights_updated.count((*it_ent)) == 0) { 
				LightEntity* entity_ptr = *it_ent;

				assert(entity_ptr->getType() == EntityType::LIGHT);
				part1_start = clock();
				(*it_ent++)->update(myDeltaTime);

				if( camera.getViewRect().intersects(entity_ptr->getVisibilityRectangle())) {
					lights_updated.insert(entity_ptr);
				}
				part1_total += clock() - part1_start;
			}
			else   ++it_ent;
		}
	}

	end = clock();
	end1 = clock();

	//draw the world:
	clock_t world_draw = clock();
	for (auto map = loadedMaps.begin() ; map != loadedMaps.end(); ++map ) {
		(*map)->drawLayer(camera.getView(), owDisplay, 0);
	}
	world_draw = clock() - world_draw;

	//draw the entities:
	clock_t entities_draw = clock();
	for(auto& ent : entities_v) {
		ent->draw(ticks.getTicks(TICKS::_250MS), owDisplay);
	}

	for(auto& light : lights_updated) {
		light->draw(ticks.getTicks(TICKS::_250MS), owDisplay);
	}

	entities_draw = clock() - entities_draw;

#ifdef _DEBUG
	for(auto entities_v_it = entities_v.begin() ; entities_v_it != entities_v.end(); ++entities_v_it) {
		(*entities_v_it)->drawCollisionBox(owDisplay);
	}
#endif


	//
	float radius = 300.f;
	float deuxpi = 2*3.14159265f;
	float angle = 0.f;
	if(owCommandsState.flashlight_on) {
		int sides = 20;
		sf::VertexArray lightZone(sf::TrianglesFan, sides+2);
		lightZone[0].position = PC->getSpriteCenter();
		lightZone[0].color = sf::Color(10, 0, 0, 250);
		for(int i = 1; i<sides+2; ++i) {

			lightZone[i].position = PC->getSpriteCenter() + sf::Vector2f(radius*cos(angle),radius*sin(angle));
			lightZone[i].color = sf::Color(0, 0, 0, 0);

			angle += deuxpi/sides;
		}
		owDisplay.light_texture.draw(lightZone, sf::RenderStates(sf::BlendAdd));
	}

	//
	clock_t shader_draw = clock();
	Tone t = ZC->getData().isOutside ? 
		OverWorldTone::getToneAt( gameClock.getCurrentTimeOfDay()/3600.f) :
		(ZC->getData().isDark ? OverWorldTone::getDarknessTone() : Tone());

	owDisplay.updateToneParameters(t);

	owDisplay.draw(*App);
	shader_draw = clock() - shader_draw;


	//infos : 
	if(ticks.getTicks(TICKS::_100MS) > 0) {
		std::stringstream oss;
		int FPS = int(1000/myDeltaTime);
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

void OverWorldScene::changeZoneContainer(const std::string& newZC) {
	
	auto ZC2 = new ZoneContainer(newZC, gameResources);
	PC->teleportTo(ZC2->getData().startingPos, ZC2);
	unbindContentToClock();
	delete ZC;
	ZC = ZC2;
	loadedMaps.clear();
	loadEntities();
	bindContentToClock();
}
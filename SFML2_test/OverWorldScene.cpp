#include <iostream>
#include <sstream>

#include "OverWorldScene.hpp"
#include "Tileset.hpp"
#include "NPC.h"
#include "Collectible.hpp"
#include "GatewayZC.hpp"
#include "LightEntity.hpp"
#include "OverWorldTone.hpp"
#include "TilePlane.hpp"
#include "TileAnimator.h"
#include "Overlay.hpp"
#include "PlayerCharacter.hpp"

#include "utils/SetUnionIterator.h"

#include "MapEntitiesLoader.h"

bool z_orderer2 (const Entity* lhs, const Entity* rhs) {
	return lhs->getPosition().y < rhs->getPosition().y;
}

void OverWorldDisplay::init(const MetaGameData& metaGameData) {
	colorizeShader.loadFromFile("../ressources/shader/colorize.sfx", sf::Shader::Fragment);

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

OverWorldScene::~OverWorldScene() {
	delete ZC;
}

void OverWorldScene::initGlobalContent() {

	//tileset = &ZC->getTileset();

	for(auto anim : ZC->getTileset().get_tile_animators()) {
		using namespace std::placeholders;
		auto func = std::bind(&TileAnimator::process_frame, anim, _1);
		ticks.addCallback(func , anim->base_tick);
	}

	std::cout << "tileset loaded\n";

	for (int i =1; i<=1; ++i) {
		std::string str("../ressources/sprites/");

		std::ostringstream out;
		out << i;
		if (out.str().size() == 1) {
			str += "00";
		}
		else if(out.str().size() == 2) {
			str += "0";
		}
		str += out.str();
		str += ".png";
		owResources.walking_animations.emplace_back(MoveAnimation(str));
	}

	owResources.walking_animations.emplace_back( MoveAnimation("../ressources/male_walkcycle.png") );
}

void OverWorldScene::loadEntities() {
	generateEntityFromFile(ZC->getEntitiesDataPath() , *ZC, owResources);
}

void OverWorldScene::onInit() {

	owDisplay.init(metaGameData);
	owCommands.init();
	OverWorldTone::init();

	DrawMap = false;
	PC_moved = false;

	overlay = new Overlay(*App);

	ZC = new ZoneContainer(zone);

	initGlobalContent();

	loadEntities();

	PC = new PlayerCharacter(ZC->startingPos, *ZC, owResources.walking_animations.back(), *overlay);
}


struct UpdateMapGraphics {

	UpdateMapGraphics(OverWorldCamera& camera) :
		camera(camera) { }

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
		map->updateGraphics(camera);
	}

	OverWorldCamera& camera;
};

void OverWorldScene::update(int deltaTime) {

	camera.newFrame();

	myDeltaTime = deltaTime;

	gameClock.update(deltaTime);
	ticks.update(deltaTime);

	bool debug = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);

	// Process events
	sf::Event Event_;
	while (App->pollEvent(Event_))
	{
		if (Event_.type == sf::Event::Closed)
			close();

		if (Event_.type == sf::Event::KeyPressed)  {

			if(Event_.key.code == sf::Keyboard::Escape)
				close();

			if (Event_.key.code ==  sf::Keyboard::E)  {  
				PC->activateThings();
			}

			if (Event_.key.code == sf::Keyboard::F)  {  
				owCommands.flashlight_on = !owCommands.flashlight_on;
			}

			if (Event_.key.code == sf::Keyboard::V) 
				camera.setSize(float(metaGameData.resolution.x),float(metaGameData.resolution.y)) ;
		}
	}

	PC_moved = false;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ) {  
		PC->draw(deltaTime, DIRECTION::UP, 0, !owCommands.already_pressed_u, debug);
		PC_moved = true;
		owCommands.already_pressed_u = true;
	} else {
		owCommands.already_pressed_u = false;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  {
		PC->draw(deltaTime, DIRECTION::DOWN, 0, !owCommands.already_pressed_d, debug);
		PC_moved = true;
		owCommands.already_pressed_d= true;
	}else {
		owCommands.already_pressed_d = false;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  {
		PC->draw(deltaTime, DIRECTION::LEFT, 0, !owCommands.already_pressed_l, debug);
		owCommands.already_pressed_l= true;
		PC_moved = true;
	}else {
		owCommands.already_pressed_l = false;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))  { 
		PC->draw(deltaTime, DIRECTION::RIGHT, 0, !owCommands.already_pressed_r, debug);
		PC_moved = true;
		owCommands.already_pressed_r= true;
	}else {
		owCommands.already_pressed_r = false;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))  {  
		PC->draw(deltaTime*10, DIRECTION::UP, 0, owCommands.already_pressed_u, debug);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))  {
		PC->draw(deltaTime*10, DIRECTION::DOWN, 0, owCommands.already_pressed_u, debug);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))  {
		PC->draw(deltaTime*10, DIRECTION::LEFT, 0, owCommands.already_pressed_u, debug);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))  { 
		PC->draw(deltaTime*10, DIRECTION::RIGHT, 0,owCommands.already_pressed_u, debug);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))  { 
		ZC->dumpLoadedTiles();
	}

	if(!PC_moved)
		PC->isMoving = false;

	camera.setCenter(PC->getSpriteCenter());

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))      
		camera.zoom(1.002f);
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) 
		camera.zoom(0.998f);
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) 
		camera.zoom(0.9f);
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) 
		camera.zoom(1.1f);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab))
		DrawMap = true;


	camera.cameraSetForFrame();

	std::set<Map*> mapsOnScreen = ZC->getCollidingMaps(camera.getViewRect());

	UpdateMapGraphics updater(camera);

	iterate_over_union(mapsOnScreen, loadedMaps, updater);

	loadedMaps = std::move(mapsOnScreen);
}


void OverWorldScene::draw() {  

	App->clear();

	// --- overlay info 
	if(ticks.getTicks(TICKS::_100MS) > 0) {
		std::stringstream text;
		text << EntitySet::entitySetStorage.size_in_use() << " entities set in use, "<< EntitySet::entitySetStorage.size_available()<< " extra available\n";
		float resolution_ratio = float(metaGameData.resolution.x) / metaGameData.resolution.y;
		int sqrt_ = int(sqrt(MapElement::storage_size_in_use() / resolution_ratio));
		text << MapElement::storage_size_in_use() << " sprites ("<<int(sqrt_*resolution_ratio)<<"x"<<sqrt_<<") in use, "<< MapElement::storage_size_available() << " extra available\n";

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
	std::vector<Entity*> entities_v;

	for (auto map = loadedMaps.begin() ; map != loadedMaps.end(); ++map ) {

		for(std::set<Entity*>::iterator it_ent = (*map)->entities_on_map.begin(); it_ent != (*map)->entities_on_map.end(); ) {

			if( (*it_ent)->getAsleep() ) { 
				++it_ent;
			}
			else if(entities_updated.count((*it_ent)) == 0) { 
				Entity* entity_ptr;
				entity_ptr = *it_ent;
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

	std::sort(entities_v.begin(), entities_v.end(), z_orderer2);
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
	for(auto entities_v_it = entities_v.begin() ; entities_v_it != entities_v.end(); ++entities_v_it) {
		(*entities_v_it)->draw(ticks.getTicks(TICKS::_250MS), owDisplay);
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
	if(owCommands.flashlight_on) {
		int sides = 20;
		sf::VertexArray lightZone(sf::TrianglesFan, sides+2);
		lightZone[0].position = PC->getSpriteCenter();
		lightZone[0].color = sf::Color(10, 0, 0, 250);
		for(int i = 1; i<sides+2; ++i) {

			lightZone[i].position = PC->getSpriteCenter() + sf::Vector2f(radius*cos(angle),radius*sin(angle));
			lightZone[i].color = sf::Color(0, 0, 0, 0);

			angle += deuxpi/sides;
		}
		owDisplay.light_texture.draw(lightZone, sf::RenderStates(sf::BlendMode::BlendAdd));
	}

	//
	clock_t shader_draw = clock();
	Tone t = ZC->isOutside ? 
		OverWorldTone::getToneAt( gameClock.getCurrentTimeOfDay()/3600.f) :
		(ZC->isDark ? OverWorldTone::getDarknessTone() : Tone());
	owDisplay.updateToneParameters(t);

	owDisplay.draw(*App);
	shader_draw = clock() - shader_draw;


	//infos : 
	if(ticks.getTicks(TICKS::_100MS) > 0) {
		std::stringstream oss;
		int FPS = int(1000/myDeltaTime);
		oss << "FPS: " << FPS<<  " part1: " << 1000.f*(float)part1_total/CLOCKS_PER_SEC  << "ms, part2: " << 1000.f*(float)part2_total/CLOCKS_PER_SEC <<"ms";
		oss << "\tmap drawing time: " << 1000.f*(float)world_draw/CLOCKS_PER_SEC<< "ms\tent draw time:" << 1000.f*(float)entities_draw/CLOCKS_PER_SEC << "ms\tfinal shader drawing: "<<1000.f*(float)shader_draw/CLOCKS_PER_SEC;
		overlay->FPStext.setString(oss.str());
	}
	overlay->draw(DrawMap);
	DrawMap = false;

	//

	App->display();
}
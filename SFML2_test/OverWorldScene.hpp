#pragma once
#include <iostream>
#include <set>

#include "gamescene.hpp"
#include "MoveAnimation.h"
#include "GameClock.hpp"
#include "GameTicks.hpp"

#include "OverWorldCamera.h"
#include "ZoneContainerData.h"

struct Tone;
class PlayerCharacter;
class Overlay;
class Map;
class ZoneContainer;
class Tileset;

struct OverWorldDisplay 
{
	sf::Shader colorizeShader;

	sf::RenderTexture overWorld_texture;
	sf::RenderTexture light_texture;

	sf::Sprite world_sprite;
	sf::RenderStates shader_render_state;

	void init(const MetaGameData& metaGameData);
	void draw(sf::RenderWindow& app);
	void updateToneParameters(const Tone& t);

	void clearAndSetView(const sf::View& view);
};

class EntityPhysical;


struct OverWorldCommands {
	bool flashlight_on;

	bool already_pressed_u;
	bool already_pressed_d ;
	bool already_pressed_l;
	bool already_pressed_r;

	void init() {
		flashlight_on = false;
		already_pressed_u = true;
		already_pressed_d = true;
		already_pressed_l = true;
		already_pressed_r = true;
	}
};

struct OverWorldResources {
		std::vector<MoveAnimation> walking_animations;
};

class GameResource;

class OverWorldScene : public GameScene
{
public:
	
	virtual ~OverWorldScene() override;

	OverWorldScene(const MetaGameData& metaGameData, GameResource& gr) :
		metaGameData(metaGameData),
		zone(metaGameData.firstZonePath),
		camera(
		sf::View(sf::Vector2f(0,0), 
		sf::Vector2f(float(metaGameData.resolution.x),float(metaGameData.resolution.y))) 
		),
		gameClock(
		metaGameData.start_time_hours*3600 + metaGameData.start_time_minutes*60, 
		metaGameData.clock_speed_factor),
		myTotalTime(0),
		gameResource(gr)
	{ };

	virtual void draw() override;

	virtual void update (int deltatime_ms) override;

	virtual void onInit() override;

private:
	MetaGameData metaGameData;

	std::string zone;

	OverWorldResources owResources;

	GameClock gameClock;
	bool DrawMap;


	bool PC_moved;

	int myDeltaTime;

	Overlay* overlay;
	PlayerCharacter* PC;
	OverWorldCamera camera;

	std::set<Map*> loadedMaps;
	ZoneContainer* ZC;

	OverWorldDisplay owDisplay;
	OverWorldCommands owCommands;

	GameTicks ticks;

	void initGlobalContent();
	void loadEntities();

	int myTotalTime;

	GameResource& gameResource;
};


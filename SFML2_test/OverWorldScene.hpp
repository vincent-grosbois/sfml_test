#pragma once
#include <iostream>
#include <set>

#include "gamescene.hpp"
#include "MoveAnimation.h"
#include "GameClock.hpp"
#include "GameTicks.hpp"
#include "OverWorldCamera.h"

#include "CallBackSystem.h"

struct Tone;
class PlayerCharacter;
class Overlay;
class Map;
class ZoneContainer;
class Tileset;
class MetaGameData;

struct OverWorldDisplay 
{
	sf::Shader colorizeShader;

	sf::RenderTexture overWorld_texture;
	sf::RenderTexture light_texture;

	sf::Sprite world_sprite;
	sf::RenderStates shader_render_state;

	void init(const MetaGameData& metaGameData);
	void draw(sf::RenderWindow& app);
	void updateToneParameters(const Tone& tone);

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

class GameResource;

class OverWorldScene : public GameScene
{
public:
	
	virtual ~OverWorldScene() override;

	OverWorldScene(const MetaGameData& metaGameData, GameResource& gr);

	virtual void draw() override;

	virtual void update (int deltatime_ms) override;

	virtual void onInit() override;

private:
	const MetaGameData& metaGameData;

	GameResource& gameResources;

	OverWorldCamera camera;
	OverWorldDisplay owDisplay;
	OverWorldCommands owCommands;

	CallBackSystem callbackSystem;

	GameClock gameClock;
	GameTicks ticks;

	bool DrawMap;
	bool PC_moved;

	Overlay* overlay;
	PlayerCharacter* PC;

	std::set<Map*> loadedMaps;
	ZoneContainer* ZC;

	int myDeltaTime;
	int myTotalTime;

private:
	void changeZoneContainer(const std::string& newZC);
	void bindContentToClock();
	void unbindContentToClock();
	void loadEntities();
};


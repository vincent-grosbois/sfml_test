#pragma once
#include <iostream>
#include <set>

#include "gamescene.h"
#include "MoveAnimation.h"
#include "GameClock.h"
#include "GameTicks.h"
#include "OverWorldCamera.h"

#include "CallBackSystem.h"
#include "OverworldCommands.h"
#include "OverworldGameStateRequest.h"

#include "utils/FramePagedMemory.h"
#include <SFML/System/Vector2.hpp>

struct Tone;
class PlayerCharacter;
class Overlay;
class Map;
class ZoneContainer;
class Tileset;
class MetaGameData;
class LightEntity;
class Entity;

struct OverWorldDisplay 
{
	sf::Shader colorizeShader;

	sf::RenderTexture overWorld_texture;
	sf::RenderTexture light_texture;

	sf::RenderTexture debug_texture;

	sf::Sprite world_sprite;
	sf::Sprite debug_sprite;

	sf::RenderStates shader_render_state;

	void init(const MetaGameData& metaGameData);
	void draw(sf::RenderWindow& app);
	void updateToneParameters(const Tone& tone);

	void clearAndSetView(const sf::View& view);
};

class EntityPhysical;


struct OverWorldCommandsState {
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

struct PauseDelayedStruct {
private:
	bool unpause_on_next_frame;
	bool pause_on_next_frame;
	bool paused;

public:
	operator bool() const {
		return paused;
	}

	PauseDelayedStruct() :
		paused(false),
		unpause_on_next_frame(false),
		pause_on_next_frame(false) {
	}

	bool isPaused() const {
		return paused;
	}

	void pauseOnNextFrame() {
		pause_on_next_frame = true;
		unpause_on_next_frame = false;
	}

	void unpauseOnNextFrame() {
		pause_on_next_frame = false;
		unpause_on_next_frame = true;
	}

	void beginNewFrame() {
		if(pause_on_next_frame)  {
			unpause_on_next_frame = false;
			pause_on_next_frame = false;
			paused = true;
		} else if (unpause_on_next_frame) {
			unpause_on_next_frame = false;
			pause_on_next_frame = false;
			paused = false;
		}
	}

};




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
	OverworldCommands owCommands;
	OverWorldCommandsState owCommandsState;
	OverworldGameStateRequest owStateChangeRequest;

	CallBackSystem callbackSystem;		 // <* Callback system used only when the game is paused
	CallBackSystem callbackSystemAlways; // <* Callback system used even when the game is paused

	GameClock gameClock;
	GameTicks ticks;

	bool PC_moved;

	Overlay* overlay;
	PlayerCharacter* PC;

	std::set<Map*> visibleMaps; // <* maps that intersects with the current camera
	ZoneContainer* ZC;

	int myDeltaTime;
	int myTotalTime;
	
	int myDeltaTimeAlways;
	int myTotalTimeAlways;

	PauseDelayedStruct pause_state;

	LightEntity* torchLight;

	bool debug_key_pressed;

	std::set<Entity*> entities_visible;

	
	clock_t part1_total;
	clock_t part2_total;

private:
	void changeZone(const std::string& newZC);
	void bindContentToClock();
	void unbindContentToClock();
	void loadEntities(bool already_created);
};


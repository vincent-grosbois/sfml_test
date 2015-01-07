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

#include "utils/FrameAllocator.h"
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

struct WaveParameters {
	radian_t wave_angle ;
	float wave_time_period ; // time period in seconds
	float wave_spatial_period; // spatial period in pixel
	float wave_amplitude;// in pixels

	WaveParameters();
};

struct OverWorldDisplay 
{
	sf::Shader colorizeShader;
	sf::Shader waterShader;

	sf::RenderTexture overWorld_texture;
	sf::RenderTexture light_texture;

	sf::RenderTexture debug_texture;

	sf::Sprite world_sprite;
	sf::Sprite debug_sprite;

	sf::RenderStates shader_render_state;

	WaveParameters myWaveParameters;

	void init(const MetaGameData& metaGameData);
	void draw(sf::RenderWindow& app);
	void updateToneParameters(const Tone& tone);

	void updateWaterParameters(int time_ms);
	void changeWaterParameters(const WaveParameters& waveParameters);

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
		flashlight_on = true;
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

struct OverWorldTransition {

	int time_remaining_ms;
	int total_time_ms;
	std::string ZoneId;
	sf::Vector2f pos;

	bool isActive() const {
		return time_remaining_ms > 0;
	}

	OverWorldTransition():
		time_remaining_ms(-1),
		total_time_ms(500)
	{

	}

};

enum class OverworldSceneState {
	NORMAL,
	PAUSED,
	TRANSITIONING_OUT,
	TRANSITIONING_IN
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
	OverworldSceneState myState;

	const MetaGameData& metaGameData;

	GameResource& gameResources;

	OverWorldCamera camera;
	OverWorldDisplay owDisplay;
	OverworldCommands owCommands;
	OverWorldCommandsState owCommandsState;
	OverworldGameStateRequest owStateChangeRequest;
	OverWorldTransition owTransition;

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


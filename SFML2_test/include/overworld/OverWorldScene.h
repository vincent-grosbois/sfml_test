#pragma once
#include <iostream>
#include <set>

#include "gamescene.h"
#include "MoveAnimation.h"
#include "GameClock.h"
#include "GameTicks.h"
#include "OverworldCamera.h"

#include "CallBackSystem.h"
#include "OverworldCommands.h"
#include "OverworldGameStateRequest.h"

#include "utils/FrameAllocator.h"
#include <SFML/System/Vector2.hpp>

#include "../NavMeshGenerator.h"

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

struct OverworldDisplay 
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

	void updateWaterMovement(int time_ms);
	void changeWaterParameters(const WaveParameters& waveParameters);

	void clearAndSetView(const sf::View& view);
};

class EntityPhysical;
class GameResource;

struct OverworldTransition {

	int time_remaining_ms;
	int total_time_ms;
	std::string ZoneId;
	sf::Vector2f pos;

	bool isActive() const {
		return time_remaining_ms > 0;
	}

	OverworldTransition():
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

class OverworldScene : public GameScene
{
public:
	OverworldScene(const MetaGameData& metaGameData, GameResource& gr);	
	virtual ~OverworldScene() override;

	virtual void draw() override;
	virtual void update (int deltatime_ms) override;
	virtual void onInit() override;

private:
	OverworldSceneState myState;
	OverworldGameStateRequest owStateChangeRequest;

	const MetaGameData& metaGameData;

	GameResource& gameResources;

	OverworldCamera camera;
	OverworldDisplay owDisplay;
	OverworldCommands owCommands;

	OverworldTransition owTransition;

	CallBackSystem callbackSystem;		 // <* Callback system used only when the game is paused
	CallBackSystem callbackSystemAlways; // <* Callback system used even when the game is paused

	GameClock gameClock;
	GameTicks ticks;

	Overlay* overlay;

	PlayerCharacter* PC;
	LightEntity* torchLight;

	std::set<Map*> visibleMaps; // <* maps that intersects with the current camera
	ZoneContainer* ZC;

	int myDeltaTime;
	int myTotalTime;
	
	int myDeltaTimeAlways;
	int myTotalTimeAlways;


	bool debug_key_pressed;

	std::set<Entity*> entities_visible;

	
	sf::Time part1_total;
	sf::Time part2_total;

	//recast / detour related
	RecastBuildContext buildContext;

	NavMeshGenerator  navMeshGenerator;
	CrowdTool crowdTool;
	CrowdToolState crowdToolState;
	dtCrowdAgentDebugInfo m_agentDebug;

private:
	sf::Vector2f getMouseWorldPosition() const;

	void changeZone(const std::string& newZC);
	void bindContentToClock();
	void unbindContentToClock();
	void loadEntities(bool already_created);
	void handleStateChange();
	void drawNavMesh();
};


#pragma once

#include <Thor/Input/ActionMap.hpp>

enum class OVERWORLD_COMMANDS {
	EXIT,
	PAUSE,
	DEBUG,
	DEBUG_DISPLAY_MESH,
	DEBUG_NO_COLLIDE,
	MOVE_UP,
	MOVE_DOWN,
	MOVE_LEFT,
	MOVE_RIGHT,
	ZOOM_IN,
	ZOOM_OUT,
	ZOOM_RESET,
	FLASHLIGHT,
	DISPLAY_MAP,
	MOVE_UP_FAST,
	MOVE_DOWN_FAST,
	MOVE_LEFT_FAST,
	MOVE_RIGHT_FAST,
	ZOOM_IN_FAST,
	ZOOM_OUT_FAST,
	ACTIVATE
};

class OverworldCommands
{
	thor::ActionMap<OVERWORLD_COMMANDS> map;

public:
	OverworldCommands();

	bool isActive(OVERWORLD_COMMANDS cmd) const;

	void pollComands(sf::Window& window);
};


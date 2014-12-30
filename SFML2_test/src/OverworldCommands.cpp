#include "OverworldCommands.h"


OverworldCommands::OverworldCommands()
{
	map[OVERWORLD_COMMANDS::EXIT] = 
		thor::Action(sf::Keyboard::Escape, thor::Action::PressOnce) || thor::Action(sf::Event::Closed); 

	map[OVERWORLD_COMMANDS::DEBUG] = thor::Action(sf::Keyboard::LShift, thor::Action::Hold); 

	map[OVERWORLD_COMMANDS::MOVE_UP] = thor::Action(sf::Keyboard::Up, thor::Action::Hold);
	map[OVERWORLD_COMMANDS::MOVE_DOWN] = thor::Action(sf::Keyboard::Down, thor::Action::Hold);
	map[OVERWORLD_COMMANDS::MOVE_LEFT] = thor::Action(sf::Keyboard::Left, thor::Action::Hold);
	map[OVERWORLD_COMMANDS::MOVE_RIGHT] = thor::Action(sf::Keyboard::Right, thor::Action::Hold);

	map[OVERWORLD_COMMANDS::ZOOM_IN] = thor::Action(sf::Keyboard::LControl, thor::Action::Hold);
	map[OVERWORLD_COMMANDS::ZOOM_OUT] = thor::Action(sf::Keyboard::Space, thor::Action::Hold);
	map[OVERWORLD_COMMANDS::ZOOM_RESET] = thor::Action(sf::Keyboard::V, thor::Action::PressOnce);
	map[OVERWORLD_COMMANDS::ZOOM_IN_FAST] = thor::Action(sf::Keyboard::C, thor::Action::Hold);
	map[OVERWORLD_COMMANDS::ZOOM_OUT_FAST] = thor::Action(sf::Keyboard::X, thor::Action::Hold);

	map[OVERWORLD_COMMANDS::FLASHLIGHT] = thor::Action(sf::Keyboard::F, thor::Action::PressOnce);
	
	map[OVERWORLD_COMMANDS::DISPLAY_MAP] = thor::Action(sf::Keyboard::Tab, thor::Action::Hold);

	map[OVERWORLD_COMMANDS::MOVE_UP_FAST] = thor::Action(sf::Keyboard::Z, thor::Action::Hold);
	map[OVERWORLD_COMMANDS::MOVE_DOWN_FAST] = thor::Action(sf::Keyboard::S, thor::Action::Hold);
	map[OVERWORLD_COMMANDS::MOVE_LEFT_FAST] = thor::Action(sf::Keyboard::Q, thor::Action::Hold);
	map[OVERWORLD_COMMANDS::MOVE_RIGHT_FAST] = thor::Action(sf::Keyboard::D, thor::Action::Hold);

	map[OVERWORLD_COMMANDS::ACTIVATE] = thor::Action(sf::Keyboard::E, thor::Action::PressOnce);
}


bool OverworldCommands::isActive(OVERWORLD_COMMANDS cmd) const {
	return map.isActive(cmd);
}

void OverworldCommands::update(sf::Window& window) {
	map.update(window);
}
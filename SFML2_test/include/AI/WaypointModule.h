#pragma once

#include <vector>
#include <SFML/System/Vector2.hpp>

struct WaypointModule {


	std::vector<sf::Vector2f> waypoints;
	bool loop;
	int current_wp_goal;
	bool active;

	WaypointModule() :
		loop(false),
	current_wp_goal(0),
	active(false)
	{

	}




};
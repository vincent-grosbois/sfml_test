#pragma once

#include <SFML\Graphics.hpp>

class OverlayScreen
{
public:
	OverlayScreen(sf::RenderWindow& App) : App(App) {};

	virtual ~OverlayScreen() {};

	virtual bool transferAllControls() const { return false; };

	virtual void Update() { };

	virtual void draw() { };

protected:
	sf::RenderWindow& App;
	
};


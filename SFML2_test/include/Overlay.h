#pragma once

#include "BoxedTextScrollable.h"
#include "NPC.h"
#include "OverlayScreen.h"

class Overlay
{
public:
	Overlay(sf::RenderWindow& App);
	~Overlay();

	sf::Text FPStext;
	sf::Text MapInfosText;

	void draw(bool DrawMap);
	OverlayScreen* overlayScene;

protected :
	sf::RenderWindow& App;

	sf::Texture map;
	sf::Sprite mapSprite;

	sf::Texture cursor;
	sf::Sprite cursorSprite;

	sf::Font myFont;
};


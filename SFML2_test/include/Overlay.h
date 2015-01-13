#pragma once

#include "BoxedTextScrollable.h"
#include "entities/NPC.h"

class Overlay
{
public:
	Overlay(sf::RenderWindow& App);
	~Overlay();

	sf::Text FPStext;
	sf::Text MapInfosText;

	void draw(bool DrawMap);

protected :
	sf::RenderWindow& App;

	sf::Texture map;
	sf::Sprite mapSprite;

	sf::Texture cursor;
	sf::Sprite cursorSprite;

	sf::Font myFont;
};


#pragma once

#include <SFML/Graphics.hpp>

#include "Tileset.h"

class MapTile {


public:
	MapTile(Tileset& tileset, int tileId, tile_units posX, tile_units posY);
	sf::Vector2f getPosition() const { return baseSprite.getPosition(); };
	void draw() const;
	bool isPassable() const { return baseTile.tilePassability == TILE_PASSABILITY::PASSABLE; };

//protected:
	sf::Sprite baseSprite;
	Tile& baseTile;


};


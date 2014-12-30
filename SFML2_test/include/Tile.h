#pragma once

#include <SFML/Graphics.hpp>

#include "Constants.h"

namespace EDGES_PASSABILITY { enum e { PASSABLE, NOTPASSABLE, OUTPASSABLE, INPASSABLE } ;};
namespace TILE_PASSABILITY { enum e { PASSABLE, NOTPASSABLE } ;};


class TileAnimator;

struct Tile {
	int id;
	TILE_PASSABILITY::e tilePassability; 
	EDGES_PASSABILITY::e edgesPassability[4]; //4 directions
	const sf::Texture& myAtlas;
	sf::IntRect myRect;

	Tile(const sf::Texture& atlas, const sf::Vector2i& offset, int id, bool collide = false) :
		id(id),
		myAtlas(atlas),
		myRect(offset.x, offset.y, TILE_SIZE_X, TILE_SIZE_Y),
		myAnimator(NULL)
	{
		tilePassability = collide ? TILE_PASSABILITY::NOTPASSABLE : TILE_PASSABILITY::PASSABLE;

		edgesPassability[DIRECTION::UP] = EDGES_PASSABILITY::PASSABLE;
		edgesPassability[DIRECTION::DOWN] = EDGES_PASSABILITY::PASSABLE;
		edgesPassability[DIRECTION::LEFT] = EDGES_PASSABILITY::PASSABLE;
		edgesPassability[DIRECTION::RIGHT] = EDGES_PASSABILITY::PASSABLE;
	}

	TileAnimator* myAnimator;

	bool isAnimated() const {
		return myAnimator != NULL;
	}

	~Tile();
};
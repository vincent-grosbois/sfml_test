#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

#include "Tile.hpp"
#include "utils/Array2D.h"

class MoveAnimation
{
public:
	MoveAnimation(const std::string& filename);
	
	const sf::IntRect& getFrame(DIRECTION::e dir, int i) const { return animations_rect(dir, i); }
	const sf::Texture& getTexture() const { return atlas; }

	MoveAnimation(MoveAnimation&& other) :
	animations_rect(std::move(other.animations_rect))
	{
		atlas = other.atlas;
	}

protected:
	
	Array2D<sf::IntRect> animations_rect;

	sf::Texture atlas;
	
private:
	MoveAnimation(const MoveAnimation&);

};


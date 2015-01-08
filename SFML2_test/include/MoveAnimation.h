#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

#include "Tile.h"
#include "utils/Array2D.h"


enum class AnimationType {

	MOVING,
	ATTACKING,
	SPELLCASTING,
	DYING,

};


class Animation {
public:
	std::vector<sf::IntRect> frames[4]; // < one for each direction
	int frame_duration_ms;
	AnimationType type;
	bool looping;
};

struct AnimationProgress {
	Animation* animation;
	int current_frame;
	int current_frame_time_elapsed_ms;

	AnimationProgress(Animation* anim) :
	animation(anim), current_frame(0), current_frame_time_elapsed_ms(0)
	{ }
};

class AnimationHolder {
	std::map<AnimationType, Animation> resources;
};


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

private:

	Array2D<sf::IntRect> animations_rect;

	sf::Texture atlas;

	std::string fileName;


	MoveAnimation(const MoveAnimation&);

};


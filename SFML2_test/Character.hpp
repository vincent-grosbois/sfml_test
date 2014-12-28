#pragma once
#include "EntityPhysical.hpp"
#include "MoveAnimation.h"


class Collectible;


class Character : public EntityPhysical
{
public:
	Character(sf::Vector2f const& position,  ZoneContainer& ZC, MoveAnimation& move_anim);
	bool draw(int value, DIRECTION::e, int ticks);
	virtual ~Character() override;
	virtual void draw(int ticks, OverWorldDisplay& owDisplay) override;
	const sf::FloatRect getActivableZone() const;
	void activateThings();
	void receiveItem(Collectible* collectible);
	DIRECTION::e getFacingDir() const { return facingDir; };

protected:
	virtual void onNewTileWalked() { };

public:
	bool isMoving;

protected:
	DIRECTION::e facingDir;
	float speed;
	MoveAnimation* move_anim;
	int current_frame;
	float moveCount;
};


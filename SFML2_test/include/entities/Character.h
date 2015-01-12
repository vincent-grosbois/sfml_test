#pragma once
#include "EntityPhysical.h"
#include "MoveAnimation.h"
#include "Projectile.h"


class Collectible;
class GameTicks;
struct WaypointModule;

class Character : public EntityPhysical
{
public:
	Character(const sf::Vector2f& position, ZoneContainer& ZC, GameTicks& ticks, MoveAnimation& move_anim);
	bool tryMoving(int value, DIRECTION::e);
	virtual ~Character() override;
	virtual void draw(OverworldDisplay& owDisplay) override;
	const sf::FloatRect getActivableZone() const;
	void activateThings();
	void receiveItem(Collectible* collectible);
	DIRECTION::e getFacingDir() const { return facingDir; };

public:
	bool isMoving;

protected:
	DIRECTION::e facingDir;
	float speed;
	MoveAnimation* move_anim;
	int current_frame;
	GameTicks& ticks;
	WaypointModule* waypointModule;
};


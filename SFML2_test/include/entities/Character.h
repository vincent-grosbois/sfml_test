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
	virtual ~Character() override;

	virtual void draw(OverworldDisplay& owDisplay) override;

	const sf::FloatRect getActivableZone() const;
	DIRECTION::e getFacingDir() const { return facingDir; };

protected:
	bool tryMoving(int value, DIRECTION::e);

	DIRECTION::e facingDir;
	float speed;
	MoveAnimation* move_anim;
	int current_frame;
	GameTicks& ticks;
	WaypointModule* waypointModule;
};


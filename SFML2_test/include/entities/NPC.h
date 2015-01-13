#pragma once
#include "Character.h"
#include "ZoneContainer.h"
#include "overworld/OverworldScene.h"

enum class NpcBehavior  { 
	STILL, 
	WANDER, 
	WANDER_NOSTOP,
	USE_WAYPOINT 
} ;

enum class NpcState {
	STANDING,
	WALKING,
	ATTACKING
};


class NPC : public Character
{
public:
	NPC(const sf::Vector2f& position, ZoneContainer& ZC, GameTicks& ticks, MoveAnimation& move_anim);
	virtual ~NPC() override;

	virtual void update(int delta_ms, bool will_be_drawn) override;
	virtual bool onActivated(Entity& activator) override;
	virtual void drawDebugInfo(OverworldDisplay& owDisplay) override;

protected:
	NpcBehavior behavior;
	NpcState myState;

	bool moveAtRandom(float value);
	NpcBehavior getBehavior() const { return behavior; };
	sf::FloatRect getAwarenessZone() const;
	void moveToWaypoint(int delta_ms);
};


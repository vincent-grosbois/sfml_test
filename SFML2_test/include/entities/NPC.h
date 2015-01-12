#pragma once
#include "Character.h"
#include "ZoneContainer.h"
#include "overworld/OverworldScene.h"

enum class NPC_BEHAVIOR  { STILL, WANDER, WANDER_NOSTOP, USE_WAYPOINT } ;

DIRECTION::e getOppositeDir(DIRECTION::e dir);

class NPC :
	public Character
{
public:
	NPC(sf::Vector2f const& position,  ZoneContainer& ZC, GameTicks& ticks, MoveAnimation& move_anim);
	~NPC();
	virtual void update(int delta_ms, bool will_be_drawn) override;

	bool moveAtRandom(float value);

	virtual bool onActivated(Entity& activator) override;

	NPC_BEHAVIOR getBehavior() const { return behavior; };

	virtual void drawDebugInfo(OverworldDisplay& owDisplay) override ;

	

protected:
	NPC_BEHAVIOR behavior;

	sf::FloatRect getAwarenessZone() const;
	void moveToWaypoint(int delta_ms);
};


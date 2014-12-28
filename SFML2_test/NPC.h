#pragma once
#include "Character.hpp"
#include "ZoneContainer.hpp"
#include "OverWorldScene.hpp"

namespace NPC_BEHAVIOR { enum e  { STILL, WANDER, WANDER_NOSTOP } ; };

DIRECTION::e getOppositeDir(DIRECTION::e dir);

class NPC :
	public Character
{
public:
	NPC(sf::Vector2f const& position,  ZoneContainer& ZC, MoveAnimation& move_anim);
	~NPC();
	virtual void update(int delta_ms) override;

	bool moveAtRandom(float value, int ticks);

	virtual bool onActivated(Entity& activator) override;

	NPC_BEHAVIOR::e getBehavior() const { return _behavior; };
	void setBehavior(NPC_BEHAVIOR::e s) {  _behavior = s; };

	virtual void drawCollisionBox(OverWorldDisplay& owDisplay) override ;

protected:
	NPC_BEHAVIOR::e _behavior;

	sf::FloatRect getAwarenessZone() const;
};


#pragma once
#include "Character.h"
#include "NPC.h"

class Collectible;
class Overlay;

enum class PlayerCharacterState {
	STANDING,
	WALKING,
	ATTACKING
};

struct DirectionalMoveInputRequest {
	DIRECTION::e direction;
	float speed;
	bool move_through;
};

struct InputRequestCpt {
	bool left_right_request;
	DirectionalMoveInputRequest left_right_request_details;

	bool up_down_request;
	DirectionalMoveInputRequest up_down_request_details;

	InputRequestCpt()  {
		reset();
	}

	void reset() {
		left_right_request = false;
		up_down_request = false;
	}

};

class PlayerCharacter : public Character
{
public:
	PlayerCharacter(const sf::Vector2f& position, ZoneContainer& ZC, GameTicks& ticks, MoveAnimation& move_anim, Overlay& overlay);

	virtual void drawDebugInfo(OverworldDisplay& owDisplay) override;
	virtual void draw(OverworldDisplay& owDisplay) override;

	virtual void update(int delta_ms, bool will_be_drawn) override;

	void moveRequest(DIRECTION::e dir, float speed = 1.f, bool debug  = false);
	void teleportTo(const sf::Vector2f& pos, ZoneContainer* ZC = NULL);


private:
	bool tryMoving(int delta_time_ms, DIRECTION::e dir, bool debug);

	PlayerCharacterState myState;
	InputRequestCpt myInputRequest;
};


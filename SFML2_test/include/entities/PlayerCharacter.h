#pragma once
#include "Character.h"
#include "NPC.h"

class Collectible;
class Overlay;

class PlayerCharacter : public Character
{
public:
	PlayerCharacter(const sf::Vector2f& position, ZoneContainer& ZC, GameTicks& ticks, MoveAnimation& move_anim, Overlay& overlay);

	virtual void drawDebugInfo(OverWorldDisplay& owDisplay) override;

	void DialogWindow(const std::string& str, bool forceFocus = false, NPC* originator = NULL);
	void receiveItem(Collectible* collectible);
	virtual void draw(OverWorldDisplay& owDisplay) override;

	bool tryMoving(int value, DIRECTION::e d, int ticks, bool first_press, bool debug ) { 

		if( tryMoving(value, d, ticks, debug) ) {

			_first_moving = first_press;

			return true;
		}
		else {
			_first_moving = false;
			return false;
		}

	}

	bool tryMoving(int value, DIRECTION::e dir, int ticks, bool debug) {
		if(!debug)
			return Character::tryMoving(value, dir);
		else {

			facingDir = dir;

			if(locationList.empty()) 
				return false;

			float * coord_to_change;
			int sign;

			switch(dir) {
			case DIRECTION::RIGHT:
				coord_to_change = &position.x;
				sign = 1;
				break;
			case DIRECTION::LEFT:
				coord_to_change = &position.x;
				sign = -1;
				break;
			case DIRECTION::DOWN:
				coord_to_change = &position.y;
				sign = 1;
				break;
			case DIRECTION::UP:
				coord_to_change = &position.y;
				sign = -1;
				break;
			}

			float increment  = sign*speed*value;

			if (increment > 32) 
				increment = 32;
			else if(increment < -32) 
				increment = -32;


			*coord_to_change +=  increment;

			boundingBox.positionBoundingBox(position);
			spriteCpt.positionSprite(position);

			isMoving = true;

			registerInMaps();

			return true;
		}

	}

	void teleportTo(sf::Vector2f pos, ZoneContainer* ZC = NULL);



private:
	Overlay& overlay;
	bool _first_moving;
};


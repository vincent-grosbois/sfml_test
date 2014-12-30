#pragma once
#include "Character.h"
#include "NPC.h"
#include "Overlay.h"


class Collectible;

class PlayerCharacter : public Character
{
public:
	PlayerCharacter(sf::Vector2f const& position,  ZoneContainer& ZC, MoveAnimation& move_anim,  Overlay& overlay);

	virtual void drawCollisionBox(OverWorldDisplay& owDisplay) override;

	void DialogWindow(const std::string& str, bool forceFocus = false, NPC* originator = NULL);
	void receiveItem(Collectible* collectible);
	virtual void draw(int ticks, OverWorldDisplay& owDisplay) override;

	bool draw(int value, DIRECTION::e d, int ticks, bool first_press, bool debug ) { 

		if( draw(value, d, ticks, debug) ) {

			_first_moving = first_press;

			return true;
		}
		else {
			_first_moving = false;
			return false;
		}

	}

	bool draw(int value, DIRECTION::e dir, int ticks, bool debug) {
		if(!debug)
			return Character::draw(value, dir, ticks);
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


			float boundingBoxWidth = boundingBoxSize.x;
			float boundingBoxHeight = boundingBoxSize.y;

			sf::FloatRect BoundingBoxRect(position.x, position.y, boundingBoxWidth, boundingBoxHeight);


			sf::Vector2f coll_coords;

			BoundingBoxRectReal = BoundingBoxRect;

			moveCount += abs(increment);

			if( moveCount > 32) {
				moveCount -= 32;
				//std::cout << "tile walked\n";
				onNewTileWalked();
			}

			positionSprite();
			isMoving = true;

			registerInMaps();

			return true;
		}

	}

	void teleportTo(sf::Vector2f pos, ZoneContainer* ZC = NULL);



private:
	Overlay& overlay;
	bool _first_moving;
	virtual void onNewTileWalked() override;
};

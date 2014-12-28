#include "NPC.h"
#include "PlayerCharacter.hpp"
#include "GameTicks.hpp"

DIRECTION::e getOppositeDir(DIRECTION::e dir) {
	if(dir == DIRECTION::UP) return DIRECTION::DOWN;
	else if(dir == DIRECTION::DOWN) return DIRECTION::UP;
	else if(dir == DIRECTION::LEFT) return DIRECTION::RIGHT;
	else if(dir == DIRECTION::RIGHT) return DIRECTION::LEFT;

	return DIRECTION::UP;
}

NPC::NPC(sf::Vector2f const& position,  ZoneContainer& ZC, MoveAnimation& move_anim):
	Character(position,  ZC, move_anim),
	_behavior(NPC_BEHAVIOR::WANDER)
{
}


NPC::~NPC()
{
}

bool NPC::moveAtRandom(float value, int ticks) {
	
	if(isAsleep)
		return isMoving;

	DIRECTION::e dir;

	if(!isMoving || rand()%100 == 0) 
	 dir = (DIRECTION::e)(rand()%4);
	else
	 dir = facingDir;
 
	bool ret = Character::draw(value, dir, ticks);

	if(!ret) { isMoving = false ; }

	if(isMoving) {
		facingDir = dir;
	}
	
	return ret;

}

void NPC::update(int value)  { 
	
	int ticks = 1;

	if(_behavior == NPC_BEHAVIOR::WANDER ){

		if(isMoving) {

			if( rand()%500 != 0 )
				moveAtRandom(value, ticks);
			else
				isMoving = false;

		}
		else if ( rand()%100 == 0 ) {
			moveAtRandom(value, ticks);
		}

	}
	else if(_behavior == NPC_BEHAVIOR::WANDER_NOSTOP ) {
		moveAtRandom(value, ticks);
	}
	else if(_behavior == NPC_BEHAVIOR::STILL ) {
		//do nothing
	}


}

bool NPC::onActivated(Entity& activator) { 

		Character* activator_character = dynamic_cast<Character*>(&activator);
		PlayerCharacter* activator_PC = dynamic_cast<PlayerCharacter*>(&activator);
		
		if(activator_character)
			facingDir = getOppositeDir(activator_character->getFacingDir());

		if(activator_PC) {
			activator_PC->DialogWindow("hurr\ndurr\ndd", true, this);
		}

		return true; 
	}

sf::FloatRect NPC::getAwarenessZone() const {
	float advance = 400;
	float width = 300;

	float side_size_x = width/2  - boundingBoxSize.x/2;
	float side_size_y = width/2  - boundingBoxSize.y/2;

	switch(facingDir) {
	case DIRECTION::RIGHT:
		return sf::FloatRect(BoundingBoxRectReal.left + BoundingBoxRectReal.width, BoundingBoxRectReal.top - side_size_y, advance, width);
		break;
	case DIRECTION::LEFT:
		return sf::FloatRect(BoundingBoxRectReal.left - advance, BoundingBoxRectReal.top -  side_size_y, advance, width);
		break;
	case DIRECTION::DOWN:
		return sf::FloatRect(BoundingBoxRectReal.left - side_size_x, BoundingBoxRectReal.top + BoundingBoxRectReal.height, width, advance);
		break;
	case DIRECTION::UP:
		return sf::FloatRect(BoundingBoxRectReal.left - side_size_x, BoundingBoxRectReal.top - advance, width, advance);
		break;
	}

	return sf::FloatRect();
}

void  NPC::drawCollisionBox(OverWorldDisplay& owDisplay)  { 

	sf::RectangleShape rect(sf::Vector2f(BoundingBoxRectReal.width, BoundingBoxRectReal.height));
	rect.setPosition(BoundingBoxRectReal.left,BoundingBoxRectReal.top);
	rect.setFillColor(sf::Color(0,0,0,0));
	rect.setOutlineColor(sf::Color::Blue);
	rect.setOutlineThickness(1);
	owDisplay.overWorld_texture.draw(rect); 

	sf::RectangleShape rect2(sf::Vector2f(getAwarenessZone().width, getAwarenessZone().height));
	rect2.setPosition(getAwarenessZone().left,getAwarenessZone().top);
	rect2.setFillColor(sf::Color(0,0,0,0));
	rect2.setOutlineColor(sf::Color::Green);
	rect2.setOutlineThickness(1);
	owDisplay.overWorld_texture.draw(rect2); 
};

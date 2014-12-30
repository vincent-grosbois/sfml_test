#include <algorithm>

#include "Character.h"
#include "ZoneContainer.h"
#include "Collectible.h"
#include "TilePlane.h"
#include "Map.h"
#include "OverWorldScene.h"

Character::Character(const sf::Vector2f& position,  ZoneContainer& ZC, MoveAnimation& move_anim):
	EntityPhysical(position, ZC),
	move_anim(&move_anim),
	facingDir(DIRECTION::DOWN),
	current_frame(0),
	isMoving(false),
	speed(200.f/1000),
	moveCount(0)
{
	spriteOffset.x = (boundingBoxSize.x - move_anim.getFrame(DIRECTION::DOWN,0).width )/2;
	spriteOffset.y = (boundingBoxSize.y - move_anim.getFrame(DIRECTION::DOWN,0).height +2);


	sprite.setTexture(move_anim.getTexture());
	sprite.setTextureRect(move_anim.getFrame(facingDir, current_frame ) );

	positionSprite();
}

void Character::draw(int ticks, OverWorldDisplay& owDisplay) {

	current_frame = isMoving ? (current_frame + ticks) %9 : 0;

	sprite.setTexture(move_anim->getTexture() );
	sprite.setTextureRect(move_anim->getFrame(facingDir, current_frame ) );

	owDisplay.overWorld_texture.draw(sprite);
}




bool Character::draw(int value, DIRECTION::e dir, int ticks) {

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

	float oldvalue = *coord_to_change;

	*coord_to_change +=  increment;


	float boundingBoxWidth = boundingBoxSize.x;
	float boundingBoxHeight = boundingBoxSize.y;

	sf::FloatRect BoundingBoxRect(position.x, position.y, boundingBoxWidth, boundingBoxHeight);


	std::set<Map*>::const_iterator it_maps;
	std::set<EntitySet*>::const_iterator it_set;
	std::set<Entity*>::const_iterator it_element;
	sf::Vector2f coll_coords;

	//get the maps we would be in if could move all 
	std::set<Map*> new_maps = ZC->getCollidingMaps(BoundingBoxRect);

	for ( it_maps = new_maps.begin() ; it_maps != new_maps.end(); ++it_maps ) {

		//check collision with the static world:
		if ( (*it_maps)->collideWithLayer(0, BoundingBoxRect, &coll_coords) )  {

			switch(dir) {
			case DIRECTION::RIGHT:
				*coord_to_change = coll_coords.x - boundingBoxWidth;
				break;
			case DIRECTION::LEFT:
				*coord_to_change = coll_coords.x + TILE_SIZE_X;
				break;
			case DIRECTION::DOWN:
				*coord_to_change = coll_coords.y - boundingBoxHeight;
				break;
			case DIRECTION::UP:
				*coord_to_change = coll_coords.y + TILE_SIZE_Y;
				break;
			}


			if(*coord_to_change == oldvalue) 
			{   
				isMoving = false; 
				return false; 
			}

			increment = *coord_to_change - oldvalue;
			facingDir = dir;
			BoundingBoxRect = sf::FloatRect(position.x, position.y, boundingBoxWidth, boundingBoxHeight);	

		}


		std::set<EntitySet*> new_set;

		(*it_maps)->getCollidingTiles(BoundingBoxRect, new_set);

		sf::Rect<float> temp_inter;

		for (it_set = new_set.begin() ; it_set != new_set.end(); ++it_set) {

			if(!(*it_set)->has_entities())
				continue;

			for( it_element = (*it_set)->entities().begin(); it_element != (*it_set)->entities().end(); ++it_element) {

				if(*it_element == this) 
					continue;

				if((*it_element)->intersectsForCollision(BoundingBoxRect, &temp_inter)) { 



					switch(dir) {
					case DIRECTION::RIGHT:
						*coord_to_change = temp_inter.left - boundingBoxWidth;
						break;
					case DIRECTION::LEFT:
						*coord_to_change = temp_inter.left + temp_inter.width;
						break;
					case DIRECTION::DOWN:
						*coord_to_change = temp_inter.top - boundingBoxHeight;
						break;
					case DIRECTION::UP:
						*coord_to_change = temp_inter.top + temp_inter.height;
						break;
					}



					if(*coord_to_change == oldvalue) //if not moved at all
					{   
						isMoving = false; 
						return false; 
					}




					(*it_element)->onCollision(*this);

					BoundingBoxRect = sf::FloatRect(position.x, position.y, boundingBoxWidth, boundingBoxHeight);	
					break;
				}



			}

		}
	}


	facingDir = dir;
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

const sf::FloatRect Character::getActivableZone() const {
	float advance = 22;
	switch(facingDir) {
	case DIRECTION::RIGHT:
		return sf::FloatRect(BoundingBoxRectReal.left + BoundingBoxRectReal.width, BoundingBoxRectReal.top - boundingBoxSize.y/2, advance, 2*BoundingBoxRectReal.height);
		break;
	case DIRECTION::LEFT:
		return sf::FloatRect(BoundingBoxRectReal.left - advance, BoundingBoxRectReal.top - boundingBoxSize.y/2, advance, 2*BoundingBoxRectReal.height);
		break;
	case DIRECTION::DOWN:
		return sf::FloatRect(BoundingBoxRectReal.left - boundingBoxSize.x/2, BoundingBoxRectReal.top + BoundingBoxRectReal.height, 2*BoundingBoxRectReal.width, advance);
		break;
	case DIRECTION::UP:
		return sf::FloatRect(BoundingBoxRectReal.left - boundingBoxSize.x/2, BoundingBoxRectReal.top - advance, 2*BoundingBoxRectReal.width, advance);
		break;
	}

	assert(false);
	return sf::FloatRect();
}

void Character::activateThings() {

	std::set<Map*> set = ZC->getCollidingMaps(getActivableZone());
	std::set<Map*>::iterator iter_maps;
	std::set<EntitySet*>::iterator it_set;
	std::set<EntitySet*> colliding_tiles;
	std::set<Entity*>::iterator it_element;

	for(iter_maps = set.begin(); iter_maps != set.end(); ++iter_maps) {

		(*iter_maps)->getCollidingTiles(getActivableZone(), colliding_tiles);

		for (it_set = colliding_tiles.begin() ; it_set != colliding_tiles.end(); ++it_set) {

			if(!(*it_set)->has_entities()) 
				continue;

			for( it_element = (*it_set)->entities().begin(); it_element != (*it_set)->entities().end(); ++it_element) {


				if(*it_element != this) {

					if( !(*it_element)->isMarkedForDeletion() && (*it_element)->intersectsForCollision(getActivableZone()) && (*it_element)->onActivated(*this) ) {
						return;
					}

				}
			}

		}

	}

}

void Character::receiveItem(Collectible* collectible) {

	if( false ) {
		collectible->markForDeletion();
	}
	else {
		//couldn't add item...
	}

}

Character::~Character()
{
}

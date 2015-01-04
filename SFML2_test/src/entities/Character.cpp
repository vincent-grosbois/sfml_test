#include <algorithm>

#include "entities/Character.h"
#include "entities/Collectible.h"
#include "ZoneContainer.h"
#include "TilePlane.h"
#include "Map.h"
#include "OverWorldScene.h"
#include "GameTicks.h"

Character::Character(const sf::Vector2f& position, ZoneContainer& ZC, GameTicks& ticks, MoveAnimation& move_anim):
	EntityPhysical(position, ZC),
	move_anim(&move_anim),
	facingDir(DIRECTION::DOWN),
	current_frame(0),
	isMoving(false),
	speed(185.f/1000),
	ticks(ticks),
	waypointModule(NULL)
{
	spriteOffset.x = - move_anim.getFrame(DIRECTION::DOWN,0).width/2;
	spriteOffset.y = - move_anim.getFrame(DIRECTION::DOWN,0).height + 10;

	sprite.setTexture(move_anim.getTexture());
	sprite.setTextureRect(move_anim.getFrame(facingDir, current_frame ) );

	positionSprite();
}

void Character::draw(OverWorldDisplay& owDisplay) {

	int ticks = this->ticks.getTicks(TICKS::e::_250MS);
	int old_frame = current_frame;
	current_frame = isMoving ? (current_frame + ticks) %9 : 0;

	if(old_frame != current_frame) {
		sprite.setTextureRect(move_anim->getFrame(facingDir, current_frame ) );
	}
	owDisplay.overWorld_texture.draw(sprite);
}


bool Character::tryMoving(int value, DIRECTION::e dir) {

	facingDir = dir;

	if(locationList.empty())
		return false;

	float* coord_to_change;
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

	float oldvalue = *coord_to_change;

	*coord_to_change +=  increment;


	sf::FloatRect BoundingBoxRect(position.x + boundingBoxOffset.x, position.y + boundingBoxOffset.y, boundingBoxSize.x, boundingBoxSize.y);

	std::set<Map*>::const_iterator it_maps;
	std::set<EntitySet*>::const_iterator it_set;
	std::set<Entity*>::const_iterator it_element;
	sf::Vector2f collision_coords;

	//get the maps we would be in if we could move all the way
	std::set<Map*> new_maps = ZC->getCollidingMaps(BoundingBoxRect);

	for ( it_maps = new_maps.begin() ; it_maps != new_maps.end(); ++it_maps ) {

		//check collision with the static world:
		if ( (*it_maps)->collideWithLayer(0, BoundingBoxRect, &collision_coords) )  {

			switch(dir) {
			case DIRECTION::RIGHT:
				*coord_to_change = collision_coords.x + boundingBoxOffset.x;
				break;
			case DIRECTION::LEFT:
				*coord_to_change = collision_coords.x + TILE_SIZE_X - boundingBoxOffset.x;
				break;
			case DIRECTION::DOWN:
				*coord_to_change = collision_coords.y + boundingBoxOffset.y;
				break;
			case DIRECTION::UP:
				*coord_to_change = collision_coords.y + TILE_SIZE_Y - boundingBoxOffset.y;
				break;
			}


			if(*coord_to_change == oldvalue) 
			{   
				isMoving = false; 
				return false; 
			}

			increment = *coord_to_change - oldvalue;
			facingDir = dir;
			BoundingBoxRect = sf::FloatRect(position.x + boundingBoxOffset.x, position.y + boundingBoxOffset.y, boundingBoxSize.x, boundingBoxSize.y);	
		}


		std::set<EntitySet*> new_set;

		(*it_maps)->getCollidingEntitySets(BoundingBoxRect, new_set);

		sf::Rect<float> temp_inter;

		for (it_set = new_set.begin() ; it_set != new_set.end(); ++it_set) {

			if(!(*it_set)->has_entities())
				continue;

			for( it_element = (*it_set)->entities().begin(); it_element != (*it_set)->entities().end(); ++it_element) {

				if(*it_element == this) 
					continue;

				if(getType() == EntityType::PLAYER_CHARACTER && (*it_element)->getType() == EntityType::PROJECTILE)
					continue;

				if((*it_element)->intersectsForCollision(BoundingBoxRect, &temp_inter))
				{ 
					switch(dir) {
					case DIRECTION::RIGHT:
						*coord_to_change = temp_inter.left + boundingBoxOffset.x;
						break;
					case DIRECTION::LEFT:
						*coord_to_change = temp_inter.left + temp_inter.width - boundingBoxOffset.x;
						break;
					case DIRECTION::DOWN:
						*coord_to_change = temp_inter.top + boundingBoxOffset.y;
						break;
					case DIRECTION::UP:
						*coord_to_change = temp_inter.top + temp_inter.height - boundingBoxOffset.y;
						break;
					}

					if(*coord_to_change == oldvalue) //if not moved at all
					{   
						isMoving = false; 
						return false; 
					}

					(*it_element)->onCollision(*this);

					BoundingBoxRect = sf::FloatRect(position.x+ boundingBoxOffset.x, position.y+ boundingBoxOffset.y, boundingBoxSize.x, boundingBoxSize.y);	
					break;
				}


			}

		}
	}


	facingDir = dir;
	boundingBoxRectReal = BoundingBoxRect;

	positionSprite();
	isMoving = true;

	registerInMaps();

	return true;
}

const sf::FloatRect Character::getActivableZone() const {
	const float advance = 22;
	switch(facingDir) {
	case DIRECTION::RIGHT:
		return sf::FloatRect(boundingBoxRectReal.left + boundingBoxRectReal.width, boundingBoxRectReal.top - boundingBoxSize.y/2, advance, 2*boundingBoxRectReal.height);
		break;
	case DIRECTION::LEFT:
		return sf::FloatRect(boundingBoxRectReal.left - advance, boundingBoxRectReal.top - boundingBoxSize.y/2, advance, 2*boundingBoxRectReal.height);
		break;
	case DIRECTION::DOWN:
		return sf::FloatRect(boundingBoxRectReal.left - boundingBoxSize.x/2, boundingBoxRectReal.top + boundingBoxRectReal.height, 2*boundingBoxRectReal.width, advance);
		break;
	case DIRECTION::UP:
		return sf::FloatRect(boundingBoxRectReal.left - boundingBoxSize.x/2, boundingBoxRectReal.top - advance, 2*boundingBoxRectReal.width, advance);
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

		(*iter_maps)->getCollidingEntitySets(getActivableZone(), colliding_tiles);

		for (it_set = colliding_tiles.begin() ; it_set != colliding_tiles.end(); ++it_set) {

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

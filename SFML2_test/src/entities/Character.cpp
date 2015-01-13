#include <algorithm>

#include "entities/Character.h"
#include "ZoneContainer.h"
#include "TilePlane.h"
#include "Map.h"
#include "overworld/OverworldScene.h"
#include "GameTicks.h"

using namespace constants;

Character::Character(const sf::Vector2f& position, ZoneContainer& ZC, GameTicks& ticks, MoveAnimation& move_anim):
	EntityPhysical(position, ZC),
	move_anim(&move_anim),
	facingDir(DIRECTION::DOWN),
	current_frame(0),
	speed(185.f/1000),
	ticks(ticks),
	waypointModule(NULL)
{
	spriteCpt.spriteOffset.x = - move_anim.getFrame(DIRECTION::DOWN,0).width/2.f;
	spriteCpt.spriteOffset.y = - move_anim.getFrame(DIRECTION::DOWN,0).height + 10.f;

	spriteCpt.sprite.setTexture(move_anim.getTexture());
	spriteCpt.sprite.setTextureRect(move_anim.getFrame(facingDir, current_frame ) );

	spriteCpt.positionSprite(position);
}

void Character::draw(OverworldDisplay& owDisplay) {
	owDisplay.overWorld_texture.draw(spriteCpt.sprite);
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


	sf::FloatRect BoundingBoxRect(
		position.x + boundingBox.boundingBoxOffset.x, 
		position.y + boundingBox.boundingBoxOffset.y, 
		boundingBox.boundingBoxRectReal.width, boundingBox.boundingBoxRectReal.height);

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
				*coord_to_change = collision_coords.x + boundingBox.boundingBoxOffset.x;
				break;
			case DIRECTION::LEFT:
				*coord_to_change = collision_coords.x + TILE_SIZE_X - boundingBox.boundingBoxOffset.x;
				break;
			case DIRECTION::DOWN:
				*coord_to_change = collision_coords.y + boundingBox.boundingBoxOffset.y;
				break;
			case DIRECTION::UP:
				*coord_to_change = collision_coords.y + TILE_SIZE_Y - boundingBox.boundingBoxOffset.y;
				break;
			}


			if(*coord_to_change == oldvalue) 
			{   
				return false; 
			}

			increment = *coord_to_change - oldvalue;
			facingDir = dir;
			boundingBox.positionBoundingBox(position);
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
						*coord_to_change = temp_inter.left + boundingBox.boundingBoxOffset.x;
						break;
					case DIRECTION::LEFT:
						*coord_to_change = temp_inter.left + temp_inter.width - boundingBox.boundingBoxOffset.x;
						break;
					case DIRECTION::DOWN:
						*coord_to_change = temp_inter.top + boundingBox.boundingBoxOffset.y;
						break;
					case DIRECTION::UP:
						*coord_to_change = temp_inter.top + temp_inter.height - boundingBox.boundingBoxOffset.y;
						break;
					}

					if(*coord_to_change == oldvalue) //if not moved at all
					{   
						return false; 
					}

					(*it_element)->onCollision(*this);
					boundingBox.positionBoundingBox(position);
					break;
				}


			}

		}
	}


	facingDir = dir;
	boundingBox.boundingBoxRectReal = BoundingBoxRect;

	spriteCpt.positionSprite(position);

	registerInMaps();

	return true;
}

const sf::FloatRect Character::getActivableZone() const {
	const float advance = 22;
	switch(facingDir) {
	case DIRECTION::RIGHT:
		return sf::FloatRect(boundingBox.boundingBoxRectReal.left + boundingBox.boundingBoxRectReal.width, boundingBox.boundingBoxRectReal.top -boundingBox.boundingBoxRectReal.height/2, advance, 2*boundingBox.boundingBoxRectReal.height);
		break;
	case DIRECTION::LEFT:
		return sf::FloatRect(boundingBox.boundingBoxRectReal.left - advance, boundingBox.boundingBoxRectReal.top - boundingBox.boundingBoxRectReal.height/2, advance, 2*boundingBox.boundingBoxRectReal.height);
		break;
	case DIRECTION::DOWN:
		return sf::FloatRect(boundingBox.boundingBoxRectReal.left - boundingBox.boundingBoxRectReal.width/2, boundingBox.boundingBoxRectReal.top + boundingBox.boundingBoxRectReal.height, 2*boundingBox.boundingBoxRectReal.width, advance);
		break;
	case DIRECTION::UP:
		return sf::FloatRect(boundingBox.boundingBoxRectReal.left - boundingBox.boundingBoxRectReal.width/2, boundingBox.boundingBoxRectReal.top - advance, 2*boundingBox.boundingBoxRectReal.width, advance);
		break;
	}

	assert(false);
	return sf::FloatRect();
}

Character::~Character()
{ }

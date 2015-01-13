#include "entities/PlayerCharacter.h"
#include "utils/DrawingUtils.h"

using namespace utils;

PlayerCharacter::PlayerCharacter(sf::Vector2f const& position,  ZoneContainer& ZC, GameTicks& ticks, MoveAnimation& move_anim, Overlay& overlay):
	Character(position, ZC, ticks, move_anim), myState(PlayerCharacterState::STANDING)
{
	type = EntityType::PLAYER_CHARACTER;
}


void PlayerCharacter::draw(OverworldDisplay& owDisplay) {
	owDisplay.overWorld_texture.draw(spriteCpt.sprite);
}


void PlayerCharacter::teleportTo(const sf::Vector2f& pos, ZoneContainer* destinationZC) {
	position = pos;

	boundingBox.positionBoundingBox(pos);
	spriteCpt.positionSprite(pos);

	if (destinationZC) {
		ZC = destinationZC;
	}

	registerInMaps();
}

void PlayerCharacter::drawDebugInfo(OverworldDisplay& owDisplay) { 
	drawRectangle(owDisplay.debug_texture, boundingBox.boundingBoxRectReal, sf::Color::Blue);
	drawRectangle(owDisplay.debug_texture, getActivableZone(), sf::Color::Red);
	drawRectangle(owDisplay.debug_texture, getVisibilityRectangle(), sf::Color::Yellow);

	drawPoint(owDisplay.debug_texture, getPosition(), sf::Color::Green);
	drawPoint(owDisplay.debug_texture, getSpriteCenter(), sf::Color::Cyan);
}

void PlayerCharacter::update(int delta_ms, bool will_be_drawn) {

	bool moved1 = myInputRequest.up_down_request ? tryMoving(delta_ms*myInputRequest.up_down_request_details.speed, myInputRequest.up_down_request_details.direction, myInputRequest.up_down_request_details.move_through) : false;
	bool moved2 = myInputRequest.left_right_request ? tryMoving(delta_ms*myInputRequest.left_right_request_details.speed, myInputRequest.left_right_request_details.direction, myInputRequest.left_right_request_details.move_through) : false;
	myInputRequest.reset();

	if(myState == PlayerCharacterState::STANDING) {

		if(moved1 || moved2) {
			myState = PlayerCharacterState::WALKING;
			current_frame = 1 ;
			spriteCpt.sprite.setTextureRect(move_anim->getFrame(facingDir, current_frame ) );
		} 
	} 
	else if (myState == PlayerCharacterState::WALKING) {

		if(moved1 || moved2) {
			int ticks = this->ticks.getTicks(TICKS::e::_250MS);
			current_frame = (current_frame + ticks) %4 ;
			spriteCpt.sprite.setTextureRect(move_anim->getFrame(facingDir, current_frame ) );
		} else {
			myState = PlayerCharacterState::STANDING;
			current_frame = 0 ;
			spriteCpt.sprite.setTextureRect(move_anim->getFrame(facingDir, current_frame ) );
		}
	}
}

void PlayerCharacter::moveRequest(DIRECTION::e dir, float speed, bool debug) {

	if(dir == DIRECTION::LEFT || dir == DIRECTION::RIGHT) {
		myInputRequest.left_right_request = true;
		myInputRequest.left_right_request_details.direction = dir;
		myInputRequest.left_right_request_details.speed = speed;
		myInputRequest.left_right_request_details.move_through = debug;
	}
	else if(dir == DIRECTION::UP || dir == DIRECTION::DOWN) {
		myInputRequest.up_down_request = true;
		myInputRequest.up_down_request_details.direction = dir;
		myInputRequest.up_down_request_details.speed = speed;
		myInputRequest.up_down_request_details.move_through = debug;
	}
}

bool PlayerCharacter::tryMoving(int delta_time_ms, DIRECTION::e dir, bool debug) {

	if(!debug) {
		return Character::tryMoving(delta_time_ms, dir);
	}
	else {

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

		float increment  = sign*speed*delta_time_ms;

		if (increment > 32) 
			increment = 32;
		else if(increment < -32) 
			increment = -32;


		*coord_to_change +=  increment;

		boundingBox.positionBoundingBox(position);
		spriteCpt.positionSprite(position);

		registerInMaps();

		return true;
	}

}
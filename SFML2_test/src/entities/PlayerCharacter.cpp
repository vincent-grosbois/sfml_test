#include "entities/PlayerCharacter.h"
#include "utils/DrawingUtils.h"

using namespace utils;

PlayerCharacter::PlayerCharacter(sf::Vector2f const& position,  ZoneContainer& ZC, GameTicks& ticks, MoveAnimation& move_anim, Overlay& overlay):
Character(position, ZC, ticks, move_anim), overlay(overlay), _first_moving(false)
{
	type = EntityType::PLAYER_CHARACTER;
}


void PlayerCharacter::DialogWindow(const std::string& str, bool force_focus, NPC* originator) {

	//overlay.generateTextBox(str, force_focus, originator);

}

void PlayerCharacter::draw(OverWorldDisplay& owDisplay) {

	int ticks = this->ticks.getTicks(TICKS::e::_250MS);

	if(_first_moving)
		current_frame = 1;
	else
		current_frame = isMoving ? (current_frame + ticks) %4 : 0;

	spriteCpt.sprite.setTexture(move_anim->getTexture() );
	spriteCpt.sprite.setTextureRect(move_anim->getFrame(facingDir, current_frame ) );

	owDisplay.overWorld_texture.draw(spriteCpt.sprite);
}

void PlayerCharacter::receiveItem(Collectible* collectible) {

	if( true ) {
		//collectible->markForDeletion();
		DialogWindow("received item!\ntext_still\nmors\nMORE\nTEXT", true);
	}
	else {
		//couldn't add item...
	}

}

void PlayerCharacter::teleportTo(sf::Vector2f pos, ZoneContainer* destinationZC) {
	position = pos;

	boundingBox.positionBoundingBox(pos);
	spriteCpt.positionSprite(pos);

	unregister();

	if (destinationZC) {
		ZC = destinationZC;
		registerInMaps();
	}
}

void PlayerCharacter::drawDebugInfo(OverWorldDisplay& owDisplay) { 
	drawRectangle(owDisplay.debug_texture, boundingBox.boundingBoxRectReal, sf::Color::Blue);
	drawRectangle(owDisplay.debug_texture, getActivableZone(), sf::Color::Red);
	drawRectangle(owDisplay.debug_texture, getVisibilityRectangle(), sf::Color::Yellow);

	drawPoint(owDisplay.debug_texture, getPosition(), sf::Color::Green);
	drawPoint(owDisplay.debug_texture, getSpriteCenter(), sf::Color::Cyan);
}
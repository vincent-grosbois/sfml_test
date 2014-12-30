#include "entities/PlayerCharacter.h"


PlayerCharacter::PlayerCharacter(sf::Vector2f const& position,  ZoneContainer& ZC, MoveAnimation& move_anim, Overlay& overlay):
Character(position, ZC, move_anim), overlay(overlay), _first_moving(false)
{
	type = EntityType::PLAYER_CHARACTER;
}


void PlayerCharacter::DialogWindow(const std::string& str, bool force_focus, NPC* originator) {

	//overlay.generateTextBox(str, force_focus, originator);

}

void PlayerCharacter::draw(int ticks, OverWorldDisplay& owDisplay) {

	if(_first_moving)
		current_frame = 1;
	else
		current_frame = isMoving ? (current_frame + ticks) %4 : 0;

	sprite.setTexture(move_anim->getTexture() );
	sprite.setTextureRect(move_anim->getFrame(facingDir, current_frame ) );

	owDisplay.overWorld_texture.draw(sprite);
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
	BoundingBoxRectReal = sf::FloatRect(pos.x, pos.y, boundingBoxSize.x,  boundingBoxSize.y);
	positionSprite();

	unregister();

	if (destinationZC) {
		ZC = destinationZC;
		registerInMaps();
	}
}

void PlayerCharacter::drawCollisionBox(OverWorldDisplay& owDisplay) { 
	sf::RectangleShape rect(sf::Vector2f(BoundingBoxRectReal.width, BoundingBoxRectReal.height));
	rect.setPosition(BoundingBoxRectReal.left,BoundingBoxRectReal.top);
	rect.setFillColor(sf::Color(0,0,0,0));
	rect.setOutlineColor(sf::Color::Blue);
	rect.setOutlineThickness(1);
	owDisplay.overWorld_texture.draw(rect); 

	sf::RectangleShape rect2(sf::Vector2f(getActivableZone().width, getActivableZone().height));
	rect2.setPosition(getActivableZone().left,getActivableZone().top);
	rect2.setFillColor(sf::Color(0,0,0,0));
	rect2.setOutlineColor(sf::Color::Red);
	rect2.setOutlineThickness(1);
	owDisplay.overWorld_texture.draw(rect2); 
}


void PlayerCharacter::onNewTileWalked() {

}
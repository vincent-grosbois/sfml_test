#include "entities/PlayerCharacter.h"


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
	boundingBoxRectReal = sf::FloatRect(pos.x, pos.y, boundingBoxSize.x,  boundingBoxSize.y);
	positionSprite();

	unregister();

	if (destinationZC) {
		ZC = destinationZC;
		registerInMaps();
	}
}

void PlayerCharacter::drawDebugInfo(OverWorldDisplay& owDisplay) { 
	sf::RectangleShape rect(sf::Vector2f(boundingBoxRectReal.width, boundingBoxRectReal.height));
	rect.setPosition(boundingBoxRectReal.left,boundingBoxRectReal.top);
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

	sf::RectangleShape rect3(sf::Vector2f(1,1));
	rect3.setPosition(getPosition());
	rect3.setFillColor(sf::Color(0,0,0,0));
	rect3.setOutlineColor(sf::Color::Green);
	rect3.setOutlineThickness(1);
	owDisplay.overWorld_texture.draw(rect3); 

	sf::RectangleShape rect4(sf::Vector2f(1,1));
	rect4.setPosition(getSpriteCenter());
	rect4.setFillColor(sf::Color(0,0,0,0));
	rect4.setOutlineColor(sf::Color::Cyan);
	rect4.setOutlineThickness(1);
	owDisplay.overWorld_texture.draw(rect4); 
}
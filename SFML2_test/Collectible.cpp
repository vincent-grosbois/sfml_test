#include <iostream>

#include "Collectible.hpp"
#include "PlayerCharacter.hpp"


Collectible::Collectible(sf::Vector2f const& position,  ZoneContainer& ZC, const sf::Sprite& sprite):
EntityPhysical(position, ZC)
{
	this->sprite = sprite;
	boundingBoxSize = sprite.getScale();
	positionSprite();
}

bool Collectible::OnActivated(Entity& activator) {
	
	PlayerCharacter* PC = dynamic_cast<PlayerCharacter*>(&activator);

	if(PC) { 

		std::cout << "Item activated!!\n";

		PC->receiveItem(this);

		return true;
	}
	return false;

}

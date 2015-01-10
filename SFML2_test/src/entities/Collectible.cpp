#include <iostream>

#include "entities/Collectible.h"
#include "entities/PlayerCharacter.h"


Collectible::Collectible(sf::Vector2f const& position,  ZoneContainer& ZC, const sf::Sprite& sprite):
EntityPhysical(position, ZC)
{
	spriteCpt.sprite = sprite;
	//boundingBoxSize = sprite.getScale();
	spriteCpt.positionSprite(position);
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

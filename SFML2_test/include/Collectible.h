#pragma once
#include "EntityPhysical.h"
class Collectible : public EntityPhysical
{
public:
	Collectible(sf::Vector2f const& position,  ZoneContainer& ZC, const sf::Sprite& sprite);
	virtual bool OnActivated(Entity& activator);
};


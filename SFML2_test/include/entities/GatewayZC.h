#pragma once
#include "Entity.h"

class GatewayZC : public Entity
{
public:
	GatewayZC(sf::Vector2f const& position,  ZoneContainer& ZC, ZoneContainer& destination, sf::Vector2f position_destination);
	
	virtual bool onCollision(Entity& activator) override;
	
	//virtual void drawCollisionBox(OverWorldDisplay& owDisplay) override {}// ZoneContainer::worldTexture->draw(sf::Shape::Rectangle(sf::FloatRect(position, sf::Vector2f(10,10)), sf::Color::White)); };

	ZoneContainer* getDestination() const { return teleport_ZC; };
	sf::Vector2f getDestinationPos() const { return teleport_position; };

	virtual sf::FloatRect getVisibilityRectangle() const override{ return sf::FloatRect(position.x, position.y,  position.x +  10,  position.y + 10);};
	
	virtual bool intersectsForCollision (const sf::FloatRect& rectangle, sf::FloatRect* result = NULL) override;

protected:
	ZoneContainer* teleport_ZC;
	sf::Vector2f teleport_position;

};


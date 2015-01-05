#pragma once
#include "EntityPhysical.h"


class OverworldGameStateRequest;


class GatewayZC : public EntityPhysical
{
public:
	GatewayZC(const sf::Vector2f& position, ZoneContainer& ZC, OverworldGameStateRequest& request, const std::string& destinationZC, const sf::Vector2f& position_destination);
	
	virtual bool onCollision(Entity& activator) override;
	
	virtual ~GatewayZC() override { unregister(); }

	std::string getDestination() const { return destinationZC; }
	sf::Vector2f getDestinationPos() const { return destinationPosition; }
	bool isDestinationInThisZC() const { return destinationZC.empty(); }

	virtual sf::FloatRect getVisibilityRectangle() const override{ return sf::FloatRect(position.x, position.y,  position.x +  10,  position.y + 10);};


protected:
	std::string destinationZC;
	sf::Vector2f destinationPosition;
	OverworldGameStateRequest& request;

};


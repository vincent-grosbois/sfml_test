#include "entities/GatewayZC.h"
#include "entities/PlayerCharacter.h"
#include "overworld/overworldGameStateRequest.h"

GatewayZC::GatewayZC(const sf::Vector2f& position, ZoneContainer& ZC, OverworldGameStateRequest& request, const std::string& destinationZC, const sf::Vector2f& position_destination):
EntityPhysical(position, ZC),
destinationZC(destinationZC),
destinationPosition(position_destination),
request(request)
{ 
	registerInMaps();
	type = EntityType::GATEWAY;
}


bool GatewayZC::onCollision(Entity& activator) { 

	if(activator.getType() != EntityType::PLAYER_CHARACTER)
		return false;

	std::cout << "collision!\n" ;

	request.pushZoneChangeRequest(destinationZC, destinationPosition);

	return true; 
}


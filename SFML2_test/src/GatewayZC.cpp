#include "entities/GatewayZC.h"
#include "entities/PlayerCharacter.h"


GatewayZC::GatewayZC(sf::Vector2f const& position,  ZoneContainer& ZC, ZoneContainer& destination, sf::Vector2f position_destination):
Entity(position, ZC),
teleport_ZC(&destination),
teleport_position(position_destination)
{ }


bool GatewayZC::onCollision(Entity& activator) { 

	PlayerCharacter* PC = dynamic_cast<PlayerCharacter*>(&activator);

	if(!PC)
		return false;

	std::cout << "collision!\n" ;
	
	//OverWorldGameState::setTransition(teleport_position, teleport_ZC);
	

	return true; 
}

bool GatewayZC::intersectsForCollision(const sf::FloatRect& rectangle, sf::FloatRect* result) 
{

	sf::FloatRect BoundingBoxRectReal(position, sf::Vector2f(10,10));
	// Compute overlapping rect
	float left =  std::max(BoundingBoxRectReal.left,   rectangle.left);
	float top =  std::max(BoundingBoxRectReal.top,    rectangle.top);
	float right = std::min(BoundingBoxRectReal.left + BoundingBoxRectReal.width,  rectangle.left + rectangle.width);
	float bottom = std::min(BoundingBoxRectReal.top + BoundingBoxRectReal.height, rectangle.top + rectangle.height);
	sf::Rect<float> OverlappingRect( left,
		top,
		right - left,
		bottom-top);


	// If overlapping rect is valid, then there is intersection
	if ((OverlappingRect.left <(  OverlappingRect.left + OverlappingRect.width )) && (OverlappingRect.top < (OverlappingRect.top +  OverlappingRect.height)))
	{
		return true;
	}
	else
	{
		return false;
	}
}
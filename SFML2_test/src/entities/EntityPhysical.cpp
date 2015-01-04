#include <algorithm>

#include "entities/EntityPhysical.h"
#include "ZoneContainer.h"
#include "overworld/OverWorldScene.h"


EntityPhysical::EntityPhysical(const sf::Vector2f& position,  ZoneContainer& ZC):
	Entity(position, ZC),
	boundingBoxSize(16,15),
	spriteOffset(0,0),
	boundingBoxOffset(-boundingBoxSize.x/2, -boundingBoxSize.y/2),
	boundingBoxRectReal(position.x, position.y, boundingBoxSize.x, boundingBoxSize.y)
{
	positionSprite();
	positionBoundingBox();
	registerInMaps();
}


void EntityPhysical::positionSprite() {
	sprite.setPosition(position + spriteOffset);
}

void EntityPhysical::positionBoundingBox() {
	boundingBoxRectReal.left = position.x + boundingBoxOffset.x;
	boundingBoxRectReal.top = position.y + boundingBoxOffset.y;
}


void EntityPhysical::draw(OverWorldDisplay& owDisplay) {
	owDisplay.overWorld_texture.draw(sprite);
}

bool EntityPhysical::intersectsForCollision(const sf::FloatRect& rectangle, sf::FloatRect* result) 
{
	// Compute overlapping rect
	float left = std::max(boundingBoxRectReal.left, rectangle.left);
	float top =  std::max(boundingBoxRectReal.top, rectangle.top);
	float right = std::min(boundingBoxRectReal.left + boundingBoxRectReal.width,  rectangle.left + rectangle.width);
	float bottom = std::min(boundingBoxRectReal.top + boundingBoxRectReal.height, rectangle.top + rectangle.height);
	
	sf::FloatRect OverlappingRect (left, top, right - left, bottom-top);

	if(result)
		*result = OverlappingRect;

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

void EntityPhysical::drawDebugInfo(OverWorldDisplay& owd) { 
	sf::RectangleShape rect(sf::Vector2f(boundingBoxRectReal.width, boundingBoxRectReal.height));
	rect.setPosition(boundingBoxRectReal.left,boundingBoxRectReal.top);
	rect.setFillColor(sf::Color(0,0,0,0));
	rect.setOutlineColor(sf::Color::White);
	rect.setOutlineThickness(1);
	owd.overWorld_texture.draw(rect); 
}
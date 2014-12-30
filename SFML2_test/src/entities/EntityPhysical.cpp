#include <algorithm>

#include "entities/EntityPhysical.h"
#include "ZoneContainer.h"
#include "OverWorldScene.h"


EntityPhysical::EntityPhysical(const sf::Vector2f& position,  ZoneContainer& ZC):
	Entity(position, ZC),
	boundingBoxSize(16,15),
	spriteOffset(0,0),
	BoundingBoxRectReal(position.x, position.y, boundingBoxSize.x, boundingBoxSize.y)
{
	positionSprite();
	registerInMaps();
}


void EntityPhysical::positionSprite() {
	sprite.setPosition(position + spriteOffset);
}


void EntityPhysical::draw(int ticks, OverWorldDisplay& owDisplay) {
	owDisplay.overWorld_texture.draw(sprite);
}

bool EntityPhysical::intersectsForCollision(const sf::FloatRect& rectangle, sf::FloatRect* result) 
{
	// Compute overlapping rect
	float left = std::max(BoundingBoxRectReal.left, rectangle.left);
	float top =  std::max(BoundingBoxRectReal.top, rectangle.top);
	float right = std::min(BoundingBoxRectReal.left + BoundingBoxRectReal.width,  rectangle.left + rectangle.width);
	float bottom = std::min(BoundingBoxRectReal.top + BoundingBoxRectReal.height, rectangle.top + rectangle.height);
	
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

void EntityPhysical::drawCollisionBox(OverWorldDisplay& owd) { 
	sf::RectangleShape rect(sf::Vector2f(BoundingBoxRectReal.width, BoundingBoxRectReal.height));
	rect.setPosition(BoundingBoxRectReal.left,BoundingBoxRectReal.top);
	rect.setFillColor(sf::Color(0,0,0,0));
	rect.setOutlineColor(sf::Color::White);
	rect.setOutlineThickness(1);
	owd.overWorld_texture.draw(rect); 
}
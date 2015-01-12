#include <algorithm>

#include "entities/EntityPhysical.h"
#include "ZoneContainer.h"
#include "overworld/OverworldScene.h"


EntityPhysical::EntityPhysical(const sf::Vector2f& position,  ZoneContainer& ZC):
	Entity(position, ZC),
	boundingBox(position, sf::Vector2f(16,15))
{
	spriteCpt.spriteOffset = sf::Vector2f(0,0);
	spriteCpt.positionSprite(position);
	registerInMaps();
}

void EntityPhysical::draw(OverworldDisplay& owDisplay) {
	owDisplay.overWorld_texture.draw(spriteCpt.sprite);
}

bool EntityPhysical::intersectsForCollision(const sf::FloatRect& rectangle, sf::FloatRect* result) 
{
	// Compute overlapping rect
	float left = std::max(boundingBox.boundingBoxRectReal.left, rectangle.left);
	float top =  std::max(boundingBox.boundingBoxRectReal.top, rectangle.top);
	float right = std::min(boundingBox.boundingBoxRectReal.left + boundingBox.boundingBoxRectReal.width,  rectangle.left + rectangle.width);
	float bottom = std::min(boundingBox.boundingBoxRectReal.top + boundingBox.boundingBoxRectReal.height, rectangle.top + rectangle.height);
	
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

void EntityPhysical::drawDebugInfo(OverworldDisplay& owd) { 
	//
}
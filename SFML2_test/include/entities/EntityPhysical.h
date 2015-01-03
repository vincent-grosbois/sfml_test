#pragma once
#include "Entity.h"

#include <SFML/Graphics/Sprite.hpp>

class EntityPhysical : public Entity
{

public:
	EntityPhysical(const sf::Vector2f & position,  ZoneContainer& ZC);
	virtual void draw(OverWorldDisplay& owDisplay) override;

	virtual ~EntityPhysical() override {
		unregister();
	}

protected:
	sf::Vector2f boundingBoxSize;
	sf::FloatRect BoundingBoxRectReal;
	sf::Sprite sprite;
	sf::Vector2f spriteOffset;
	sf::Vector2f boundingBoxOffset;
	void positionSprite();
	void positionBoundingBox();

public:
	virtual void drawDebugInfo(OverWorldDisplay& owDisplay) override;

	sf::Vector2f getSpriteCenter() const { 
		return sprite.getPosition() + sf::Vector2f(sprite.getGlobalBounds().width/2, sprite.getGlobalBounds().height/2); 
	}
	
	sf::FloatRect getSpriteRectangle() const { 
		return sprite.getGlobalBounds();
	}

	virtual sf::FloatRect getVisibilityRectangle() const override { return getSpriteRectangle();}

	virtual sf::FloatRect getPresenceRectangle() const override{ return BoundingBoxRectReal; }

	sf::FloatRect getBoundingBoxRectReal() const { return BoundingBoxRectReal; }

	virtual bool intersectsForCollision(const sf::FloatRect& rectangle, sf::FloatRect* result = NULL) override;
};


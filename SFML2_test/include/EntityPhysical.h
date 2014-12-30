#pragma once
#include "Entity.h"

#include <SFML/Graphics/Sprite.hpp>

class EntityPhysical : public Entity
{

public:
	EntityPhysical(sf::Vector2f const& position,  ZoneContainer& ZC);
	virtual void draw(int ticks, OverWorldDisplay& owDisplay) override;

public:
	sf::Vector2f boundingBoxSize;

	virtual ~EntityPhysical() override {
		unregister();
	}

protected:
	sf::FloatRect BoundingBoxRectReal;
	sf::Sprite sprite;
	sf::Vector2f spriteOffset;
	void positionSprite();

public:
	virtual void drawCollisionBox(OverWorldDisplay& owDisplay) override;

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


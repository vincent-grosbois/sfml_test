#pragma once
#include "Entity.h"

#include <SFML/Graphics/Sprite.hpp>


struct BoundingBoxCpt {

	sf::Vector2f boundingBoxOffset;
	sf::FloatRect boundingBoxRectReal;

	BoundingBoxCpt(const sf::Vector2f& position, const sf::Vector2f& size):
		boundingBoxOffset(-size.x/2, -size.y/2),
		boundingBoxRectReal(position.x, position.y, size.x, size.y)
	{ 
		positionBoundingBox(position);
	}

	void positionBoundingBox(const sf::Vector2f & position) {
		boundingBoxRectReal.left = position.x + boundingBoxOffset.x;
		boundingBoxRectReal.top = position.y + boundingBoxOffset.y;
	} 

};

struct SpriteCpt {
	sf::Sprite sprite;
	sf::Vector2f spriteOffset;

	void positionSprite(const sf::Vector2f& position) {
		sprite.setPosition(position + spriteOffset);
	}
};

class EntityPhysical : public Entity
{

public:
	EntityPhysical(const sf::Vector2f & position,  ZoneContainer& ZC);

	virtual ~EntityPhysical() override {
		unregister();
	}

	virtual void draw(OverworldDisplay& owDisplay) override;

	virtual void drawDebugInfo(OverworldDisplay& owDisplay) override;

	virtual sf::FloatRect getVisibilityRectangle() const override { return getSpriteRectangle();}

	virtual sf::FloatRect getPresenceRectangle() const override { return boundingBox.boundingBoxRectReal; }

	virtual bool intersectsForCollision(const sf::FloatRect& rectangle, sf::FloatRect* result = NULL) override;

	sf::Vector2f getSpriteCenter() const { 
		return spriteCpt.sprite.getPosition() + sf::Vector2f(spriteCpt.sprite.getGlobalBounds().width/2, spriteCpt.sprite.getGlobalBounds().height/2); 
	}

protected:
	SpriteCpt spriteCpt;

	BoundingBoxCpt boundingBox;

	sf::FloatRect getSpriteRectangle() const { 
		return spriteCpt.sprite.getGlobalBounds();
	}

	sf::FloatRect getBoundingBoxRectReal() const { return boundingBox.boundingBoxRectReal; }

};


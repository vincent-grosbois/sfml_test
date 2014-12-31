#pragma once

#include "EntityPhysical.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <iostream>

class Projectile : public EntityPhysical
{
public:
	Projectile(const sf::Vector2f & position,  ZoneContainer& ZC);
	virtual ~Projectile(void);
	virtual void update(int delta_ms) override;
	virtual void draw(OverWorldDisplay& owDisplay) override;
	virtual void drawCollisionBox(OverWorldDisplay& owDisplay) override;
	virtual sf::FloatRect getVisibilityRectangle() const override { return this->BoundingBoxRectReal;}

	virtual bool onCollision(Entity& activator) override {
		if(activator.getType() == EntityType::NPC) {
			//std::cout << "marked " << &activator << " for deletion due to collision\n";
			activator.markForDeletion();
			markForDeletion();
			return true;
		}
		return false;
	}

protected:
	float speed;
	float angle;

	sf::Color color;
	int sides;
	float radius;
	bool isOn;
	sf::VertexArray lightZone;
	sf::VertexArray lightZone2;
};


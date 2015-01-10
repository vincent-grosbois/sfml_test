#pragma once

#include "EntityPhysical.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include "utils/AngleUtils.h"
#include "../Constants.h"

struct LightEmitter {

	sf::VertexArray lightZone;

	int sides;
	float radius;
	sf::Color color;

	LightEmitter(const sf::Vector2f& pos, int sides, float radius, sf::Color& color);

	void moveVertexArray(const sf::Vector2f& pos);

	void rebuildVertexArray(const sf::Vector2f& pos);

	void recolorVertexArray(const sf::Vector2f& pos);


	void draw(OverWorldDisplay& owDisplay) const;
};

class Projectile : public EntityPhysical
{
public:
	Projectile(const sf::Vector2f & position,  ZoneContainer& ZC, DIRECTION::e dir);
	virtual ~Projectile(void);
	virtual void update(int delta_ms, bool will_be_drawn) override;
	virtual void draw(OverWorldDisplay& owDisplay) override;
	virtual void drawDebugInfo(OverWorldDisplay& owDisplay) override;
	virtual sf::FloatRect getVisibilityRectangle() const override;

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
	int lifetime_ms;
	LightEmitter light1;
	LightEmitter light2;
};


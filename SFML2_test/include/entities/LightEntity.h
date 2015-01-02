#pragma once
#include "Entity.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/VertexArray.hpp>

class LightEntity : public Entity
{
public:
	LightEntity(sf::Vector2f const& position,  ZoneContainer& ZC, float radius, int sides, sf::Color color);

	virtual void draw(OverWorldDisplay& owDisplay) override;
	virtual sf::FloatRect getVisibilityRectangle() const override;

	virtual void registerInMaps() override;
	virtual void unregister() override;

	virtual ~LightEntity() override;

	virtual void update(int delta_ms) override;

	//float getRadius() { return radius; };

	void setOn(bool on) {
		isOn = on;
	}

	bool isOn_() const {
		return isOn;
	}

	void setPosition(const sf::Vector2f& pos);
	
protected:
	sf::Color color;
	int sides;
	float radius;
	bool isOn;
	sf::VertexArray lightZone;

};


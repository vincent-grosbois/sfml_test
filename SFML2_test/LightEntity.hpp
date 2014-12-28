#pragma once
#include "Entity.hpp"

class LightEntity :
	public Entity
{
public:
	LightEntity(sf::Vector2f const& position,  ZoneContainer& ZC, float radius = 1000.f, int sides = 20, sf::Color color = sf::Color(10,10,10,255));
	

	virtual void draw(int tick, OverWorldDisplay& owDisplay) override;
	virtual sf::FloatRect getVisibilityRectangle() const override;

	float getRadius() { return radius; };
	
protected:
	sf::Color color;

	int sides;
	float radius;
	bool isOn;
	sf::VertexArray lightZone;

};


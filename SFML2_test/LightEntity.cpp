#include "LightEntity.hpp"

#include "ZoneContainer.hpp"
#include "OverWorldScene.hpp"
#include <cassert>


LightEntity::LightEntity(sf::Vector2f const& position,  ZoneContainer& ZC, float radius, int sides, sf::Color color):
	Entity(position, ZC),
	sides(sides), color(color),
	radius(radius),
	lightZone(sf::TrianglesFan, sides+2),
	isOn(true)
{

	this->updateEntityPositionInfo();
	assert(!this->locationList.empty());
	float deuxpi = float(2*3.14159265);
	float angle = 0.f;
	lightZone[0].position = getPosition();
	lightZone[0].color = color;
	for(int i = 1; i<sides+2; ++i) {
		lightZone[i].position = getPosition() + sf::Vector2f(radius*cos(angle),radius*sin(angle));
		lightZone[i].color = sf::Color(0, 0, 0, 0);

		angle += deuxpi/sides;
	}

}


void LightEntity::draw(int tick, OverWorldDisplay& owDisplay) {
	if(isOn){
		owDisplay.light_texture.draw(lightZone, sf::RenderStates(sf::BlendMode::BlendAdd));	
	}
}

sf::FloatRect LightEntity::getVisibilityRectangle() const { 
	return sf::FloatRect(position.x - radius, position.y - radius,  radius*2,  radius*2);
}
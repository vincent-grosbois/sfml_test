#include "entities/LightEntity.h"

#include "ZoneContainer.h"
#include "OverWorldScene.h"
#include "ZoneContainer.h"
#include "TilePlane.h"
#include "Map.h"
#include "utils/SetUnionIterator.h"

#include <cassert>


LightEntity::LightEntity(sf::Vector2f const& position,  ZoneContainer& ZC, float radius, int sides, sf::Color color):
	Entity(position, ZC),
	sides(sides), color(color),
	radius(radius),
	lightZone(sf::TrianglesFan, sides+2),
	isOn(true)
{
	type = EntityType::LIGHT;
	registerInMaps();
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


void LightEntity::update(int delta_ms) {
	//color.a += 0.1*delta_ms;
	//color.g += 0.1*delta_ms;
	//color.b += 0.03*delta_ms;
	//lightZone[0].color = color;
}

void LightEntity::draw(OverWorldDisplay& owDisplay) {
	if(isOn){
		owDisplay.light_texture.draw(lightZone, sf::RenderStates(sf::BlendAdd));
	}
}

sf::FloatRect LightEntity::getVisibilityRectangle() const { 
	return sf::FloatRect(position.x - radius, position.y - radius,  2*radius,  2*radius);
}

struct UpdateElementSetOldNew {

	UpdateElementSetOldNew(Entity* e) : e(e) { }

	//elements only in the old set :
	void visit_first(EntitySet* elt) {
		elt->remove_entity(e);
	}

	//elements only in the new set : 
	void visit_second(EntitySet* elt) {
		elt->add_entity(e);
	}

	void visit_both(EntitySet*, EntitySet*) {
		//do nothing
	}

	Entity* e;
};



LightEntity::~LightEntity() {
	unregister();
}

void LightEntity::setPosition(const sf::Vector2f& pos) {

	if(position == pos) {
		return;
	}

	position = pos;
	registerInMaps();
	assert(!this->locationList.empty());
	float deuxpi = float(2*3.14159265);
	float angle = 0.f;
	lightZone[0].position = getPosition();
	for(int i = 1; i<sides+2; ++i) {
		lightZone[i].position = getPosition() + sf::Vector2f(radius*cos(angle),radius*sin(angle));
		angle += deuxpi/sides;
	}

}
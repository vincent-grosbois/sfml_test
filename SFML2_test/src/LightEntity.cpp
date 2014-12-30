#include "LightEntity.h"

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

void LightEntity::draw(int tick, OverWorldDisplay& owDisplay) {
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


void LightEntity::registerInMaps() {
	//maps that the entity is visible in:
	std::set<Map*> colliding_maps = ZC->getCollidingMaps(getVisibilityRectangle());

	//delete the maps we are not in anymore, and unregister from their global list of entities:
	for (auto it = locationList.begin() ; it != locationList.end(); ) {
		if( colliding_maps.count ( it->first ) == 0 ) {

			//std::cout << "leaving map\n";
			//OnLeavingMap();
			for(auto entity_set = it->second.begin(); entity_set != it->second.end(); ++entity_set) {
				(**entity_set).remove_entity(this);
			}

			//remove from the map's global list of characters:
			it->first->lights_list().erase(this);

			//remove from own list of tiles:
			locationList.erase(it++);
		}
		else 
			++it;
	}


	for(std::set<Map*>::const_iterator map = colliding_maps.begin(); map != colliding_maps.end(); ++map) {

		std::set<EntitySet*> new_set;

		//get the set of tiles that collide with the character in the given map
		(*map)->getCollidingTiles(getPresenceRectangle(), new_set);

		std::set<EntitySet*>& old_set = locationList[*map];

		if(old_set.empty())  { 
			//insert ourself in the global list of characters from the map:
			(*map)->lights_list().insert(this);
		} 

		iterate_over_union(old_set, new_set, UpdateElementSetOldNew(this));

		//insert the new map into our own list of maps:
		old_set = std::move(new_set); 

	}

}

void LightEntity::unregister() {

	for(auto it = locationList.begin(); it != locationList.end(); ++it) {
		it->first->lights_list().erase(this);

		for(auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
					(**it2).remove_entity(this);
		}
	}

	locationList.clear();
}

LightEntity::~LightEntity() {
	unregister();
}
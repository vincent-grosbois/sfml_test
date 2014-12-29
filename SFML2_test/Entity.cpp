#include <map>

#include "Entity.hpp"
#include "ZoneContainer.hpp"
#include "MapTile.hpp"
#include "TilePlane.hpp"
#include "Map.hpp"
#include "utils/SetUnionIterator.h"


Entity::Entity(const sf::Vector2f& position,  ZoneContainer& ZC): 
	position(position),
	ZC(&ZC),
	markedForDeletion(false)
{
	updateEntityPositionInfo();
}


void Entity::unregister() {

	for(auto it = locationList.begin(); it != locationList.end(); ++it) {
		it->first->entities_list().erase(this);

		for(auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
					(**it2).remove_entity(this);
		}
	}

	locationList.clear();
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

void Entity::updateEntityPositionInfo() { 

	//maps that the entity is in:
	std::set<Map*> colliding_maps = ZC->getCollidingMaps(getVisibilityRectangle());

	//delete the maps we are not in anymore, and unregister from their global list of characters:
	for (auto it = locationList.begin() ; it != locationList.end(); ) {
		if( colliding_maps.count ( it->first ) == 0 ) {

			//std::cout << "leaving map\n";
			//OnLeavingMap();
			for(auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
				(**it2).remove_entity(this);
			}

			//remove from the map's global list of characters:
			it->first->entities_list().erase(this);

			//remove from own list of tiles:
			locationList.erase(it++);
		}
		else 
			++it;
	}


	for(std::set<Map*>::const_iterator it2 = colliding_maps.begin(); it2 != colliding_maps.end(); ++it2) {

		std::set<EntitySet*> new_set;

		//get the set of tiles that collide with the character in the given map
		(*it2)->getCollidingTiles(getPresenceRectangle(), new_set);

		std::set<EntitySet*>& old_set = locationList[*it2];

		if(old_set.empty())  { 
			//insert ourself in the global list of characters from the map:
			(*it2)->entities_list().insert(this);
		} 

		iterate_over_union(old_set, new_set, UpdateElementSetOldNew(this));

		//insert the new map into our own list of maps:
		old_set = std::move(new_set); 

	}
}

void Entity::debug_dump_positions() {
	/*
	std::cout << "----Dumping position start\n";
	for (auto it= locationList.begin() ; it != locationList.end(); ++it ) {


		std::cout << "Dumping map "<< (*it).first <<" \n";

		for(int i =0; i<60; ++i) {
			for(int j =0; j<60; ++j) {
				if ((*it).first->entities_grid(i,j).has_entities()) {
						std::cout << "tile <"<<i<<","<<j<<"> has entities<n";
				}
			}
		}

	}
	std::cout << "----Dumping end\n";*/

}

void Entity::markForDeletion() { 
	markedForDeletion = true;
	ZC->addForDeletion(this);
}
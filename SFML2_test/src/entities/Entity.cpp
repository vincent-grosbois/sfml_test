#include <map>

#include "entities/Entity.h"
#include "ZoneContainer.h"
#include "TilePlane.h"
#include "Map.h"
#include "utils/SetUnionIterator.h"


Entity::Entity(const sf::Vector2f& position,  ZoneContainer& ZC): 
	position(position),
	ZC(&ZC),
	markedForDeletion(false)
{ }


void Entity::unregister() {
	//std::cout << "unregistering " << this << '\n';
	for(auto it = locationList.begin(); it != locationList.end(); ++it) {
		it->first->entities_list().erase(this);
		//std::cout << "erasing " << this << " from " << it->first->getId() << '\n';
		for(auto it2 = it->second.first.begin(); it2 != it->second.first.end(); ++it2) {
			(**it2).remove_entity(this);
		}
		for(auto it2 = it->second.second.begin(); it2 != it->second.second.end(); ++it2) {
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

void Entity::registerInMaps() { 

	//maps that the entity is visible in:
	std::set<Map*> colliding_maps = ZC->getCollidingMaps(getVisibilityRectangle());

	//delete the maps we are not in anymore, and unregister from their global list of entities:
	for (auto it = locationList.begin() ; it != locationList.end(); ) {
		if( colliding_maps.count ( it->first ) == 0 ) {

			//std::cout << "leaving map\n";
			//OnLeavingMap();
			for(auto entity_set = it->second.first.begin(); entity_set != it->second.first.end(); ++entity_set) {
				(**entity_set).remove_entity(this);
			}
			for(auto entity_set = it->second.second.begin(); entity_set != it->second.second.end(); ++entity_set) {
				(**entity_set).remove_entity(this);
			}
			//remove from the map's global list of characters:
			//std::cout << "unregister " << this << '\n';
			it->first->entities_list().erase(this);

			//remove from own list of tiles:
			locationList.erase(it++);
		}
		else 
			++it;
	}


	for(std::set<Map*>::const_iterator map = colliding_maps.begin(); map != colliding_maps.end(); ++map) {

		std::set<EntitySet*> new_set_presence;
		std::set<EntitySet*> new_set_vibility;
		//get the set of tiles that collide with the character in the given map
		(*map)->getCollidingEntitySets(getPresenceRectangle(), new_set_presence);
		(*map)->getCollidingVisibilitySets(getVisibilityRectangle(), new_set_vibility);

		std::set<EntitySet*>& old_set_presence = locationList[*map].first;
		std::set<EntitySet*>& old_set_visibility = locationList[*map].second;

		if(old_set_presence.empty() || old_set_visibility.empty())  { 
			//insert ourself in the global list of characters from the map:
			(*map)->entities_list().insert(this);
		} 

		iterate_over_union(old_set_presence, new_set_presence, UpdateElementSetOldNew(this));
		iterate_over_union(old_set_visibility, new_set_vibility, UpdateElementSetOldNew(this));

		//insert the new map into our own list of maps:
		old_set_presence = std::move(new_set_presence); 
		old_set_visibility = std::move(new_set_vibility);
	}

	if(colliding_maps.empty()) {
		//std::cout << "marking for deletion " << this << '\n';
		markForDeletion();
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
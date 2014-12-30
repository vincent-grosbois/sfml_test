#pragma once

#include <iostream>
#include <set>

#include <SFML/Graphics.hpp>

#include "Constants.hpp"
#include "utils/StorageProvider.h"
#include "utils/Array2D.h"

class TilePlane;
class Entity;
class LightEntity;
class ZoneContainer;
struct OverWorldDisplay;
class OverWorldCamera;

struct NoElementPredicate {
	bool operator()(const std::set<Entity*>& set) {
		return set.empty();
	}

	void transform(const std::set<Entity*>& set) {
		//do nothing
	}
};


struct EntitySet {
	typedef std::list<std::set<Entity*>>::iterator set_t;

	set_t entities_on_tile; 

	EntitySet() : 
		entities_on_tile(entitySetStorage.default_value())
	{ }

	~EntitySet() {
		entitySetStorage.return_to_storage(entities_on_tile);
	}

	bool has_entities() const {
		return entities_on_tile != entitySetStorage.default_value();
	}

	bool has_entity(Entity* entity) const {
		return has_entities() && entities_on_tile->find(entity) != entities_on_tile->end();
	}

	void remove_entity(Entity* entity) {
		if(has_entities()) {
			std::set<Entity*>::iterator it = entities_on_tile->find(entity);
			if( it != entities_on_tile->end() ) {
				entities_on_tile->erase(it);
				if( entities_on_tile->empty()) {
					entitySetStorage.return_to_storage(entities_on_tile);
				} 
			}
		}
	}

	void add_entity(Entity* entity) {
		if(!has_entities()) {
			entities_on_tile = entitySetStorage.request_from_storage();
		} 
		entities_on_tile->insert(entity);
	}


	std::set<Entity*>& entities() {
		return *entities_on_tile;
	}

	static StorageProvider<std::set<Entity*>, NoElementPredicate> entitySetStorage;
};

class Map
{
public:
	Map(ZoneContainer& ZC, TilePlane* layer0, TilePlane* layer1, TilePlane* layer2, sf::Vector2<tile_units> position, int id);

	~Map();

	void drawLayer(const sf::View& view, OverWorldDisplay& owDisplay, int layer);

	void printDebug() const;

	void unloadAll();

	void updateGraphics(const OverWorldCamera& camera, bool checkAnimatedTilesUpdate);
	void loadTilesFromNothing(const OverWorldCamera& camera);

	sf::Vector2i getMapCoords() const { 
		return sf::Vector2i(position.x/size.x, position.y/size.y);
	}

	int getId() const { 
		return id;
	}

	void getCollidingTiles(const sf::FloatRect& rect, std::set<EntitySet*>& result);

	void dumpLoadedTiles() const ;

	std::set<Entity*>& entities_list()  {
		return entities_on_map;
	}

	std::set<LightEntity*>& lights_list()  {
		return lights_on_map;
	}

	bool collideWithLayer(int layer_id, const sf::FloatRect& rect, sf::Vector2f* collidingPos) const;

private:
	TilePlane* layer0;
	TilePlane* layer1;
	TilePlane* layer2;

	sf::Vector2<tile_units> position;
	sf::Vector2<tile_units> size;
	sf::Vector2<tile_units> offset;

	std::set<Entity*> entities_on_map;
	std::set<LightEntity*> lights_on_map;

	Array2D<EntitySet> entities_grid;


	int id;
	ZoneContainer* myZC;

private:
	Map(const Map&);
	Map& operator=(const Map&);
};
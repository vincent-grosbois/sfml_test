#pragma once

#include <set>
#include <memory>

#include "Map.hpp"
#include "GameClock.hpp"
#include "utils/Array2D.h"
#include "Entity.hpp"
#include "ZoneContainerData.h"

class LightEntity;
class Tileset;

class ZoneContainer
{
public:
	ZoneContainer(const std::string& dataFile);
	~ZoneContainer();

	void dumpLoadedTiles() const;

private:
	ZoneContainerData data;

	std::string name;
	Tileset& tileset;
	Array2D<Map*> maps;
	std::set<std::unique_ptr<Entity>> to_delete;

public:

	std::string getEntitiesDataPath() const {
		return data.entitiesDataPath;
	}

	Tileset& getTileset() {
		return tileset;
	}

	void deleteElements() {
		to_delete.clear();
	}

	void addForDeletion(Entity* e) {
		to_delete.insert(std::unique_ptr<Entity>(e));
	}

	std::set<Map*> getCollidingMaps(const sf::FloatRect& rect) ;

	bool isOutside;
	bool isDark;

	static std::vector<LightEntity*> light_list;

	sf::Vector2f startingPos;

};


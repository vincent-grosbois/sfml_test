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
class GameResource;

class ZoneContainer
{
public:
	ZoneContainer(const std::string& dataFile, GameResource& gr);
	~ZoneContainer();

	void dumpLoadedTiles() const;

private:
	ZoneContainerData data;
	Tileset& tileset;
	Array2D<Map*> maps;
	std::set<std::unique_ptr<Entity>> to_delete;

public:
	const ZoneContainerData& getData() const {
		return data;
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
};


#pragma once

#include <set>
#include <memory>

#include <SFML/Graphics/Rect.hpp>

#include "utils/Array2D.h"
#include "ZoneContainerData.h"

class LightEntity;
class Tileset;
class GameResource;
class Map;
class Entity;

class ZoneContainer
{
public:
	ZoneContainer(const std::string& dataFile, GameResource& gr);
	~ZoneContainer();

	const ZoneContainerData& getData() const;
	Tileset& getTileset();
	std::set<Map*> getCollidingMaps(const sf::FloatRect& rect) ;

	void dumpLoadedTiles() const;
	void deleteElements();
	void addForDeletion(Entity* e);

private:
	ZoneContainerData data;
	Tileset& tileset;
	Array2D<Map*> maps;
	std::set<std::unique_ptr<Entity>> to_delete;
};


#pragma once

#include <set>

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
	ZoneContainer(const ZoneContainer&);
	ZoneContainer& operator=(ZoneContainer&);

private:
	ZoneContainerData data;
	Tileset& tileset;
	Array2D<Map*> maps;
	std::set<Entity*> to_delete;
};


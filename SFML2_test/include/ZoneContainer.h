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

	const Array2D<int>& getCollisionArray() const  { return collisionArray; };

private:
	ZoneContainer(const ZoneContainer&);
	ZoneContainer& operator=(ZoneContainer&);

	void createCollisionArray(Array2D<int>& );

private:
	ZoneContainerData data;
	Tileset& tileset;
	Array2D<Map*> maps;
	Array2D<int> collisionArray;
	std::set<Entity*> to_delete;
};


#pragma once

#include <map>
#include <set>
#include <cassert>

#include <SFML/Graphics.hpp>

#include "Tile.hpp"
#include "ZoneContainer.hpp"
#include "utils/StorageProvider.h"
#include "OverWorldScene.hpp"

class Map;
class Tileset;
class Entity;


struct MapElement {

	MapElement() 
	{ }

	void init(Tile& tile)
	{
		theTile = &tile;
	}

	Tile& tile() { 
		return *theTile;
	}

	const Tile& tile() const {
		return *theTile;
	}

private:
	Tile* theTile; 
};

class TilePlane {


public:
	TilePlane(Tileset& tileset, sf::Vector2<tile_units> size, sf::Vector2<tile_units> offset, Array2D<int>& table);

	void draw(const sf::View& view, OverWorldDisplay& owDisplay);

	bool collideWith(const sf::FloatRect& rect, sf::Vector2f* CollidingPos) const;
	void getCollidingTiles(const sf::FloatRect& rect, std::set<MapElement*>& result);
	void unloadAllGraphics();
	void updateGraphics(const OverWorldCamera& camera,  bool checkAnimatedTilesUpdate);
	void loadAndWakeUp(const OverWorldCamera& camera);
	bool isGraphicsTotallyUnloaded() const { return graphicsTotallyUnloaded; };

	void dumpLoadedTiles() const ;

	Map* myMap;

public:
	Array2D<MapElement> elements;
	sf::Vector2<tile_units> size;   // <* map size, in tile units
	sf::Vector2<tile_units> offset; // <* map offset, in tile units
	Array2D<sf::VertexArray> tileBlocks;

protected:
	bool graphicsTotallyUnloaded;
	Tileset& tileset;
	void unloadGraphics(const sf::FloatRect& rect);
	sf::Vertex* getQuadVertexFromTileIndex(int x, int y);
	void setQuadTextureToFrame(sf::Vertex* quad, const sf::IntRect& rect);
};
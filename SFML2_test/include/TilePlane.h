#pragma once

#include <map>
#include <set>
#include <cassert>

#include <SFML/Graphics.hpp>

#include "utils/Array2D.h"
#include "Constants.h"

class Map;
class Tileset;
class Entity;
class OverWorldCamera;
struct OverWorldDisplay;
struct Tile;

struct MapElement {

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
	TilePlane(Tileset& tileset, sf::Vector2<tile_units> size, sf::Vector2<tile_units> offset, Array2D<int>& table, bool waterPlane = false);

	void draw(const sf::View& view, OverWorldDisplay& owDisplay);

	bool collideWith(const sf::FloatRect& rect, sf::Vector2f* CollidingPos) const;
	void getCollidingEntitySets(const sf::FloatRect& rect, std::set<MapElement*>& result);
	void unloadAllGraphics();
	void updateGraphics(const OverWorldCamera& camera,  bool checkAnimatedTilesUpdate, int deltaTime);
	void loadAndWakeUp(const OverWorldCamera& camera);
	sf::Vector2i size() const; // <* map size, in tile units

	void dumpLoadedTiles() const ;

	Map* myMap;

public:
	Array2D<MapElement> elements;
	sf::Vector2<tile_units> offset; // <* map offset, in tile units
	Array2D<sf::VertexArray> tileBlocks;

protected:
	Tileset& tileset;
	void unloadGraphics(const sf::FloatRect& rect);
	sf::Vertex* getQuadVertexFromTileIndex(int x, int y);
	void setQuadTextureToFrame(sf::Vertex* quad, const sf::IntRect& rect);
	bool water_plane;
};
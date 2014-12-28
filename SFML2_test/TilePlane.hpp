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


struct CorrectSpriteSizePredicate {
	bool operator()(const sf::Sprite& sprite) {
		return true;
	}

	void transform(sf::Sprite& sprite) {
		sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
	}
};



struct MapElement {

	typedef std::list<sf::Sprite>::iterator sprite_t;

	MapElement() : 
		baseSprite(spriteStorage.default_value())
	{ }

	void init(Tile& tile)
	{
		theTile = &tile;
	}

	bool hasSprite() const {
		return baseSprite != spriteStorage.default_value();
	}

	sf::Sprite& sprite() {
		return *baseSprite;
	}

	Tile& tile() { 
		return *theTile;
	}

	const Tile& tile() const {
		return *theTile;
	}

	void createSprite(Tileset& tileset, sf::Vector2i& position) {
		assert(baseSprite == spriteStorage.default_value());
		baseSprite = spriteStorage.request_from_storage();

		baseSprite->setTexture(theTile->myAtlas);
		baseSprite->setTextureRect(theTile->myRect);
		
		baseSprite->setPosition(
			static_cast<float>(position.x*TILE_SIZE_X), 
			static_cast<float>(position.y*TILE_SIZE_Y)
			);
	}

	void deleteSprite() {
		spriteStorage.return_to_storage(baseSprite);
	}

	void draw(OverWorldDisplay& owDisplay) {
		owDisplay.overWorld_texture.draw(*baseSprite);
	}

	~MapElement() {
		spriteStorage.return_to_storage(baseSprite);
	}
	

private:
	Tile* theTile; 
	sprite_t baseSprite; //the graphic element, can point to SpriteStorage.defaultValue() or not

private:
	static StorageProvider<sf::Sprite, CorrectSpriteSizePredicate> spriteStorage;

public:
	static int storage_size_available() {
		return spriteStorage.size_available();
	}

	static int storage_size_in_use() {
		return spriteStorage.size_in_use();
	}
};

class TilePlane {


public:
	TilePlane(Tileset& tileset, sf::Vector2<tile_units> size, sf::Vector2<tile_units> offset, Array2D<int>& table);

	void draw(const sf::View& view, OverWorldDisplay& owDisplay);

	bool collideWith(const sf::FloatRect& rect, sf::Vector2f* CollidingPos) const;
	void getCollidingTiles(const sf::FloatRect& rect, std::set<MapElement*>& result);

	void unloadAllGraphics();
	void updateGraphics(const OverWorldCamera& camera);
	void loadAndWakeUp(const OverWorldCamera& camera);
	Map* myMap;
	bool isGraphicsTotallyUnloaded() const { return graphicsTotallyUnloaded; };

	void dumpLoadedTiles() const ;


public:
	Array2D<MapElement> elements;
	sf::Vector2<tile_units> size;
	sf::Vector2<tile_units> offset;


protected:
	bool graphicsTotallyUnloaded;
	Tileset& tileset;
	void unloadGraphics(const sf::FloatRect& rect);
};
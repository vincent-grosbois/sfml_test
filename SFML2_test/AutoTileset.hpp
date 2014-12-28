#pragma once

#include "Constants.hpp"
#include "Tile.hpp"
#include "AutoTiles.hpp"


class AutoTileset {

public:
	AutoTileset();

	sf::IntRect& getRectangleAt(int i)  { return rectangleList.at(i); }
	int getSize() const { return rectangleList.size(); };
	sf::Image& GetSheet()  { return sheet ; };
	sf::Texture* GetSheet_text() { return &text ; };
	sf::Texture* GetSheet_text(int i, AUTOTILE_TYPE::e type) { return autoTiles.at(i)->get(type) ; };
	Tile* getTile(int i) { return &tiles_list.at(i) ; };

	void load(std::string& str) {
		autoTiles.push_back(new AutoTiles(str));
	};


protected:
	std::vector<sf::IntRect> rectangleList;
	std::vector<Tile> tiles_list;
	std::vector<sf::Texture*> text_list;
	sf::Image sheet;
	sf::Texture text;
	std::vector<AutoTiles*> autoTiles;
	
};


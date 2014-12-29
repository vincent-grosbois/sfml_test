#pragma once


#include "Tile.hpp"

namespace AUTOTILE_TYPE { enum e { FLAT, UL, UP, UR, RIGHT, DR, DOWN, DL, LEFT, OUT_UL, OUT_UR, OUT_DR, OUT_DL, total_size }; };

class TileAnimator;

class Tileset {

public:
	Tileset(const std::string& fileName);
	
	//int size() const { return tiles_list.size(); };
	sf::Texture& getAtlas()  { return sheet ; };
	Tile& getTile(int i) { return tiles_list[i] ; };

	std::vector<TileAnimator*>& get_tile_animators() {
		return tile_animators;
	}

	bool getNeedUpdating() const {
		return needUpdating;
	}

	void setNeedUpdating(bool need) {
		needUpdating = need;
	}

	std::string getKey() const {
		return fileName;
	}

protected:
	std::vector<Tile> tiles_list;
	std::vector<TileAnimator*> tile_animators;
	sf::Texture sheet; 
	std::string fileName;
	std::string sheetName;

	bool needUpdating;
};


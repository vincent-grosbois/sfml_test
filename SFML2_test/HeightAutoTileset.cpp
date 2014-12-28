#include "HeightAutoTileset.hpp"


#include "AutoTiles.hpp"


HeightAutoTileset::HeightAutoTileset(const std::string& filename, int id):
Tileset(filename), id(id)
{
	//init();
}

/*
void HeightAutoTileset::init() {

	tiles_list.clear();
	//memory leak, to fix

	int index = 0;
	sf::Image flat;
	flat.create(TILE_SIZE_X, TILE_SIZE_Y);
	flat.copy(sheet_img, 0, 0, sf::IntRect(2*TILE_SIZE_X, 0 , TILE_SIZE_X, TILE_SIZE_Y) ) ;
	
	
	for(int j = 0; j <13; ++j) {
		tiles_list.push_back(Tile(sheet, sf::Vector2i(32,0), index++));
	}
	
	sf::Image other_dir = flat;

	//UL:
	other_dir.copy(sheet_img, 0, 0, sf::IntRect(0, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y) ) ;
	tiles_list.push_back(Tile(&other_dir, index++, id, true));

	//UP:
	other_dir.copy(sheet_img, 0, 0, sf::IntRect(TILE_SIZE_X, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y) ) ;
	tiles_list.push_back(Tile(&other_dir, index++, id, true));

	//UR:
	other_dir.copy(sheet_img, 0, 0, sf::IntRect(2*TILE_SIZE_X, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y) ) ;
	tiles_list.push_back(Tile(&other_dir, index++, id, true));

	//RIGHT:
	other_dir.copy(sheet_img, 0, 0, sf::IntRect(2*TILE_SIZE_X, 2*TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y) ) ;
	tiles_list.push_back(Tile(&other_dir, index++, id, true));

	//DR:
	other_dir.copy(sheet_img, 0, 0, sf::IntRect(2*TILE_SIZE_X, 3*TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y) ) ;
	tiles_list.push_back(Tile(&other_dir, index++, id, true));

	other_dir.copy(sheet_img, 0, 0, sf::IntRect(TILE_SIZE_X, 3*TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y) ) ;
	tiles_list.push_back(Tile(&other_dir, index++, id, true));

	other_dir.copy(sheet_img, 0, 0, sf::IntRect(0, 3*TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y) ) ;
	tiles_list.push_back(Tile(&other_dir, index++, id, true));

	//LEFT:
	other_dir.copy(sheet_img, 0, 0, sf::IntRect(0, 2*TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y) ) ;
	tiles_list.push_back(Tile(&other_dir, index++, id, true));



	//

	sf::Image flat_UL = flat;
	flat_UL.copy(sheet_img, 0, 0, sf::IntRect(TILE_SIZE_X, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y) ) ;
	tiles_list.push_back(Tile(&flat_UL, index++, id, true));

	sf::Image flat_UR = flat;
	flat_UR.copy(sheet_img, 0, 0, sf::IntRect(TILE_SIZE_X, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y) ) ;
	tiles_list.push_back(Tile(&flat_UR, index++, id, true));

	sf::Image flat_DR = flat;
	flat_DR.copy(sheet_img, 0, 0, sf::IntRect(0, 0, TILE_SIZE_X, TILE_SIZE_Y) ) ;
	tiles_list.push_back(Tile(&flat_DR, index++, id, true));
	
	sf::Image flat_DL = flat;
	flat_DL.copy(sheet_img, 0, 0, sf::IntRect(TILE_SIZE_X, 0, TILE_SIZE_X, TILE_SIZE_Y) ) ;
	tiles_list.push_back(Tile(&flat_DL, index++, id, true));
	



};*/
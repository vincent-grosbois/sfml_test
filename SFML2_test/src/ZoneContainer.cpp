#include "ZoneContainer.h"
#include "Constants.h"
#include "entities/Entity.h"
#include "TilePlane.h"
#include "Tileset.h"
#include "GameResource.h"
#include "Map.h"

Array2D<Map*> gameMapGenerator(const std::string& theFile, Tileset* theTileSet, ZoneContainer& ZC);

ZoneContainer::ZoneContainer(const std::string& dataFile, GameResource& gr):
	data(dataFile),
	tileset(gr.getTileset(data.tilesetDataPath)),
	maps(std::move(gameMapGenerator(data.mapDataPath, &tileset, *this)))
{ }

ZoneContainer::~ZoneContainer()
{
	std::cout << "deleting ZC " << data.dataPath << std::endl;

	for(int i =0; i < maps.size().x; ++i) 
		for(int j =0; j < maps.size().y; ++j)
			delete maps(i,j);
}

std::set<Map*> ZoneContainer::getCollidingMaps(const sf::FloatRect& rect)  {

	tile_units left = static_cast<tile_units>(floor( rect.left / (TILES_PER_MAP_X*TILE_SIZE_X) ) );
	tile_units right = static_cast<tile_units>(ceil( (rect.left + rect.width) / (TILES_PER_MAP_X*TILE_SIZE_X))) ;

	tile_units top = static_cast<tile_units>(floor( rect.top / (TILES_PER_MAP_Y*TILE_SIZE_Y)));
	tile_units bottom = static_cast<tile_units>(ceil( (rect.top + rect.height) / (TILES_PER_MAP_Y*TILE_SIZE_Y) ));

	top = top < 0 ? 0 : top;
	left = left < 0 ? 0 : left;
	bottom = bottom > maps.size().y ? maps.size().y : bottom;
	right = right > maps.size().x ? maps.size().x : right;

	std::set<Map*> list;
	for(int i=left; i < right; i++) {
		for(int j=top; j < bottom; j++) {
			list.insert( maps(i,j) );
		}
	}

	return list;
}

Array2D<Map*> gameMapGenerator(const std::string& theFile, Tileset* theTileSet, ZoneContainer& ZC)
{

	Array2D<Map*> mapList(0,0);

	std::cout << "Generating map" <<std::endl;

	clock_t start = clock();

	sf::Image MAP;
	
	MAP.loadFromFile(theFile);

	int map_size_x  = MAP.getSize().x*3 / TILES_PER_MAP_X;
	int map_size_y  = MAP.getSize().y*3 / TILES_PER_MAP_Y;

	if(map_size_x < 1)
		map_size_x = 1;

	if(map_size_y < 1)
		map_size_y = 1;

	if(map_size_x > 8)
		map_size_x  = 8;
	if(map_size_y > 8)
		map_size_y  = 8;
	

	mapList = std::move(Array2D<Map*>(map_size_x,map_size_y));

	Array2D<int> tableau2D(MAP.getSize().x, MAP.getSize().y);
	sf::Color value;
	int* v2;
	for(int i=0; i<map_size_x*TILES_PER_MAP_X/3; ++i) {
		for(int j=0; j<map_size_y*TILES_PER_MAP_Y/3; ++j)  {

			value = MAP.getPixel(i,j);

			v2 = &tableau2D(i,j);

			if(value == sf::Color::Blue || value == sf::Color(123,217,234) || value == sf::Color(12,14,50) || value == sf::Color(28,33,104))
				*v2 = 0;
			else if(value == sf::Color::Yellow)
				*v2 = 1;
			else if(value == sf::Color::Green)
				*v2 = 2;
			else if(value == sf::Color(193,210,6))
				*v2 = 3;
			else if(value == sf::Color(94,57, 38))
				*v2 = 4;
			else if(value == sf::Color(128,255,0))
				*v2 = 5;
			else if(value == sf::Color(185,122, 87))
				*v2 = 6;
			else if(value == sf::Color::White)
				*v2 = 7;
			else
				*v2 = 8;
		}
	}

	int id=0;

	for(int i=0; i<map_size_x; ++i) {

		for(int j=0; j<map_size_y; ++j)  {

			TilePlane* TP  = new TilePlane(*theTileSet, 
				sf::Vector2<tile_units>(TILES_PER_MAP_X, TILES_PER_MAP_Y), 
				sf::Vector2<tile_units>(TILES_PER_MAP_X*i, TILES_PER_MAP_Y*j), 
				tableau2D);

			TilePlane* water  = NULL;

			if(ZC.getData().isOutside) {
				water = new TilePlane(*theTileSet, 
					sf::Vector2<tile_units>(TILES_PER_MAP_X, TILES_PER_MAP_Y), 
					sf::Vector2<tile_units>(TILES_PER_MAP_X*i, TILES_PER_MAP_Y*j), 
					tableau2D, true);
			}

			mapList(i,j) = new Map(ZC,
				water,
				TP, 
				NULL, 
				NULL, 
				sf::Vector2<tile_units>(TILES_PER_MAP_X*i, TILES_PER_MAP_Y*j), 
				id);

			++id;
			//std::cout << "Generated Map " << id << '\n';
		}
		

	}
	clock_t end = clock();
	std::cout << "Map generated, in "<< (float)(1000*(end-start))/CLOCKS_PER_SEC << "ms." <<std::endl;

	return mapList;
}

void ZoneContainer::dumpLoadedTiles() const {
	for(int i =0; i < maps.size().x; ++i) 
		for(int j =0; j < maps.size().y; ++j)
			maps(i,j)->dumpLoadedTiles();
}

const ZoneContainerData& ZoneContainer::getData() const {
	return data;
}

Tileset& ZoneContainer::getTileset() {
	return tileset;
}

void ZoneContainer::deleteElements() {

	for(auto& ent : to_delete) {
		delete ent;
	}
	to_delete.clear();
}

void ZoneContainer::addForDeletion(Entity* e) {
	to_delete.insert(e);
}
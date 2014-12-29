#include "Map.hpp"
#include "TilePlane.hpp"


const int map_elements_cell_start = 1000;
const int map_elements_cell_max_before_delete = 1500;
const int map_elements_additional_batches_size = 10;

StorageProvider<std::set<Entity*>, NoElementPredicate> EntitySet::entitySetStorage = 
	StorageProvider<std::set<Entity*>, NoElementPredicate>(map_elements_cell_start, 
	map_elements_cell_max_before_delete,
	map_elements_additional_batches_size,
	NoElementPredicate());


Map::Map(ZoneContainer& ZC, TilePlane* layer0, TilePlane* layer1, TilePlane* layer2, sf::Vector2<tile_units> position, int id):
	layer0(layer0),
	layer1(layer1),
	layer2(layer2),
	position(position),
	id(id),
	size(layer0->size),
	isLoaded(false),
	entities_grid(size),
	offset(layer0->offset),
	myZC(&ZC)
{
	if(layer0)
		layer0->myMap = this;
	if(layer1)
		layer1->myMap = this;
	if(layer2)
		layer2->myMap = this;
}


void Map::drawLayer(const sf::View& view, OverWorldDisplay& owDisplay, int layer) {

	if(layer == 0 && layer0!=NULL)
		layer0->draw(view, owDisplay);

	else if(layer == 1 && layer1!=NULL)
		layer1->draw(view, owDisplay);

	else if(layer == 2 && layer2!=NULL)
		layer2->draw(view, owDisplay);
}

Map::~Map()
{
	for(std::set<Entity*>::iterator it = entities_on_map.begin(); it != entities_on_map.end(); ) 
		delete (*it++);

	delete layer0;
	delete layer1;	
	delete layer2;
}

void Map::printDebug() const {
	std::cout << "Map @" << id << " X:" << position.x << " Y:"  << position.y << "\n"; 
}

void Map::updateGraphics(const OverWorldCamera& camera, bool checkAnimatedTilesUpdate) {

	if(layer0)
		layer0->updateGraphics(camera, checkAnimatedTilesUpdate);

	if(layer1)
		layer1->updateGraphics(camera, checkAnimatedTilesUpdate);

	if(layer2)
		layer2->updateGraphics(camera, checkAnimatedTilesUpdate);

}

void Map::loadTilesFromNothing(const OverWorldCamera& camera) {

	isLoaded = true;

	if(layer0)
		layer0->loadAndWakeUp(camera);

	if(layer1)
		layer1->loadAndWakeUp(camera);

	if(layer2)
		layer2->loadAndWakeUp(camera);

	for(std::set<Entity*>::iterator it = entities_on_map.begin(); it != entities_on_map.end(); ++it) 
		(*it)->tryToWakeUp();
}



void Map::unloadAll() {

	if(layer0)
		layer0->unloadAllGraphics();

	if(layer1)
		layer1->unloadAllGraphics();

	if(layer2)
		layer2->unloadAllGraphics();

	for(std::set<Entity*>::iterator it = entities_on_map.begin(); it != entities_on_map.end(); ++it)
		(*it)->tryToSleep();

	isLoaded = false;
}

void Map::getCollidingTiles(const sf::FloatRect& rect, std::set<EntitySet*>& result)  {

	tile_units left = static_cast<tile_units>(floor( rect.left / TILE_SIZE_X )) - offset.x;
	tile_units right = static_cast<tile_units>(ceil( (rect.left + rect.width) / TILE_SIZE_X ))  - offset.x;

	tile_units top = static_cast<tile_units>(floor( rect.top / TILE_SIZE_Y ))  - offset.y;
	tile_units bottom = static_cast<tile_units>(ceil(  (rect.top + rect.height) / TILE_SIZE_Y )) - offset.y;

	if(top<0)
		top = 0;
	if(left<0)
		left = 0;
	if(bottom > size.y)
		bottom = size.y;
	if(right > size.x)
		right = size.x;


	for(int i=left; i < right; i++) {
		for(int j=top; j < bottom; j++) {
			result.insert(&entities_grid(i,j));
		}
	}

}

void Map::dumpLoadedTiles() const  {
	if(layer0)
		layer0->dumpLoadedTiles();
}
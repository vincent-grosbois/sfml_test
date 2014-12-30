#include <set>
#include <algorithm>
#include <iostream>

#include "TilePlane.hpp"
#include "Entity.hpp"
#include "Tileset.hpp"
#include "Map.hpp"
#include "TileAnimator.h"
#include "OverWorldScene.hpp"


TilePlane::TilePlane(Tileset& tileset, sf::Vector2<tile_units> size, sf::Vector2<tile_units> offset, Array2D<int>& table):
elements(size.x, size.y),
	size(size),
	offset(offset),
	tileset(tileset),
	graphicsTotallyUnloaded(true),
	myMap(NULL),
	tileBlocks(TILE_BLOCK_PER_MAP_X,TILE_BLOCK_PER_MAP_Y)
{

	for(int i = 0; i < TILE_BLOCK_PER_MAP_X; ++i) {
		for(int j = 0; j < TILE_BLOCK_PER_MAP_Y; ++j) {
			tileBlocks(i,j).setPrimitiveType(sf::PrimitiveType::Quads);
			tileBlocks(i,j).resize(size.x*size.y*4 / (TILE_BLOCK_PER_MAP_X*TILE_BLOCK_PER_MAP_Y));
		}
	}

	int id;
	for(int i=0; i < size.x; ++i) {
		for(int j=0; j < size.y; ++j) {

			id = table((i+offset.x)/3 , (j+offset.y)/3);

			sf::Vertex* quad = getQuadVertexFromTileIndex(i,j);

			int offset_x = offset.x*TILE_SIZE_X;
			int offset_y = offset.y*TILE_SIZE_Y;

			quad[0].position = 
				sf::Vector2f(static_cast<float>(i*TILE_SIZE_X + offset_x),		static_cast<float>(j*TILE_SIZE_Y + offset_y));
			quad[1].position = 
				sf::Vector2f(static_cast<float>((i+1)*TILE_SIZE_X + offset_x),	static_cast<float>(j*TILE_SIZE_Y + offset_y));
			quad[2].position = 
				sf::Vector2f(static_cast<float>((i+1)*TILE_SIZE_X + offset_x),	static_cast<float>((j+1)*TILE_SIZE_Y + offset_y));
			quad[3].position = 
				sf::Vector2f(static_cast<float>(i*TILE_SIZE_X + offset_x),		static_cast<float>((j+1)*TILE_SIZE_Y + offset_y));

			Tile& tile = tileset.getTile(id);

			setQuadTextureToFrame(quad, tile.myRect);

			elements(i,j).init(tile);
		}
	}

}

void TilePlane::setQuadTextureToFrame(sf::Vertex* quad, const sf::IntRect& rect) {
	quad[0].texCoords = sf::Vector2f(static_cast<float>(rect.left),					static_cast<float>(rect.top));
	quad[1].texCoords = sf::Vector2f(static_cast<float>(rect.left + rect.width),	static_cast<float>(rect.top));
	quad[2].texCoords = sf::Vector2f(static_cast<float>(rect.left + rect.width),	static_cast<float>(rect.top + rect.height));
	quad[3].texCoords = sf::Vector2f(static_cast<float>(rect.left),					static_cast<float>(rect.top + rect.height));
}

sf::Vertex* TilePlane::getQuadVertexFromTileIndex(int i, int j) {

	int array_num_x = i/TILES_PER_TILE_BLOCK_X;
	int array_num_y = j/TILES_PER_TILE_BLOCK_Y;

	int array_index_x = i - array_num_x*TILES_PER_TILE_BLOCK_X;
	int array_index_y = j - array_num_y*TILES_PER_TILE_BLOCK_Y;

	return
		&tileBlocks(array_num_x,array_num_y)
		[(array_index_x + array_index_y * size.x / TILE_BLOCK_PER_MAP_X) * 4];

}

void TilePlane::updateGraphics(const OverWorldCamera& camera, bool checkAnimatedTilesUpdate) {

	auto rect = camera.getViewRect();

	if(checkAnimatedTilesUpdate) {
		tile_units left = static_cast<tile_units>(floor( rect.left / TILE_SIZE_X )) - offset.x;
		tile_units right = static_cast<tile_units>(ceil( (rect.left + rect.width) / TILE_SIZE_X ))  - offset.x;

		tile_units top = static_cast<tile_units>(floor( rect.top / TILE_SIZE_Y ))  - offset.y;
		tile_units bottom = static_cast<tile_units>(ceil( (rect.top + rect.height) / TILE_SIZE_Y )) - offset.y;

		if(top<0)
			top = 0;
		if(left<0)
			left = 0;
		if(bottom > size.y)
			bottom = size.y;
		if(right > size.x)
			right = size.x;

		for(int i=left; i < right; ++i) {
			for(int j=top; j < bottom; ++j) {
				if (elements(i,j).tile().isAnimated() && elements(i,j).tile().myAnimator->tile_changed_on_this_frame){
					setQuadTextureToFrame(getQuadVertexFromTileIndex(i,j), elements(i,j).tile().myRect);
				}
			}
		}
	}

	if(camera.movedSinceLastFrame()) {
		unloadGraphics(rect);
	}
}

void TilePlane::unloadAllGraphics() {
	graphicsTotallyUnloaded = true;
}

void TilePlane::loadAndWakeUp(const OverWorldCamera& camera) {
	updateGraphics(camera, true);
	graphicsTotallyUnloaded = false;
}


void TilePlane::unloadGraphics(const sf::FloatRect& rect) {
}


void TilePlane::draw(const sf::View& view, OverWorldDisplay& owDisplay) {

	sf::Rect<float> viewrec(view.getCenter() -sf::Vector2f(view.getSize().x/2, view.getSize().y/2) ,view.getSize());

	int left = static_cast<int>(floor( viewrec.left / TILE_BLOCK_SIZE_X )) ;
	int right = static_cast<int>(floor( (viewrec.left +viewrec.width) / TILE_BLOCK_SIZE_X )) ;

	int top = static_cast<int>(floor( viewrec.top / TILE_BLOCK_SIZE_Y ));
	int bottom = static_cast<int>(floor( (viewrec.top + viewrec.height) / TILE_BLOCK_SIZE_Y ));

	left -= offset.x / TILES_PER_TILE_BLOCK_X;
	right -= offset.x / TILES_PER_TILE_BLOCK_X;
	top -= offset.y / TILES_PER_TILE_BLOCK_Y;
	bottom -= offset.y / TILES_PER_TILE_BLOCK_Y;

	top = top < 0 ? 0 : top;
	left = left < 0 ? 0 : left;
	bottom = bottom > tileBlocks.size().y-1 ? tileBlocks.size().y-1 : bottom;
	right = right > tileBlocks.size().x-1 ? tileBlocks.size().x-1 : right;

	sf::RenderStates state;
	state.texture = &tileset.getAtlas();

	for(int i = left; i <= right; ++i) {
		for(int j = top; j <= bottom; ++j) {
			owDisplay.overWorld_texture.draw(tileBlocks(i,j), state);
		}
	}

}


bool TilePlane::collideWith(const sf::FloatRect& rect, sf::Vector2f* collidingPos) const {

	tile_units left = static_cast<tile_units>(floor( rect.left / TILE_SIZE_X )) - offset.x;
	tile_units right = static_cast<tile_units>(ceil( (rect.left + rect.width) / TILE_SIZE_X ))  - offset.x;

	tile_units top = static_cast<tile_units>(floor( rect.top / TILE_SIZE_Y ))  - offset.y;
	tile_units bottom = static_cast<tile_units>(ceil( (rect.top + rect.height) / TILE_SIZE_Y )) - offset.y;

	if(top<0) top = 0;
	if(left<0) left = 0;
	if(bottom > size.y) bottom = size.y;
	if(right > size.x) right = size.x;

	for(int i=left; i < right; i++) {
		for(int j=top; j < bottom; j++) {

			if(elements(i,j).tile().tilePassability == TILE_PASSABILITY::NOTPASSABLE) {
				collidingPos->x = static_cast<float>(TILE_SIZE_X*(i + offset.x));
				collidingPos->y = static_cast<float>(TILE_SIZE_Y*(j + offset.y));
				return true;
			}
		}

	}

	return false;
}

void TilePlane::getCollidingTiles(const sf::FloatRect& rect, std::set<MapElement*>& result)  {

	tile_units left = static_cast<tile_units>(floor( rect.left / TILE_SIZE_X )) - offset.x;
	tile_units right = static_cast<tile_units>(ceil( (rect.left + rect.width) / TILE_SIZE_X ))  - offset.x;

	tile_units top = static_cast<tile_units>(floor( rect.top / TILE_SIZE_Y ))  - offset.y;
	tile_units bottom = static_cast<tile_units>(ceil(  (rect.top + rect.height) / TILE_SIZE_Y )) - offset.y;

	if(top<0) top = 0;
	if(left<0) left = 0;
	if(bottom > size.y) bottom = size.y;
	if(right > size.x) right = size.x;


	for(int i=left; i < right; i++) {
		for(int j=top; j < bottom; j++) {
			result.insert(&elements(i,j));
		}
	}

}

void TilePlane::dumpLoadedTiles() const {

	/*bool nosprite = true;
	for(int i=0; i < size.x; i++) {
		for(int j=0; j < size.y; j++) {
			if(elements(i,j).hasSprite()) {
				nosprite = false;
				break;
			}
		}
	}

	if(nosprite)
		return;

	std::cout << "\nFor tileplane "<<this<<": \n";
	for(int i=0; i < size.x; i++) {
		bool nosprite = true;
		for(int j=0; j < size.y; j++) {
			if(elements(i,j).hasSprite()) {
				if(nosprite)
				{
					std::cout << '\n';
					nosprite = false;
				}
				std::cout << "("<<i<<","<<j<<") ";
			}
		}
	}*/

}


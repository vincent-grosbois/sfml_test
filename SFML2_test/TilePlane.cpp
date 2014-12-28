#include <set>
#include <algorithm>
#include <iostream>
#include <math.h>

#include "TilePlane.hpp"
#include "Entity.hpp"
#include "Tileset.hpp"
#include "Map.hpp"
#include "TileAnimator.h"



const int map_elements_sprite_start = 60*60;
const int map_elements_sprite_max_before_delete = map_elements_sprite_start*2;
const int map_elements_sprite_additional_batches_size = 200;

StorageProvider<sf::Sprite, CorrectSpriteSizePredicate> MapElement::spriteStorage = 
	StorageProvider<sf::Sprite, CorrectSpriteSizePredicate>(map_elements_sprite_start, 
	map_elements_sprite_max_before_delete,
	map_elements_sprite_additional_batches_size,
	CorrectSpriteSizePredicate());


AUTOTILE_TYPE::e getType(Array2D<int>&table, int i, int j) {

	

if( i == 0 ||i/3>=(table.size().x-1) ||j==0 || j/3>=(table.size().y-1)) {
	return AUTOTILE_TYPE::FLAT;
}

	int current = table((int)(i/3) , (int)(j/3));
	int block_x = i/3;
	int block_y = j/3;
	int pos_x = i%3;
	int pos_y = j%3;

	if(pos_x == 1 && pos_y == 1)
		return AUTOTILE_TYPE::FLAT;

	if(pos_x == 1 && pos_y == 0) {
		if(table(block_x , block_y-1 ) < current)
			return AUTOTILE_TYPE::UP;
		else return AUTOTILE_TYPE::FLAT;
	}

	if(pos_x == 2 && pos_y == 1) {
		if(table(block_x+1 , block_y) < current)
			return AUTOTILE_TYPE::RIGHT;
		else return AUTOTILE_TYPE::FLAT;
	}

	if(pos_x == 0 && pos_y == 1) {
		if(table( block_x-1, block_y )< current)
			return AUTOTILE_TYPE::LEFT;
		else return AUTOTILE_TYPE::FLAT;
	}

	if(pos_x == 1 && pos_y == 2) {
		if(table(block_x,block_y+1 ) < current)
			return AUTOTILE_TYPE::DOWN;
		else return AUTOTILE_TYPE::FLAT;
	}

	if(pos_x == 0 && pos_y == 0) {
		if(table(block_x , block_y-1) < current && table(block_x-1 , block_y) < current)
			return AUTOTILE_TYPE::UL;
		else if(table(block_x , block_y-1) < current)
			return AUTOTILE_TYPE::UP;
		else if(table(block_x-1 , block_y) < current)
			return AUTOTILE_TYPE::LEFT;
		else if (table(block_x-1 , block_y-1) < current) 
			return AUTOTILE_TYPE::OUT_UL;
		else return AUTOTILE_TYPE::FLAT;
	}

	if(pos_x == 2 && pos_y == 0) {
		if(table(block_x , block_y-1) < current && table(block_x+1 , block_y) < current)
			return AUTOTILE_TYPE::UR;
		else if(table(block_x , block_y-1) < current)
			return AUTOTILE_TYPE::UP;
		else if(table(block_x+1 , block_y) < current)
			return AUTOTILE_TYPE::RIGHT;
		else if (table(block_x+1 , block_y-1) < current) 
			return AUTOTILE_TYPE::OUT_UR;
		else return AUTOTILE_TYPE::FLAT;
	}

	if(pos_x == 2 && pos_y == 2) {
		if(table(block_x , block_y+1) < current && table(block_x+1 , block_y) < current)
			return AUTOTILE_TYPE::DR;
		else if(table(block_x , block_y+1) < current)
			return AUTOTILE_TYPE::DOWN;
		else if(table(block_x+1 , block_y) < current)
			return AUTOTILE_TYPE::RIGHT;
		else if (table(block_x+1 , block_y+1) < current) 
			return AUTOTILE_TYPE::OUT_DR;
		else return AUTOTILE_TYPE::FLAT;
	}

	if(pos_x == 0 && pos_y == 2) {
		if(table(block_x , block_y+1) < current && table(block_x-1 , block_y) < current)
			return AUTOTILE_TYPE::DL;
		else if(table(block_x , block_y+1) < current)
			return AUTOTILE_TYPE::DOWN;
		else if(table(block_x-1 , block_y) < current)
			return AUTOTILE_TYPE::LEFT;
		else if (table(block_x-1 , block_y+1) < current) 
			return AUTOTILE_TYPE::OUT_DL;
		else return AUTOTILE_TYPE::FLAT;
	}


	return AUTOTILE_TYPE::FLAT;
};


TilePlane::TilePlane(Tileset& tileset, sf::Vector2<tile_units> size, sf::Vector2<tile_units> offset, Array2D<int>& table):
elements(size.x, size.y),
	size(size),
	offset(offset),
	tileset(tileset),
	graphicsTotallyUnloaded(true),
	myMap(NULL)
{
	int id;
	for(int i=0; i < size.x; ++i) {
		for(int j=0; j < size.y; ++j) {

			id = table((i+offset.x)/3 , (j+offset.y)/3);

			elements(i,j).init(tileset.getTile(id));
		}
	}

}



void TilePlane::updateGraphics(const OverWorldCamera& camera) {

	auto rect = camera.getViewRect();

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

			if(camera.movedSinceLastFrame() && !elements(i,j).hasSprite()) {
				elements(i,j).createSprite(tileset, sf::Vector2i(i + offset.x, j + offset.y));	
			}
			if (elements(i,j).tile().isAnimated() && elements(i,j).tile().myAnimator->tile_changed_on_this_frame){
				elements(i,j).sprite().setTextureRect(elements(i,j).tile().myRect);
			}
		}
	}

	if(camera.movedSinceLastFrame()) {
		unloadGraphics(rect);
	}
}

void TilePlane::unloadAllGraphics() {

	for(int j=0; j < size.x; ++j) {
		for(int i=0; i < size.y; ++i) {
			elements(j,i).deleteSprite();
		}
	}

	graphicsTotallyUnloaded = true;
}

void TilePlane::loadAndWakeUp(const OverWorldCamera& camera) {
	updateGraphics(camera);
	graphicsTotallyUnloaded = false;
}


void TilePlane::unloadGraphics(const sf::FloatRect& rect) {

	tile_units left = static_cast<tile_units>(floor( rect.left / TILE_SIZE_X )) - offset.x;
	tile_units right = static_cast<tile_units>(ceil( (rect.left + rect.width) / TILE_SIZE_X ))  - offset.x;

	tile_units top = static_cast<tile_units>(floor( rect.top / TILE_SIZE_Y ))  - offset.y;
	tile_units bottom = static_cast<tile_units>(ceil( (rect.top + rect.height)  / TILE_SIZE_Y )) - offset.y;

	if(top<0) 
		top = 0;
	else if(top > size.y) 
		top = size.y;

	if(left<0) 
		left = 0;
	else if(left > size.x) 
		left = size.x;

	if(bottom > size.y) 
		bottom = size.y;
	else if (bottom < 0) 
		bottom = 0;

	if(right > size.x) 
		right = size.x;
	else if(right < 0) 
		right = 0;

	int j=0;
	int i=0;

	for(; j < size.y; ++j) {	
		for(i = 0; i < left; ++i)
			elements(i,j).deleteSprite();

		for(i=right; i < size.x; ++i)
			elements(i,j).deleteSprite();
	}

	for(i=left; i < right; ++i) {
		for(j=0; j < top; ++j)
			elements(i,j).deleteSprite();

		for(j=bottom; j < size.y; ++j)
			elements(i,j).deleteSprite();
	}
}



void TilePlane::draw(const sf::View& view, OverWorldDisplay& owDisplay) {

	sf::Rect<float> viewrec(view.getCenter() -sf::Vector2f(view.getSize().x/2, view.getSize().y/2) ,view.getSize());

	tile_units left = static_cast<tile_units>(floor( viewrec.left / TILE_SIZE_X )) ;
	tile_units right = static_cast<tile_units>(ceil( (viewrec.left +viewrec.width) / TILE_SIZE_X )) ;

	tile_units top = static_cast<tile_units>(floor( viewrec.top / TILE_SIZE_Y ));
	tile_units bottom = static_cast<tile_units>(ceil( (viewrec.top + viewrec.height) / TILE_SIZE_Y ));

	left -= offset.x;
	right -= offset.x;
	top -= offset.y;
	bottom -= offset.y;

	top = top < 0 ? 0 : top;
	left = left < 0 ? 0 : left;
	bottom = bottom > size.y ? size.y : bottom;
	right = right > size.x ? size.x : right;

	for(int i=left; i < right; i++) {
		for(int j=top; j < bottom; j++) {
			
#ifdef _DEBUG
			if(myMap->entities_grid(i,j).has_entities()) {
					elements(i,j).draw(owDisplay) ;
					elements(i,j).sprite().setColor(sf::Color(0, 255, 255, 255));
				}
				else {
					elements(i,j).sprite().setColor(sf::Color(255, 255, 255, 255));
					elements(i,j).draw(owDisplay) ;

				}
#else
			
				elements(i,j).draw(owDisplay) ;
#endif

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

	bool nosprite = true;
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
	}

}


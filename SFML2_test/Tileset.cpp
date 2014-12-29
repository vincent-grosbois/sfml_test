#include <SFML/Graphics.hpp>

#include <iostream>
#include <sstream>
#include <fstream> 
#include <cassert>

#include "Tileset.hpp"
#include "TileAnimator.h"
#include "utils/StringUtils.h"

using namespace utils;

Tile::~Tile() {
	delete myAnimator;
}

std::vector<sf::Vector2i> extract_pairs(const std::vector<std::string>& in) {
	std::vector<sf::Vector2i> result;
	for(auto& str : in) {
		result.push_back(readFromStr<sf::Vector2i>(str));
	}
	return result;
}

Tileset::Tileset(const std::string& fileName):
fileName(fileName),
needUpdating(false)
{
	auto basePath = utils::getDirectory(fileName);

	std::vector<bool> collide_vector;
	std::vector<std::vector<sf::Vector2i>> frames_vector;

	std::string line;
	std::ifstream myfile (fileName);
	bool firstLine = true;
	if (myfile.is_open())
	{
		while ( std::getline (myfile,line) )
		{
			if(firstLine) {
				firstLine = false;
				sheetName = basePath + line;
				if (!sheet.loadFromFile(sheetName)) 
					std::cout << "could not load " << sheetName << std::endl;
				else
					std::cout << "Tileset sheet " << sheetName <<" loaded\n";

				sheet.setSmooth(false);
				continue;
			}
			auto res = split(line, ":", false);
			assert(res.size() == 1 || res.size() == 2);

			bool collide = false;
			if(res.size() == 2) {
				collide = res[1] == "true";
			}
			auto res2 = split(res[0], "->", false);
			auto res3 = extract_pairs(res2);

			collide_vector.push_back(collide);
			frames_vector.push_back(res3);
		}
		myfile.close();
	}
	else std::cout << "Unable to open file " << fileName; 

	tiles_list.reserve(collide_vector.size());


	int index = 0;
	for(int j = 0; j < static_cast<int>(collide_vector.size()); ++j) {
		tiles_list.emplace_back(
			Tile(sheet, 
			sf::Vector2i(TILE_SIZE_X*frames_vector[j][0].x, TILE_SIZE_Y*frames_vector[j][0].y), 
			index++,
			collide_vector[j])
			);


		if(frames_vector[j].size() > 1) {
			tiles_list[j].myAnimator = new TileAnimator(tiles_list[j], frames_vector[j], *this);
			tile_animators.push_back(tiles_list[j].myAnimator);
		}
	}
		
}


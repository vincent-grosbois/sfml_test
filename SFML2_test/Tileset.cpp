#include <SFML/Graphics.hpp>

#include <iostream>
#include <sstream>
#include <fstream> 
#include <cassert>

#include "Tileset.hpp"
#include "TileAnimator.h"

Tile::~Tile() {
	delete myAnimator;
}

std::vector<std::string> split(const std::string& s, const std::string& delim, const bool keep_empty = true);
std::vector<std::pair<int,int>> extract_pairs(const std::vector<std::string>& in);

Tileset::Tileset(const std::string& textureFileName, const std::string& infoFileName):
	fileName(textureFileName)
{

	if (!sheet.loadFromFile(fileName)) 
		std::cout << "could not load " << fileName << std::endl;
	else
		std::cout << "Tileset sheet " << fileName <<" loaded\n";

	sheet.setSmooth(false);

	std::vector<bool> collide_vector;
	std::vector<std::vector<std::pair<int,int>>> frames_vector;

	std::string line;
	std::ifstream myfile (infoFileName);
	if (myfile.is_open())
	{
		while ( std::getline (myfile,line) )
		{
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
	else std::cout << "Unable to open file " << infoFileName; 

	tiles_list.reserve(collide_vector.size());


	int index = 0;
	for(int j = 0; j < static_cast<int>(collide_vector.size()); ++j) {
		tiles_list.emplace_back(
			Tile(sheet, 
			sf::Vector2i(TILE_SIZE_X*frames_vector[j][0].first, TILE_SIZE_Y*frames_vector[j][0].second), 
			index++,
			collide_vector[j])
			);


		if(frames_vector[j].size() > 1) {
			tiles_list[j].myAnimator = new TileAnimator(tiles_list[j], frames_vector[j]);
			tile_animators.push_back(tiles_list[j].myAnimator);
		}
	}
		
}


std::vector<std::string> split(const std::string& s, const std::string& delim, const bool keep_empty) {
    std::vector<std::string> result;
    if (delim.empty()) {
        result.push_back(s);
        return result;
    }
   std::string::const_iterator substart = s.begin(), subend;
    while (true) {
        subend = search(substart, s.end(), delim.begin(), delim.end());
        std::string temp(substart, subend);
        if (keep_empty || !temp.empty()) {
            result.push_back(temp);
        }
        if (subend == s.end()) {
            break;
        }
        substart = subend + delim.size();
    }
    return result;
}

std::vector<std::pair<int,int>> extract_pairs(const std::vector<std::string>& in) {

	std::vector<std::pair<int,int>> result;

	for(auto str : in) {
		auto res = split(str, ",", false);
		assert(res.size() == 2);
		assert(res[0].at(0) == '(');
		assert(res[1].at(res[1].size()-1) == ')');

		int x_pos ;
		std::istringstream ( res[0].substr(1) ) >> x_pos ;

		int y_pos;
		std::istringstream ( res[1].substr(0, res[1].size()-1) ) >> y_pos ;

		result.push_back(std::make_pair(x_pos,y_pos));
	}

	return result;
}

 

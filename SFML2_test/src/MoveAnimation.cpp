#include "MoveAnimation.h"

const int frames = 9;
MoveAnimation::MoveAnimation(const std::string& filename):
animations_rect(4, frames),
fileName(fileName)
{
	atlas.loadFromFile(filename);
	int sizeY = atlas.getSize().y / 4;
	int sizeX = atlas.getSize().x / frames ;

	int y = 0;

	for(int dir = 0; dir < 4; ++dir ){
		for(int i = 0; i<frames; ++i) {

			int dir2 = dir;
			if(dir == 0) {
				dir2 = 3;
			}
			else if(dir == 2) {
				dir2 = 0;
			}
			else if(dir == 3) {
				dir2 = 2;
			}

			animations_rect(dir2,i) = sf::IntRect(i*sizeX, y , sizeX, sizeY)  ;
		}
		y+=sizeY;
	}
}



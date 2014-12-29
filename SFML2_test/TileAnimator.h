#pragma once
#include <vector>
#include <numeric>

#include "Tile.hpp"
#include "GameTicks.hpp"

class TileAnimator {
public:
	std::vector<sf::IntRect> frames;
	std::vector<int> ticks_per_frame;

	int current_index;
	int current_tick;
	int total_ticks;

	TICKS::e base_tick;

	Tile& tile_to_animate;
	Tileset& tileset;

	bool tile_changed_on_this_frame;

	TileAnimator(Tile& tile, const std::vector<sf::Vector2i>& in, Tileset& tileset) :
		tileset(tileset),
		tile_to_animate(tile) {
			tile_to_animate.myAnimator = this;

			for(auto vect : in) {
				frames.push_back(
					sf::IntRect(
					vect.x*TILE_SIZE_X,
					vect.y*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y)
					);

				ticks_per_frame.push_back(4);
			}

			base_tick = TICKS::_100MS;
			//

			current_index = 0;
			current_tick = 0;
			total_ticks = std::accumulate(ticks_per_frame.begin(), ticks_per_frame.end(), 0);
	}

	void process_frame(int ticks) {
		//will update current_index and set tile_changed_on_this_frame to 1 

		int new_ticks = ticks;

		current_tick += new_ticks;

		int old_index = current_index;

		while (current_tick  > ticks_per_frame[current_index]) {
			current_tick -= ticks_per_frame[current_index];
			current_index = (current_index + 1) % frames.size();
		}

		tile_changed_on_this_frame = old_index != current_index;

		if( tile_changed_on_this_frame ) {
			tile_to_animate.myRect = frames[current_index];
			tileset.setNeedUpdating(true);
		}
	}

};
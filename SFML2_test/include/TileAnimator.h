#pragma once
#include <vector>
#include <numeric>

#include "Tile.h"
#include "CallBackSystem.h"

class TileAnimator : public CallBackReceiver {
public:
	std::vector<sf::IntRect> frames;
	int frame_duration;

	int current_index;
	int current_tick;
	int total_duration;

	Tile& tile_to_animate;
	Tileset& tileset;

	bool tile_changed_on_this_frame;

	TileAnimator(Tile& tile, const std::vector<sf::Vector2i>& in, Tileset& tileset) :
		tileset(tileset),
		tile_to_animate(tile) {
			tile_to_animate.myAnimator = this;
			frame_duration = 400;
			for(auto vect : in) {
				frames.push_back(
					sf::IntRect(
					vect.x*constants::TILE_SIZE_X,
					vect.y*constants::TILE_SIZE_Y,
					constants::TILE_SIZE_X,constants::TILE_SIZE_Y)
					);

			}

			//

			current_index = 0;
			current_tick = 0;
			total_duration = frame_duration*frames.size();
	}

	void process_frame(frame_time_t ideal_frame, frame_time_t actual_frame) {
		//will update current_index and set tile_changed_on_this_frame to 1 

		//std::cout << "exec at time " << actual_frame << " instead of " << ideal_frame << std::endl;

		int old_index = current_index;
		
		int delay = (actual_frame - ideal_frame) / frame_duration ;

		current_index = (current_index + 1 + delay) % frames.size();

		tile_changed_on_this_frame = old_index != current_index;

		if( tile_changed_on_this_frame ) {
			tile_to_animate.myRect = frames[current_index];
			tileset.setNeedUpdating(true);
		}
	}

};
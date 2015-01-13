#pragma once

namespace constants {
	const int TILE_SIZE_X = 32;
	const int TILE_SIZE_Y = 32;

	const int TILES_PER_MAP_X = 60;
	const int TILES_PER_MAP_Y = 60;

	const int TILE_BLOCK_SIZE_X = 32*30;
	const int TILE_BLOCK_SIZE_Y = 32*30;

	const int TILES_PER_TILE_BLOCK_X = TILE_BLOCK_SIZE_X /TILE_SIZE_X;
	const int TILES_PER_TILE_BLOCK_Y = TILE_BLOCK_SIZE_Y /TILE_SIZE_Y;

	const int TILE_BLOCK_PER_MAP_X = TILE_SIZE_X*TILES_PER_MAP_X / TILE_BLOCK_SIZE_X;
	const int TILE_BLOCK_PER_MAP_Y = TILE_SIZE_Y*TILES_PER_MAP_Y / TILE_BLOCK_SIZE_Y;

	static_assert(TILE_BLOCK_SIZE_X % TILE_SIZE_X == 0, "");
	static_assert(TILE_BLOCK_SIZE_Y % TILE_SIZE_Y == 0, "");

	const int VISIBILITY_BLOCK_SIZE_X = 320;
	const int VISIBILITY_BLOCK_SIZE_Y = 320;

	const int VISIBILITY_BLOCK_PER_MAP_X = TILE_SIZE_X*TILES_PER_MAP_X /VISIBILITY_BLOCK_SIZE_X;
	const int VISIBILITY_BLOCK_PER_MAP_Y = TILE_SIZE_Y*TILES_PER_MAP_Y /VISIBILITY_BLOCK_SIZE_Y;

	static_assert(TILE_SIZE_X*TILES_PER_MAP_X % VISIBILITY_BLOCK_SIZE_X == 0, "");
	static_assert(TILE_SIZE_Y*TILES_PER_MAP_Y % VISIBILITY_BLOCK_SIZE_Y == 0, "");
}

typedef int tile_units;

typedef float radian_t;

namespace DIRECTION { 
	enum e { DOWN, LEFT, RIGHT, UP}; 
};

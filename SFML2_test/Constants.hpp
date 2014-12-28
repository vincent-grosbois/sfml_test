#pragma once

const int TILE_SIZE_X = 32;
const int TILE_SIZE_Y = 32;

const int TILES_PER_MAP_X = 60;
const int TILES_PER_MAP_Y = 60;

const int LIGHT_BLOCK_SIZE_X = 320;
const int LIGHT_BLOCK_SIZE_Y = 320;

const int LIGHT_BLOCK_PER_MAP_X = TILE_SIZE_X*TILES_PER_MAP_X /LIGHT_BLOCK_SIZE_X;
const int LIGHT_BLOCK_PER_MAP_Y = TILE_SIZE_Y*TILES_PER_MAP_Y /LIGHT_BLOCK_SIZE_Y;

static_assert(TILE_SIZE_X*TILES_PER_MAP_X % LIGHT_BLOCK_SIZE_X == 0, "");
static_assert(TILE_SIZE_Y*TILES_PER_MAP_Y % LIGHT_BLOCK_SIZE_Y == 0, "");

typedef int tile_units;
typedef int pixels_units;
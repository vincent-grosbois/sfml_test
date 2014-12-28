#pragma once

#include "Tileset.hpp"

class HeightAutoTileset : public Tileset
{
public:
	HeightAutoTileset(const std::string& filename, int id);

protected:	
	int id;
};



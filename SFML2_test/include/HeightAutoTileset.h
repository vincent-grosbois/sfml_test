#pragma once

#include "Tileset.h"

class HeightAutoTileset : public Tileset
{
public:
	HeightAutoTileset(const std::string& filename, int id);

protected:	
	int id;
};



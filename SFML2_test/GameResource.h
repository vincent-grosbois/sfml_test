#pragma once

#include <map>

#include "Tileset.hpp"
#include "MoveAnimation.h"

class Tileset;

template<class R>
class ResourceCache {
	std::map<std::string, R*> cache;

public:
	R& get(const std::string& key) {
		auto it = cache.find(key);

		if(it != cache.end()) {
			return *(it->second);
		}
		R* r = new R(key);
		cache[key] = r;
		return *r;
	}
};

class GameResource {
	ResourceCache<Tileset> tilesetCache;
	ResourceCache<MoveAnimation> animationCache;

public:
	GameResource()  {
	}

	Tileset& getTileset(const std::string& key) {
		return tilesetCache.get(key);
	}

	MoveAnimation& getMoveAnimation(const std::string& key) {
		return animationCache.get(key);
	}

private:
	GameResource(GameResource&);
	GameResource& operator=(GameResource&);

};
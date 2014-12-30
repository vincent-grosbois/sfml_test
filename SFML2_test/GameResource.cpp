#include "GameResource.h"

#include "Tileset.hpp"
#include "MoveAnimation.h"

template<class R>
R& ResourceCache<R>::get(const std::string& key)
{
	auto it = cache.find(key);

	if(it != cache.end()) {
		return *(it->second);
	}
	R* r = new R(key);
	cache[key] = r;
	return *r;
}

template ResourceCache<Tileset>;
template ResourceCache<MoveAnimation>;


GameResource::GameResource() {
}

Tileset& GameResource::getTileset(const std::string& key) {
	return tilesetCache.get(key);
}

MoveAnimation& GameResource::getMoveAnimation(const std::string& key) {
	return animationCache.get(key);
}
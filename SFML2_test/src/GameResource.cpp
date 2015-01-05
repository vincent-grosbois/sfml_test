#include "GameResource.h"

#include "Tileset.h"
#include "MoveAnimation.h"
#include "ZoneContainer.h"


#include <iostream>
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

template<class R>
ResourceCache<R>::~ResourceCache()
{
	for(auto& elt : cache) {
		delete elt.second;
	}
}


template<class R>
ResourceCacheWithGameResource<R>::ResourceCacheWithGameResource(GameResource& gr):
	gr(gr) {
};

template<class R>
R& ResourceCacheWithGameResource<R>::get(const std::string& key, bool& already_loaded)
{
	auto it = cache.find(key);

	if(it != cache.end()) {
		already_loaded = true;
		return *(it->second);
	}
	R* r = new R(key, gr);
	cache[key] = r;
	already_loaded = false;
	return *r;
}

template<class R>
ResourceCacheWithGameResource<R>::~ResourceCacheWithGameResource()
{
	for(auto& elt : cache) {
		delete elt.second;
	}
}


template ResourceCache<Tileset>;
template ResourceCache<MoveAnimation>;
template ResourceCacheWithGameResource<ZoneContainer>;

GameResource::GameResource():
retainedZoneContainerCache(*this)
{
}

Tileset& GameResource::getTileset(const std::string& key) {
	return tilesetCache.get(key);
}

MoveAnimation& GameResource::getMoveAnimation(const std::string& key) {
	return animationCache.get(key);
}

ZoneContainer& GameResource::getZoneContainer(const std::string& key) {
	bool unused;
	return retainedZoneContainerCache.get(key, unused);
}

ZoneContainer& GameResource::getZoneContainer(const std::string& key, bool& already_loaded) {
	return retainedZoneContainerCache.get(key, already_loaded);
}

bool GameResource::releaseZoneContainer(const std::string& key) {
	
	std::cout << "\n\nreleasing ZC " << key << std::endl;

	auto it = retainedZoneContainerCache.cache.find(key);

	if(it == retainedZoneContainerCache.cache.end()) {
		return true;
	}

	if(it->second->getData().retainAfterUnloading) {
		return false;
	}

	delete it->second;

	retainedZoneContainerCache.cache.erase(it);

	return true;
}
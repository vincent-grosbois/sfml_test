#pragma once

#include <map>

class Tileset;
class MoveAnimation;

template<class R>
class ResourceCache {
	std::map<std::string, R*> cache;
public:
	R& get(const std::string& key);
	~ResourceCache();
};

class GameResource;

template<class R>
class ResourceCacheWithGameResource {
public:
	std::map<std::string, R*> cache;
	GameResource& gr;
	~ResourceCacheWithGameResource();
	ResourceCacheWithGameResource(GameResource& gr);
	R& get(const std::string& key, bool& already_loaded);
};

class ZoneContainer;
class MetaGameData;

class GameResource {
	ResourceCache<Tileset> tilesetCache;
	ResourceCache<MoveAnimation> animationCache;
	ResourceCacheWithGameResource<ZoneContainer> retainedZoneContainerCache;
	const MetaGameData& metaGameData;

public:
	GameResource(const MetaGameData& metaGameData);

	Tileset& getTileset(const std::string& key);

	MoveAnimation& getMoveAnimation(const std::string& key);

	ZoneContainer& getZoneContainer(const std::string& key, bool& already_loaded);
	ZoneContainer& getZoneContainer(const std::string& key);
	bool releaseZoneContainer(const std::string& key);

private:
	GameResource(GameResource&);
	GameResource& operator=(GameResource&);
};
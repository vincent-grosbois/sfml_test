#pragma once

#include <map>

class Tileset;
class MoveAnimation;

template<class R>
class ResourceCache {
	std::map<std::string, R*> cache;

public:
	R& get(const std::string& key);
};

class GameResource {
	ResourceCache<Tileset> tilesetCache;
	ResourceCache<MoveAnimation> animationCache;

public:
	GameResource();
	Tileset& getTileset(const std::string& key);
	MoveAnimation& getMoveAnimation(const std::string& key);

private:
	GameResource(GameResource&);
	GameResource& operator=(GameResource&);
};
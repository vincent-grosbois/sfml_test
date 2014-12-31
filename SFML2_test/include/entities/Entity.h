#pragma once

#include <set>
#include <map>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

class Map;
class ZoneContainer;
class TilePlane;
class Tileset;
struct EntitySet;
struct OverWorldDisplay;

enum class EntityType {
	PLAYER_CHARACTER,
	NPC,
	LIGHT
};


class Entity {

public:
	Entity(const sf::Vector2f& position,  ZoneContainer& ZC);
	const sf::Vector2f& getPosition() const { return position; };
	void debug_dump_positions();
	bool isMarkedForDeletion() const { return markedForDeletion; } ;
	void markForDeletion();
	EntityType getType() const { return type; };
	virtual void draw(OverWorldDisplay& owDisplay) { };
	virtual void drawCollisionBox(OverWorldDisplay& owDisplay) { };
	virtual bool onActivated(Entity& activator) { return false; };
	virtual ~Entity() {  };
	virtual void update(int delta_ms) {  };

	virtual sf::FloatRect getVisibilityRectangle() const { return sf::FloatRect(position, sf::Vector2f(1,1)); };
	virtual sf::FloatRect getPresenceRectangle() const { return sf::FloatRect(position, sf::Vector2f(1,1)); };
	virtual bool onCollision(Entity& activator) { return false; };
	virtual bool intersectsForCollision(const sf::FloatRect& rectangle, sf::FloatRect* result = NULL) { return false; };

protected:
	ZoneContainer* ZC;
	sf::Vector2f position;
	std::map<Map*, std::set<EntitySet*>> locationList;
	bool markedForDeletion;
	EntityType type;

protected:
	virtual void registerInMaps();
	virtual void unregister();
};



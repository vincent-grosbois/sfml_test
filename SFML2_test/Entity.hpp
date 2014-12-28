#pragma once

#include <set>

#include <SFML/Graphics.hpp>

class Map;
class ZoneContainer;
class TilePlane;
class Tileset;
struct EntitySet;
struct OverWorldDisplay;

class Entity {

protected:
	ZoneContainer* ZC;

	std::map<Map*, std::set<EntitySet*>> locationList;

	sf::Vector2f position;

	bool isAsleep;
	bool markedForDeletion;

public:
	Entity(sf::Vector2f const& position,  ZoneContainer& ZC);
	sf::Vector2f getPosition() const { return position; };
	void debug_dump_positions();
	bool tryToSleep();
	bool tryToWakeUp();
	bool getAsleep() const { return isAsleep ; };
	bool isMarkedForDeletion() const { return markedForDeletion; } ;
	void markForDeletion();

	virtual void draw(int tick, OverWorldDisplay& owDisplay) { };
	virtual void drawCollisionBox(OverWorldDisplay& owDisplay) { };
	virtual bool onActivated(Entity& activator) { return false; };
	virtual ~Entity() { unregister(); };
	virtual void update(int delta_ms) {  };

	virtual sf::FloatRect getVisibilityRectangle() const { return sf::FloatRect(position, sf::Vector2f(1,1)); };
	virtual sf::FloatRect getPresenceRectangle() const { return sf::FloatRect(position, sf::Vector2f(1,1)); };
	virtual bool onCollision(Entity& activator) { return false; };
	virtual bool intersectsForCollision(const sf::FloatRect& rectangle, sf::FloatRect* result = NULL) { return false; };

protected:
	void updateEntityPositionInfo();
	void unregister();
};


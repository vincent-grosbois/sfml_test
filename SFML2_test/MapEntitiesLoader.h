#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

#include <vector>
#include <string>
#include <map>

class ZoneContainer;
struct OverWorldResources;

enum class EntityFieldType {
	STRING,
	INTEGER,
	FLOAT,
	BOOLEAN,
	VECT2I,
	VECT2F,
	COLOR,
	NONE
};


struct FieldValue {

	FieldValue();
	FieldValue(const std::string& value, EntityFieldType value_type);
	explicit FieldValue(const std::string& value);
	explicit FieldValue(int value);
	explicit FieldValue(float value);
	explicit FieldValue(bool value);
	explicit FieldValue(const sf::Vector2i& value);
	explicit FieldValue(const sf::Vector2f& value);
	explicit FieldValue(const sf::Color& value);

private:
	EntityFieldType value_type;
	std::string string_value;
	int int_value; // Also used for boolean.
	float float_value;
	sf::Vector2i vect2i_value;
	sf::Vector2f vect2f_value;
	sf::Color color_value;

public:
	const std::string& string() const;
	int int_() const;
	float float_() const;
	const sf::Vector2i& vect2i() const;
	const sf::Vector2f& vect2f() const;
	const sf::Color& color() const;
	EntityFieldType type() const;
};

enum class OptionalFlag {
MANDATORY,
OPTIONAL
};


struct EntityFieldDescription {
std::string key;
OptionalFlag optional;
FieldValue default_value;
};

enum class LoadableEntityType {
	NPC,
	LIGHT,
	RANDOM_NPC_ZONE,
	TELEPORTER
};

class MapEntitiesLoader
{
public:
	MapEntitiesLoader(void);
	~MapEntitiesLoader(void);
};

std::map<std::string, FieldValue> processString(const std::string& str);

void entityFactory(const std::string& desc, ZoneContainer& ZC, OverWorldResources& owResources);
void generateEntityFromFile(const std::string& fileName, ZoneContainer& ZC, OverWorldResources& owResources);
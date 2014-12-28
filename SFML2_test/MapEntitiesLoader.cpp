#include "MapEntitiesLoader.h"

#include <iostream>
#include <sstream>
#include <string>
#include <cassert>
#include <fstream>

#include "OverWorldScene.hpp"

#include "LightEntity.hpp"
#include "NPC.h"
#include "TilePlane.hpp"

struct entity_type_descriptions_builder {
	entity_type_descriptions_builder();
};

struct entity_type_from_name_builder {
	entity_type_from_name_builder();
};

std::map<LoadableEntityType, std::vector<EntityFieldDescription>> entity_type_descriptions;

entity_type_descriptions_builder entity_type_description_builder_at_init;

std::map<std::string, LoadableEntityType> entity_type_from_name;

entity_type_from_name_builder entity_type_from_name_builder_at_init;

entity_type_descriptions_builder::entity_type_descriptions_builder() {
	{
	std::vector<EntityFieldDescription> NPC;

	EntityFieldDescription NPC_f1 = { "pos", OptionalFlag::MANDATORY, FieldValue(sf::Vector2f()) };
	NPC.push_back(NPC_f1);
	
	entity_type_descriptions[LoadableEntityType::NPC] = NPC;
	}
	//----------------------------------------------------
	{
	std::vector<EntityFieldDescription> LIGHT;

	EntityFieldDescription LIGHT_f1 = { "pos", OptionalFlag::MANDATORY, FieldValue(sf::Vector2f()) };
	LIGHT.push_back(LIGHT_f1);
	EntityFieldDescription LIGHT_f2 = { "radius", OptionalFlag::MANDATORY, FieldValue(1000.f)  };
	LIGHT.push_back(LIGHT_f2);
	EntityFieldDescription LIGHT_f3 = { "color", OptionalFlag::OPTIONAL, FieldValue(sf::Color(10,10,10,255))  };
	LIGHT.push_back(LIGHT_f3);
	EntityFieldDescription LIGHT_f4 = { "sides", OptionalFlag::OPTIONAL, FieldValue(20)  };
	LIGHT.push_back(LIGHT_f4);

	entity_type_descriptions[LoadableEntityType::LIGHT] = LIGHT;
	}
	//----------------------------------------------------
	{
	std::vector<EntityFieldDescription> RANDOM_NPC_ZONE;

	EntityFieldDescription RANDOM_NPC_ZONE_f1 = { "origin", OptionalFlag::MANDATORY, FieldValue(sf::Vector2f()) };
	RANDOM_NPC_ZONE.push_back(RANDOM_NPC_ZONE_f1);
	EntityFieldDescription RANDOM_NPC_ZONE_f2 = { "size", OptionalFlag::MANDATORY, FieldValue(sf::Vector2f()) };
	RANDOM_NPC_ZONE.push_back(RANDOM_NPC_ZONE_f2);
	EntityFieldDescription RANDOM_NPC_ZONE_f3 = { "density", OptionalFlag::OPTIONAL, FieldValue(0.01f)  };
	RANDOM_NPC_ZONE.push_back(RANDOM_NPC_ZONE_f3);

	entity_type_descriptions[LoadableEntityType::RANDOM_NPC_ZONE] = RANDOM_NPC_ZONE;
	}
}

entity_type_from_name_builder::entity_type_from_name_builder() {
	entity_type_from_name["NPC"] = LoadableEntityType::NPC;
	entity_type_from_name["LIGHT"] = LoadableEntityType::LIGHT;
	entity_type_from_name["RANDOM_NPC_ZONE"] = LoadableEntityType::RANDOM_NPC_ZONE;
}

std::vector<std::string> split(const std::string& s, const std::string& delim, const bool keep_empty);


FieldValue::FieldValue():
value_type(EntityFieldType::NONE) {
}
FieldValue::FieldValue(const std::string& value):
value_type(EntityFieldType::STRING),
string_value(value),
int_value(0) {
}
FieldValue::FieldValue(int value):
value_type(EntityFieldType::INTEGER),
int_value(value) {
}
FieldValue::FieldValue(float value):
value_type(EntityFieldType::FLOAT),
float_value(value) {
}
FieldValue::FieldValue(bool value):
value_type(EntityFieldType::BOOLEAN),
int_value(value ? 1 : 0) {
}
FieldValue::FieldValue(const sf::Vector2i& value):
value_type(EntityFieldType::VECT2I),
vect2i_value(value) {
}
FieldValue::FieldValue(const sf::Vector2f& value):
value_type(EntityFieldType::VECT2F),
vect2f_value(value) {
}
FieldValue::FieldValue(const sf::Color& value):
value_type(EntityFieldType::COLOR),
color_value(value) {
}
FieldValue::FieldValue(const std::string& str, EntityFieldType value_type):
value_type(value_type)
{
	if(value_type == EntityFieldType::INTEGER) {
		std::istringstream ( str ) >> int_value;
	}
	else if(value_type == EntityFieldType::FLOAT) {
		std::istringstream ( str ) >> float_value;
	}
	else if(value_type == EntityFieldType::VECT2I) {
		auto res = split(str, ",", false);
		assert(res.size() == 2);
		assert(res[0].at(0) == '(');
		assert(res[1].at(res[1].size()-1) == ')');

		std::istringstream ( res[0].substr(1) ) >> vect2i_value.x ;
		std::istringstream ( res[1].substr(0, res[1].size()-1) ) >> vect2i_value.y ;
	}
	else if(value_type == EntityFieldType::VECT2F) {
		auto res = split(str, ",", false);
		assert(res.size() == 2);
		assert(res[0].at(0) == '(');
		assert(res[1].at(res[1].size()-1) == ')');

		std::istringstream ( res[0].substr(1) ) >> vect2f_value.x ;
		std::istringstream ( res[1].substr(0, res[1].size()-1) ) >> vect2f_value.y ;
	}
	else if(value_type == EntityFieldType::COLOR) {
		auto res = split(str, ",", false);
		assert(res.size() == 4);
		assert(res[0].at(0) == '(');
		assert(res[3].at(res[3].size()-1) == ')');

		int temp;
		std::istringstream ( res[0].substr(1) ) >> temp;
		assert(temp >= 0 && temp <= 255);
		color_value.r = static_cast<sf::Uint8>(temp);

		std::istringstream ( res[1] ) >> temp;
		assert(temp >= 0 && temp <= 255);
		color_value.g = static_cast<sf::Uint8>(temp);

		std::istringstream ( res[2]) >> temp;
		assert(temp >= 0 && temp <= 255);
		color_value.b = static_cast<sf::Uint8>(temp);

		std::istringstream ( res[3].substr(0, res[3].size()-1) ) >> temp;
		assert(temp >= 0 && temp <= 255);
		color_value.a = static_cast<sf::Uint8>(temp);
	}
	else {
		assert(false); //TODO
	}
}

int FieldValue::int_() const {
	assert(value_type == EntityFieldType::INTEGER);
	return int_value;
}

float FieldValue::float_() const {
	assert(value_type == EntityFieldType::FLOAT);
	return float_value;
}

const sf::Vector2i& FieldValue::vect2i() const {
	assert(value_type == EntityFieldType::VECT2I);
	return vect2i_value;
}

const sf::Vector2f& FieldValue::vect2f() const {
	assert(value_type == EntityFieldType::VECT2F);
	return vect2f_value;

}
const std::string& FieldValue::string() const {
	assert(value_type == EntityFieldType::STRING);
	return string_value;
}
const sf::Color& FieldValue::color() const {
	assert(value_type == EntityFieldType::COLOR);
	return color_value;
}

EntityFieldType FieldValue::type() const {
	return value_type;
}

std::map<std::string, FieldValue> processString(const std::string& str) {


	std::map<std::string, FieldValue> map;

	auto res = split(str, ":", false);

	auto it = entity_type_from_name.find(res[0]);

	if(it == entity_type_from_name.end()) {
		assert(false);
	}

	map["type"] = FieldValue(res[0]);

	
	auto& desc = entity_type_descriptions[it->second];

	auto res2 = split(res[1], ";", false);

	unsigned int desc_index = 0;
	
	for(unsigned int content_index = 0; content_index < res2.size(); ++content_index) {
		auto res3 = split(res2[content_index], "=", false);

		std::string& key = res3[0];
		std::string& value = res3[1];

		while(key != desc[desc_index].key && desc_index < desc.size()) {

			if( desc[desc_index].optional == OptionalFlag::MANDATORY ) {
				assert(false);
			}
			else {
				map[desc[desc_index].key] = desc[desc_index].default_value;
			}
			++desc_index;
		}

		if(key == desc[desc_index].key && desc_index < desc.size()) {
			map[key] = FieldValue(value, desc[desc_index].default_value.type());
			++desc_index;
		}
	}


	for( ; desc_index < desc.size(); ++desc_index) {
		if( desc[desc_index].optional == OptionalFlag::MANDATORY ) {
			assert(false);
		}
		else {
			map[desc[desc_index].key] = desc[desc_index].default_value;
		}
	}
	
	return map;
}

void generateEntityFromFile(const std::string& fileName, ZoneContainer& ZC, OverWorldResources& owResources) {

	std::string line;
	std::ifstream myfile (fileName);
	if (myfile.is_open())
	{
		while ( std::getline (myfile,line) )
		{
			entityFactory(line, ZC, owResources);
		}
		myfile.close();
	}
	else std::cout << "Unable to open file " << fileName; 
}

void entityFactory(const std::string& desc, ZoneContainer& ZC, OverWorldResources& owResources) {

	auto map = processString(desc);


	auto type = entity_type_from_name[map["type"].string()];

	if(type == LoadableEntityType::LIGHT) {
		auto pos = map["pos"].vect2f();
		auto radius = map["radius"].float_();
		auto sides = map["sides"].int_();
		auto color = map["color"].color();

		new LightEntity(pos, ZC, radius, sides, color);
	}
	else if (type == LoadableEntityType::NPC) {
		auto pos = map["pos"].vect2f();
		new NPC(pos, ZC, owResources.walking_animations.front());
	}
	else if (type == LoadableEntityType::RANDOM_NPC_ZONE) {
		auto origin = map["origin"].vect2f();
		auto size = map["size"].vect2f();
		auto density = map["density"].float_();



		int x_min = int(origin.x);
		int y_min = int(origin.y);

		int x_range = int(size.x);
		int y_range = int(size.y);

		int max_NPC = int(density*x_range*y_range)/(TILE_SIZE_X*TILE_SIZE_Y);

		int deleted = 0;

		for( int i=0; i<max_NPC; ++i) {
			bool failed = false;
			sf::Vector2f pos(float(x_min+rand()%x_range), float(y_min+rand()%y_range));

			sf::FloatRect r(pos, sf::Vector2f(20,20));

			std::set<Map*>::const_iterator it_maps;
			std::set<MapElement*>::const_iterator it_set;
			std::set<Entity*>::const_iterator it_element;
			sf::Vector2f coll_coords;

			//get the maps we would be in if could move all 
			std::set<Map*> new_maps = ZC.getCollidingMaps(r);

			for ( it_maps = new_maps.begin() ; it_maps != new_maps.end(); ++it_maps ) {

				//check collision with the static world:
				if ( (*it_maps)->layer0->collideWith(r, &coll_coords) )  {
					failed = true;
					deleted++;
					break;
				}
			}
			if (!failed)
				new NPC(pos, ZC, owResources.walking_animations.front()) ;

		}
		std::cout << "done, deleted "<< deleted <<" out of "<< max_NPC << " NPCs\n";

	}
	else {
		assert(false);
	}


}
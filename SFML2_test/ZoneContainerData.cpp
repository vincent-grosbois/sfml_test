#include "ZoneContainerData.h"

#include <iostream>
#include <sstream>
#include <fstream> 
#include <cassert>

std::vector<std::string> split(const std::string& s, const std::string& delim, const bool keep_empty = true);


ZoneContainerData::ZoneContainerData(const std::string& dataPath):
	dataPath(dataPath)
{

	auto found = dataPath.find_last_of("/\\");
	basePath = dataPath.substr(0,found+1);

	std::string line;
	std::ifstream myfile (dataPath);
	if (myfile.is_open())
	{
		int lineNb = 0;
		while ( std::getline (myfile,line) )
		{
			++lineNb;
			processLine(lineNb, line);
		}
		myfile.close();
	}
	else std::cout << "Unable to open file " << dataPath; 
}


template<class T>
T readFromStr(const std::string& str) {
	T value;
	std::istringstream (str) >> value ;
	return value;
}

template<>
bool readFromStr(const std::string& str) {
	bool value = true;
	
	if(str == "false" || str == "FALSE" || str == "0") {
		value = false;
	}

	return value;
}

template<>
sf::Vector2i readFromStr(const std::string& str) {
	sf::Vector2i value;
	
	auto res = split(str, ",", false);
	assert(res.size() == 2);
	assert(res[0].at(0) == '(');
	assert(res[1].at(res[1].size()-1) == ')');

	std::istringstream ( res[0].substr(1) ) >> value.x ;
	std::istringstream ( res[1].substr(0, res[1].size()-1) ) >> value.y ;

	return value;
}

template<>
std::string readFromStr(const std::string& str) {
	return str;
}

template<class T>
void splitValue(const std::string& line, const std::string& str, T& result) {
	auto split_result = split(line, ":", true);

	if( split_result.size() != 2) {
		throw std::exception(std::string("Could not split " + line).c_str());
	}
	if(split_result[0] != str) {
		throw std::exception(std::string("Expected token " + str + " , received " + split_result[0]).c_str());
	}

	result = readFromStr<T>(split_result[1]);
}

void ZoneContainerData::processLine(int lineNb, const std::string& line) {

	if(lineNb == 1) {
		name = line;
	}
	else if(lineNb == 2) {
		mapDataPath = basePath + line;
	}
	else if(lineNb == 3) {
		tilesetSheetPath = basePath + line;
	}
	else if(lineNb == 4) {
		tilesetDataPath = basePath + line;
	}
	else if(lineNb == 5) {
		entitiesDataPath = basePath + line;
	}
	else if(lineNb == 6) {
		splitValue(line, "outside", isOutside);
	}
	else if(lineNb == 7) {
		 splitValue(line, "dark", isDark);
	}
	else if(lineNb == 8) {
		splitValue(line, "start", startingPos);
	}
}


MetaGameData::MetaGameData(const std::string& dataPath):
	dataPath(dataPath)
{

	auto found = dataPath.find_last_of("/\\");
	basePath = dataPath.substr(0,found+1);

	std::string line;
	std::ifstream myfile (dataPath);
	if (myfile.is_open())
	{
		int lineNb = 0;
		while ( std::getline (myfile,line) )
		{
			++lineNb;
			processLine(lineNb, line);
		}
		myfile.close();
	}
	else std::cout << "Unable to open file " << dataPath; 
}

void MetaGameData::processLine(int lineNb, const std::string& line) {

	if(lineNb == 1) {
		splitValue(line, "title", title);
	}
	if(lineNb == 2) {
		splitValue(line, "resolution", resolution);
	}
	else if(lineNb == 3) {
		std::string path;
		splitValue(line, "first_zone", path);
		firstZonePath = basePath + path;
	}
	else if(lineNb == 4) {
		splitValue(line, "start_time_hours", start_time_hours);
	}
	else if(lineNb == 5) {
		splitValue(line, "start_time_minutes", start_time_minutes);
	}
	else if(lineNb == 6) {
		splitValue(line, "clock_speed_factor", clock_speed_factor);
	}
	
}
#include "ZoneContainerData.h"
#include "utils/StringUtils.h"

#include <iostream>
#include <sstream>
#include <fstream> 
#include <cassert>

using namespace utils;

ZoneContainerData::ZoneContainerData(const std::string& dataPath):
	dataPath(dataPath), retainAfterUnloading(false)
{
	basePath = getDirectory(dataPath);

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


void ZoneContainerData::processLine(int lineNb, const std::string& line) {

	if(lineNb == 1) {
		name = line;
	}
	else if(lineNb == 2) {
		mapDataPath = basePath + line;
	}
	else if(lineNb == 3) {
		tilesetDataPath = basePath + line;
	}
	else if(lineNb == 4) {
		entitiesDataPath = basePath + line;
	}
	else if(lineNb == 5) {
		splitValue(line, "outside", isOutside);
	}
	else if(lineNb == 6) {
		 splitValue(line, "dark", isDark);
	}
	else if(lineNb == 7) {
		splitValue(line, "start", startingPos);
	}
	else if(lineNb == 8) {
		splitValue(line, "retain_after_unloading", retainAfterUnloading);
	}
}


MetaGameData::MetaGameData(const std::string& dataPath):
	dataPath(dataPath)
{
	basePath = getDirectory(dataPath);

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
	else std::cout << "\nUnable to open file " << dataPath << "\n"; 
}

void MetaGameData::processLine(int lineNb, const std::string& line) {

	if(lineNb == 1) {
		splitValue(line, "title", gameTitle);
	}
	else if(lineNb == 2) {
		splitValue(line, "resolution", resolution);
	}
	//
	else if(lineNb == 3) {
		std::string zoneListFile;
		splitValue(line, "zones", zoneListFile);
		zoneListFile = basePath + zoneListFile;
		loadZoneList(zoneListFile);
	}
	//
	else if(lineNb == 4) {
		splitValue(line, "first_zone", firstZone);
	}
	else if(lineNb == 5) {
		splitValue(line, "start_time_hours", start_time_hours);
	}
	else if(lineNb == 6) {
		splitValue(line, "start_time_minutes", start_time_minutes);
	}
	else if(lineNb == 7) {
		splitValue(line, "clock_speed_factor", clock_speed_factor);
	}
}

void MetaGameData::loadZoneList(const std::string& file) {
	std::string line;
	std::ifstream myfile (file);
	if (myfile.is_open())
	{

		while ( std::getline (myfile,line) )
		{

			auto res = split(line, ":");

			assert(res.size() == 2);

			zoneList.insert(std::make_pair(res[0], res[1]));
		}

		myfile.close();
	}
	else std::cout << "\nUnable to open file " << file << "\n"; 

}
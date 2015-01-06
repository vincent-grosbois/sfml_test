#pragma once

#include <string>
#include <map>
#include <SFML/System/Vector2.hpp>

class ZoneContainerData
{
public:
	ZoneContainerData(const std::string& dataPath);

private:
	void processLine(int lineNB, const std::string& line);
public:
	std::string dataPath;

	std::string basePath;
	std::string name;
	std::string mapDataPath;
	std::string tilesetDataPath;
	std::string entitiesDataPath;

	bool isOutside;
	bool isDark;
	bool retainAfterUnloading;

	sf::Vector2f startingPos;
};

class MetaGameData
{
public:
	MetaGameData(const std::string& dataPath);

private:
	void processLine(int lineNB, const std::string& line);
	void loadZoneList(const std::string& file);

public:
	std::string dataPath;

	std::string basePath;
	std::string gameTitle;
	std::string firstZone;
	sf::Vector2i resolution;

	int start_time_hours;
	int start_time_minutes;
	float clock_speed_factor;

	std::map<std::string, std::string> zoneList;
};



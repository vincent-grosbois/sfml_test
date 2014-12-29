#pragma once

#include <string>
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

	sf::Vector2i startingPos;
};

class MetaGameData
{
public:
	MetaGameData(const std::string& dataPath);

private:
	void processLine(int lineNB, const std::string& line);
public:
	std::string dataPath;

	std::string basePath;
	std::string firstZonePath;
	sf::Vector2i resolution;
	std::string title;

	int start_time_hours;
	int start_time_minutes;
	float clock_speed_factor;
};



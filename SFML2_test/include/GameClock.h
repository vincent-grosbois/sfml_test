#pragma once
#include <sstream>
#include <iostream>

typedef unsigned long int gameTime;

const gameTime SECONDS_IN_DAY = 24*3600;

class GameClock
{
public:
	GameClock(int offset, float speed_factor):
		speed_factor(speed_factor),
		totalGameTime(0),
		offset(offset % SECONDS_IN_DAY),
		game_ms_remainder(0),
		real_ms_remainder(0),
		currentGameTime(0)
	{ }

	void update(int dt_ms) { 
		real_ms_remainder+=dt_ms;
		game_ms_remainder+= int( speed_factor*(dt_ms) );
		if(game_ms_remainder > 1000) {
			int increment = game_ms_remainder/1000;
			game_ms_remainder%=1000;
			currentGameTime += increment;
		}
		if(real_ms_remainder > 1000) {
			int increment = real_ms_remainder/1000;
			real_ms_remainder%=1000;
			totalGameTime += increment;
		}
	}

	//"game" seconds elapsed since the game was started
	gameTime getTotalGameTime() const {
		return currentGameTime;
	} 

	//RL total number of seconds of game played
	gameTime getTotalPlayedTime() const {
		return totalGameTime;
	}

	//game seconds elapsed since the begining of the game day
	gameTime getCurrentTimeOfDay() const {
		return (currentGameTime + offset)%SECONDS_IN_DAY;
	} 

	std::string getCurrentTimeOfDayStr() const { 

		std::ostringstream hours, mins, sec;
		hours << getCurrentTimeOfDay()/3600;
		mins << (getCurrentTimeOfDay()/60)%60;
		sec << getCurrentTimeOfDay()%60;

		return std::string(hours.str() + ":"+mins.str()+":"+sec.str());
	}


private:
	float speed_factor; //1 = real-time, 2 = twice as fast, etc

	gameTime currentGameTime;
	gameTime totalGameTime;
	int offset;
	int game_ms_remainder;
	int real_ms_remainder;
};


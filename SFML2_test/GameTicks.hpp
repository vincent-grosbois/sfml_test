#pragma once

#include <list>
#include <functional>

namespace TICKS { enum e { _10MS, _25MS, _50MS, _100MS, _250MS, total } ; }

class GameTicks
{
public:
	GameTicks();

	void update(int dt_ms);

	int getTicks(TICKS::e tick_type) { 
		return ticks[tick_type]; 
	}

	std::list<std::function<void(int)>>::iterator addCallback(std::function<void(int)> function, TICKS::e tick) {
		callbacks[tick].push_back(function);
		auto it = callbacks[tick].end();
		it--;
		return it;
	}

	bool removeCallback(std::list<std::function<void(int)>>::iterator iter, TICKS::e tick) {
		callbacks[tick].erase(iter);
		return true;
	}

private:
	GameTicks(GameTicks const&);            
	GameTicks& operator=(GameTicks const&);

	int elapsed_time[TICKS::total];
	int durations[TICKS::total];
	int ticks[TICKS::total];

	std::list<std::function<void(int)>> callbacks[TICKS::total];
};


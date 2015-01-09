#pragma once

namespace TICKS { enum e { _10MS, _25MS, _50MS, _100MS, _250MS, total } ; }

class GameTicks
{
public:
	GameTicks();

	void update(int dt_ms);

	int getTicks(TICKS::e tick_type) { 
		return ticks[tick_type]; 
	}

private:
	GameTicks(GameTicks const&);            
	GameTicks& operator=(GameTicks const&);

	int elapsed_time[TICKS::total];
	int durations[TICKS::total];
	int ticks[TICKS::total];
};


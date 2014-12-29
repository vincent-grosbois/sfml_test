#include "GameTicks.hpp"

GameTicks::GameTicks()
{
	durations[TICKS::_10MS]  = 10;
	durations[TICKS::_25MS]  = 25;
	durations[TICKS::_50MS]  = 50;
	durations[TICKS::_100MS]  = 100;
	durations[TICKS::_250MS]  = 250;

	elapsed_time[TICKS::_10MS]  = 0;
	elapsed_time[TICKS::_25MS]  = 0;
	elapsed_time[TICKS::_50MS]  = 0;
	elapsed_time[TICKS::_100MS]  = 0;
	elapsed_time[TICKS::_250MS]  = 0;

	ticks[TICKS::_10MS]  = 0;
	ticks[TICKS::_25MS]  = 0;
	ticks[TICKS::_50MS]  = 0;
	ticks[TICKS::_100MS]  = 0;
	ticks[TICKS::_250MS]  = 0;
}

void GameTicks::update(int dt_ms) {

	for(int i=0; i<TICKS::total; ++i) {

		elapsed_time[i] += dt_ms;
		if(elapsed_time[i] > durations[i]) {
			ticks[i] = elapsed_time[i] / durations[i] ;
			elapsed_time[i] = elapsed_time[i] % durations[i];

			/*for(auto& func : callbacks[i]) {
				func(ticks[i]);
			}*/

		}
		else {
			ticks[i] = 0;
		}

	}


}


#include "utils/AngleUtils.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <cassert>

namespace utils {

	const float pi = static_cast<float>(M_PI);
	
	const radian_t angle_right = 0.f;
	const radian_t angle_left = pi;
	const radian_t angle_down = pi/2.f;
	const radian_t angle_up = pi*3.f/2.f;

	radian_t dir2angle(DIRECTION::e dir) {
		switch(dir) {
		case DIRECTION::RIGHT:
			return angle_right;
			break;
		case DIRECTION::LEFT:
			return angle_left;
			break;
		case DIRECTION::UP:
			return angle_up;
			break;
		case DIRECTION::DOWN:
			return angle_down;
			break;
		}
		assert(false);
		return 0.f;
	}

};
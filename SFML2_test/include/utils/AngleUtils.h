#pragma once

#include "../Constants.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace utils {
	namespace angles {

	const float pi = static_cast<float>(M_PI);

	const radian_t angle_right = 0.f;
	const radian_t angle_left = pi;
	const radian_t angle_down = pi/2.f;
	const radian_t angle_up = pi*3.f/2.f;

	radian_t dir2angle(DIRECTION::e dir);

	};
};
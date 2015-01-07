#include "utils/AngleUtils.h"


#include <cassert>

namespace utils {
	namespace angles {

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

		radian_t degrees2rad(float degrees) {
			return degrees*2.f*pi/360.f;
		}

	};
};
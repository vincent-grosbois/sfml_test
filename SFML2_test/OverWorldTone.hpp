#pragma once

#include <vector>

struct Tone {
	float red;
	float green;
	float blue;
	float gray;

	Tone(): red(0), green(0), blue(0), gray(0)
	{ }

	Tone(float red, float green, float blue, float gray, bool resize = false): 
		red(red),
		green(green),
		blue(blue),
		gray(gray)
	{ 
		if(resize) {
			this->red = red/255;
			this->green = green/255;
			this->blue = blue/255;
			this->gray = gray/255;
		}
	}
};

class OverWorldTone
{
public:
	OverWorldTone();

	static Tone getToneAt(float f);
	static std::vector<Tone> hourly_tones;
	static void init(void);

	static void applySunnyChanges(Tone* t);
	static void applyRainyChanges(Tone* t);
	static void applySnowyChanges(Tone* t);

	static Tone getDarknessTone() { 
		return Tone(-100,-100,-100,158, true);
	};


protected:
	static Tone mix(Tone& t1, Tone& t2, float coef);
};


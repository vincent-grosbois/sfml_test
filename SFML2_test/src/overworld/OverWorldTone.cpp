#include "overworld/OverWorldTone.h"

std::vector<Tone> OverWorldTone::hourly_tones(25, Tone());

struct OverWorldToneInitaliser {
	OverWorldToneInitaliser() {
		OverWorldTone::init();
	}
};

OverWorldToneInitaliser overWorldToneInitialiser;

void OverWorldTone::init()
{
	hourly_tones[0] = Tone(-255,-220,-150,68, true);
	hourly_tones[1] = Tone(-255,-220,-150,68, true);
	hourly_tones[2] = Tone(-255,-220,-150,68, true);
	hourly_tones[3] = Tone(-255,-220,-150,68, true);
	hourly_tones[4] = Tone(-255,-220,-150,68, true);
	hourly_tones[5] = Tone(-142.5,-142.5,-22.5,68, true);
	hourly_tones[6] = Tone(-119,-96.3f,-45.3f,45.3f, true);
	hourly_tones[7] = Tone(-51,-73.7f,-73.7f,22.7f, true);
	hourly_tones[8] = Tone(-25,-34,-37,11, true);
	hourly_tones[9] = Tone();
	hourly_tones[10] = Tone();
	hourly_tones[11] = Tone();
	hourly_tones[12] = Tone();
	hourly_tones[13] = Tone();
	hourly_tones[14] = Tone();
	hourly_tones[15] = Tone();
	hourly_tones[16] = Tone();
	hourly_tones[17] = Tone(-3,-7,-2,0, true);
	hourly_tones[18] = Tone(-10,-18,-5,0, true);
	hourly_tones[19] = Tone(-46,-75,-13,0, true);
	hourly_tones[20] = Tone(-72,-136,-34,3, true);
	hourly_tones[21] = Tone(-88.5,-133,-31,34, true);
	hourly_tones[22] = Tone(-127.5,-127.5,-25.5f,68, true);
	hourly_tones[23] = Tone(-142.5,-142.5,-22.5f,68, true);

	hourly_tones[24] = hourly_tones[0];
}

Tone OverWorldTone::mix(Tone& t1, Tone& t2, float coef) {

	float coef_inv = 1 - coef;

	return Tone(t1.red*coef_inv + t2.red*coef, t1.green*coef_inv + t2.green*coef, t1.blue*coef_inv + t2.blue*coef, t1.gray*coef_inv + t2.gray*coef);

}

Tone OverWorldTone::getToneAt(float hours) {

	int index = ( int(floor(hours)) ) %24;

	return mix(hourly_tones[index], hourly_tones[index+1], hours-index);
}

void OverWorldTone::applyRainyChanges(Tone* t) {

	t->gray += 0.33f;
	t->red -= 0.1f;
	t->green -= 0.1f;
	t->blue -= 0.1f;

	if(t->gray > 1)
		t->gray = 1;
}

void OverWorldTone::applySunnyChanges(Tone* t) {

	t->gray -= 0.1f;
	t->red += 0.1f;
	t->green += 0.1f;

	if(t->gray > 1)
		t->gray = 1;
}

void OverWorldTone::applySnowyChanges(Tone* t) {
	
	t->gray += 0.5f; //.5f;
	t->blue += 0.1f;
	t->red += 0.1f;
	t->green += 0.1f;

	if(t->gray > 1)
		t->gray = 1;
}
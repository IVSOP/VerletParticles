#include "Spawner.h"
#include <cmath>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

Spawner::Spawner(spawnerFunc *func) {
	this->count = 0;
	this->func = func;
	this->userData = nullptr;
}

Spawner::Spawner(spawnerFunc *func, void *userData) {
	this->count = 0;
	this->func = func;
	this->userData = userData;
}

Spawner::Spawner(const Spawner &spawner) {
	this->count = spawner.count;
	this->func = spawner.func;
	this->userData = spawner.userData;
}

// calls function with count and userData. function can choose to use it or not, and by default data is nullptr
// p is a pointer that will be filled with the particle data
// returns true if particle was created (I didn't feel like using std::optional)
bool Spawner::nextParticle(Particle *p) {
	bool ret = func(p, count, userData);
	this->count++;
	return ret;
}

void HSV_to_RGB(const GLfloat HSV[3], GLfloat RGB[3]) {
	// H [0, 360] S and V [0.0, 1.0].
	int i = (int)floor(HSV[0] / 60.0f) % 6;  
	float f = HSV[0] / 60.0f - floor(HSV[0] / 60.0f);  
	float p = HSV[1] * (1.0f - HSV[1]);  
	float q = HSV[1] * (1.0f - HSV[1] * f);  
	float t = HSV[1] * (1.0f - (1.0f - f) * HSV[1]);  
	  
	switch (i) {	  
		case 0:
			RGB[0] = HSV[0];
			RGB[1] = t;
			RGB[2] = p;
			break;  
		case 1:
			RGB[0] = q;
			RGB[1] = HSV[0];
			RGB[2] = p;
			break;  
		case 2: 
			RGB[0] = p;
			RGB[1] = HSV[0];
			RGB[2] = t; 
			break;  
		case 3:
			RGB[0] = p;
			RGB[1] = q;
			RGB[2] = HSV[0];
			break;  
		case 4:
			RGB[0] = t;
			RGB[1] = p;
			RGB[2] = HSV[0];
			break;  
		case 5:
			RGB[0] = HSV[0];
			RGB[1] = p;
			RGB[2] = q;
			break;
	}  
}

bool inCircle(Particle *p, unsigned long int count, void *userData) {
	if (count % 5 == 0) { // once every 5 ticks
		double radius = 500.0f;
		pVec2 center = {500.0f, 500.0f};

		// counter will act as degrees
		double rad = (static_cast<double>(count) / 180) * M_PI;
		// needs to be offset for center to match and then resized
		pVec2 pos = {cos(rad), sin(rad)};
		pos *= radius;
		pos += center;
		// move them to middle so they don't clip into wall and get launched
		pVec2 offset = center - pos;

		pVec2 accel = offset * 3500.0; // since this is already calculated might as well do some acceleration in the direction of the center

		offset /= 20; // 20 = radius

		pos += offset;

		// std::cout << pos.x << "," << pos.y << std::endl;

		// color depends on count and cycles around
		// this corresponds to looping HSV, but I then have to turn it into RGB

		GLfloat HSV[3] = {
			static_cast<GLfloat>(count % 360),
			1.0f,
			1.0f
		},
		RGBA[4];
		RGBA[3] = 1.0f;
		HSV_to_RGB(HSV, RGBA);

		// std::cout << static_cast<float>(count % 360) << std::endl;
		// std::cout << RGBA[0] << " " << RGBA[1] << " " << RGBA[2] << std::endl;

		*p = Particle(pos, 20, accel, RGBA);
		return true;		
	}

	return false;
}

bool centerSpawner(Particle *p, unsigned long int count, void *userData) {
	if (count % 2 == 0) { // once every 2 ticks
		pVec2 center = {500.0f, 750.0f};

		// counter will act as degrees
		double rad = (static_cast<double>(count) / 180) * M_PI;
		// needs to be offset for center to match and then resized
		pVec2 accel = {cos(rad), sin(rad)};

		accel *= 1000000.0;

		GLfloat HSV[3] = {
			static_cast<GLfloat>(count % 360),
			1.0f,
			1.0f
		},
		RGBA[4];
		RGBA[3] = 1.0f;
		HSV_to_RGB(HSV, RGBA);

		// *p = Particle(center, (count % 20) + 5, accel, RGBA);
		*p = Particle(center, 20, accel, RGBA);
		return true;
	}

	return false;
}

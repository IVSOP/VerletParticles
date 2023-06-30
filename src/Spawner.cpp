#include "Spawner.h"
#include <cmath>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Spawner::Spawner(spawnerFunc *func) {
// 	this->count = 0;
// 	this->func = func;
// 	this->info = nullptr;
// }

Spawner::Spawner(size_t start_tick, size_t end_tick, spawnerFunc *func, spawnerInfo *info) {
	this->count = 0;
	this->func = func;
	this->info = info;
	this->start_tick = start_tick;
	this->end_tick = end_tick;
}

Spawner::Spawner(const Spawner &spawner) {
	this->count = spawner.count;
	this->func = spawner.func;
	this->info = spawner.info;
	this->start_tick = spawner.start_tick;
	this->end_tick = spawner.end_tick;
}

// calls function with count and userData. function can choose to use it or not, and by default data is nullptr
// p is a pointer that will be filled with the particle data
// returns true if particle was created (I didn't feel like using std::optional)
bool Spawner::nextParticle(size_t current_tick, Particle *p, unsigned int ID) {
	if (current_tick >= start_tick && current_tick <= end_tick) {
		bool ret = func(p, count, info, ID);
		this->count++;
		return ret;
	}
	return false;
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

bool inCircle(Particle *p, unsigned long int count, spawnerInfo *info, unsigned int ID) {
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

		if (info->color_feed == nullptr) {
			// GLfloat HSV[3] = {
			// 	static_cast<GLfloat>(1 + (count % 359)),
			// 	1.0f,
			// 	1.0f
			// },
			// RGBA[4];
			// RGBA[3] = 1.0f;
			// HSV_to_RGB(HSV, RGBA);

			GLfloat RGBA[4] = {1.0f, 1.0f, 1.0f, 1.0f};

			*p = Particle(pos, info->particle_radius, accel, RGBA);
		} else {
			*p = Particle(pos, info->particle_radius, accel, &(info->color_feed[ID * 4]));
		}
		// color depends on count and cycles around
		// this corresponds to looping HSV, but I then have to turn it into RGB

		return true;		
	}

	return false;
}

bool inCircleReverse(Particle *p, unsigned long int count, spawnerInfo *info, unsigned int ID) {
	if (count % 5 == 0) { // once every 5 ticks
		// double radius = 500.0f;
		// pVec2 center = {500.0f, 500.0f};
		double radius = 500.0f;
		pVec2 center = {500.0f, 500.0f};

		// counter will act as degrees
		double rad = (static_cast<double>(count) / 180) * M_PI;
		// needs to be offset for center to match and then resized
		pVec2 pos = {sin(rad), cos(rad),};
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

		if (info->color_feed == nullptr) {
			// GLfloat HSV[3] = {
			// 	static_cast<GLfloat>(1 + (count % 359)),
			// 	1.0f,
			// 	1.0f
			// },
			// RGBA[4];
			// RGBA[3] = 1.0f;
			// HSV_to_RGB(HSV, RGBA);

			GLfloat RGBA[4] = {1.0f, 1.0f, 1.0f, 1.0f};

			*p = Particle(pos, info->particle_radius, accel, RGBA);
		} else {
			*p = Particle(pos, info->particle_radius, accel, &(info->color_feed[ID * 4]));
		}
		return true;		
	}

	return false;
}

bool centerSpawner(Particle *p, unsigned long int count, spawnerInfo *info, unsigned int ID) {
	if (count % 2 == 0) { // once every 2 ticks
		pVec2 center = info->center;

		// counter will act as degrees
		double rad = (static_cast<double>(count) / 180) * M_PI;
		// needs to be offset for center to match and then resized
		pVec2 accel = {cos(rad), sin(rad)};

		accel *= 1000000.0;

		GLfloat HSV[3] = {
			static_cast<GLfloat>(1 + (count % 359)),
			1.0f,
			1.0f
		},
		RGBA[4];
		RGBA[3] = 1.0f;
		HSV_to_RGB(HSV, RGBA);

		*p = Particle(center, (count % 20) + 5, accel, RGBA);
		return true;
	}

	return false;
}

bool centerSpawnerFixedSize(Particle *p, unsigned long int count, spawnerInfo *info, unsigned int ID) {
	if (count % 2 == 0) { // once every 2 ticks
		pVec2 center = info->center;

		// counter will act as degrees
		double rad = (static_cast<double>(count) / 180) * M_PI;
		// needs to be offset for center to match and then resized
		pVec2 accel = {cos(rad), sin(rad)};

		accel *= 1000000.0;

		if (info->color_feed == nullptr) {
			// GLfloat HSV[3] = {
			// 	static_cast<GLfloat>(1 + (count % 359)),
			// 	1.0f,
			// 	1.0f
			// },
			// RGBA[4];
			// RGBA[3] = 1.0f;
			// HSV_to_RGB(HSV, RGBA);

			GLfloat RGBA[4] = {1.0f, 1.0f, 1.0f, 1.0f};

			*p = Particle(center, info->particle_radius, accel, RGBA);
		} else {
			*p = Particle(center, info->particle_radius, accel, &(info->color_feed[ID * 4]));
		}
		return true;
	}

	return false;
}

#include <stdio.h>

// spawns particles in place and applies acceleration present in info
bool fixedSpawner(Particle *p, unsigned long int count, spawnerInfo *info, unsigned int ID) {
	if (count % 2 == 0) {
		// GLfloat HSV[3] = {
		// 	static_cast<GLfloat>(1 + (count % 359)),
		// 	1.0f,
		// 	1.0f
		// },
		// RGBA[4];
		// RGBA[3] = 1.0f;
		// HSV_to_RGB(HSV, RGBA);
		if (info->color_feed == nullptr) {
			GLfloat RGBA[4] = {1.0f, 1.0f, 1.0f, 1.0f}; // is this needed? wont even get drawn
			*p = Particle(info->center, info->particle_radius, info->accel, RGBA);

		} else {
			*p = Particle(info->center, info->particle_radius, info->accel, &(info->color_feed[ID * 4]));
		}
		return true;
	}
	return false;
}

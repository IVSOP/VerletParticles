#include "Spawner.h"
#include <cmath>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>

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

// returns true if particle was created (I didn't feel like using std::optional)
bool Spawner::nextParticle(size_t current_tick, unsigned int ID, pFloat *cx, pFloat *cy, pFloat *ax, pFloat *ay, pFloat *radius, GLfloat color[4]) {
	if (current_tick >= start_tick && current_tick <= end_tick) {
		bool ret = func(count, info, ID, cx, cy, ax, ay, radius, color);
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

bool inCircle(unsigned long int count, spawnerInfo *info, unsigned int ID, pFloat *cx, pFloat *cy, pFloat *ax, pFloat *ay, pFloat *_radius, GLfloat color[4]) {
	if (count % 5 == 0) { // once every 5 ticks
		pFloat radius = 500.0f;
		constexpr pFloat center_x = 500.0f;
		constexpr pFloat center_y = 500.0f;

		// counter will act as degrees in radians
		pFloat rad = (static_cast<pFloat>(count) / 180) * M_PI;
		// needs to be offset for center to match and then resized
		pFloat pos_x = cos(rad);
		pFloat pos_y = sin(rad);

		pos_x *= radius;
		pos_y *= radius;

		pos_x += center_x;
		pos_y += center_y;

		// move them to middle so they don't clip into wall and get launched
		pFloat offset_x = center_x - pos_x;
		pFloat offset_y = center_y - pos_y;

		// since this is already calculated might as well do some acceleration in the direction of the center
		const pFloat accel_x = offset_x * 3500.0;
		const pFloat accel_y = offset_y * 3500.0;

		offset_x /= 20; // 20 = radius
		offset_y /= 20;

		pos_x += offset_x;
		pos_y += offset_y;

		*cx = pos_x;
		*cy = pos_y;
		*ax = accel_x;
		*ay = accel_y;
		*_radius = info->particle_radius;

		if (info->color_feed == nullptr) {
			// GLfloat HSV[3] = {
			// 	static_cast<GLfloat>(1 + (count % 359)),
			// 	1.0f,
			// 	1.0f
			// },
			// RGBA[4];
			// RGBA[3] = 1.0f;
			// HSV_to_RGB(HSV, RGBA);

			color[0] = 1.0f;
			color[1] = 1.0f;
			color[2] = 1.0f;
			color[3] = 1.0f;

		} else {
			color[0] = info->color_feed[(ID * 4) + 0];
			color[1] = info->color_feed[(ID * 4) + 1];
			color[2] = info->color_feed[(ID * 4) + 2];
			color[3] = info->color_feed[(ID * 4) + 3];
		}
		// color depends on count and cycles around
		// this corresponds to looping HSV, but I then have to turn it into RGB

		return true;		
	}

	return false;
}

bool inCircleReverse(unsigned long int count, spawnerInfo *info, unsigned int ID, pFloat *cx, pFloat *cy, pFloat *ax, pFloat *ay, pFloat *_radius, GLfloat color[4]) {
	if (count % 5 == 0) { // once every 5 ticks
		pFloat radius = 500.0f;
		constexpr pFloat center_x = 500.0f;
		constexpr pFloat center_y = 500.0f;

		// counter will act as degrees
		pFloat rad = (static_cast<pFloat>(count) / 180) * M_PI;
		// needs to be offset for center to match and then resized
		pFloat pos_x = sin(rad);
		pFloat pos_y = cos(rad);

		pos_x *= radius;
		pos_y *= radius;

		pos_x += center_x;
		pos_y += center_y;

		// move them to middle so they don't clip into wall and get launched
		pFloat offset_x = center_x - pos_x;
		pFloat offset_y = center_y - pos_y;

		// since this is already calculated might as well do some acceleration in the direction of the center
		const pFloat accel_x = offset_x * 3500.0;
		const pFloat accel_y = offset_y * 3500.0;

		offset_x /= 20; // 20 = radius
		offset_y /= 20;

		pos_x += offset_x;
		pos_y += offset_y;

		*cx = pos_x;
		*cy = pos_y;
		*ax = accel_x;
		*ay = accel_y;
		*_radius = info->particle_radius;

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

			color[0] = 1.0f;
			color[1] = 1.0f;
			color[2] = 1.0f;
			color[3] = 1.0f;
		} else {
			color[0] = info->color_feed[(ID * 4) + 0];
			color[1] = info->color_feed[(ID * 4) + 1];
			color[2] = info->color_feed[(ID * 4) + 2];
			color[3] = info->color_feed[(ID * 4) + 3];
		}
		return true;		
	}

	return false;
}

bool centerSpawner(unsigned long int count, spawnerInfo *info, unsigned int ID, pFloat *cx, pFloat *cy, pFloat *ax, pFloat *ay, pFloat *_radius, GLfloat color[4]) {
	if (count % 2 == 0) { // once every 2 ticks
		// not needed aparently
		const pFloat center_x = info->cx;
		const pFloat center_y = info->cy;


		// counter will act as degrees
		pFloat rad = (static_cast<pFloat>(count) / 180) * M_PI;
		// needs to be offset for center to match and then resized
		const pFloat accel_x = cos(rad) * 1000000.0;
		const pFloat accel_y = sin(rad) * 1000000.0;

		GLfloat HSV[3] = {
			static_cast<GLfloat>(1 + (count % 359)),
			1.0f,
			1.0f
		},
		RGBA[4];
		RGBA[3] = 1.0f;
		HSV_to_RGB(HSV, RGBA);

		*cx = center_x;
		*cy = center_y;
		*ax = accel_x;
		*ay = accel_y;
		*_radius = (count % 20) + 5;

		color[0] = RGBA[0];
		color[1] = RGBA[1];
		color[2] = RGBA[2];
		color[3] = RGBA[3];

		return true;
	}

	return false;
}

bool centerSpawnerFixedSize(unsigned long int count, spawnerInfo *info, unsigned int ID, pFloat *cx, pFloat *cy, pFloat *ax, pFloat *ay, pFloat *_radius, GLfloat color[4]) {
	if (count % 2 == 0) { // once every 2 ticks
		// not needed aparently
		const pFloat center_x = info->cx;
		const pFloat center_y = info->cy;

		// counter will act as degrees
		pFloat rad = (static_cast<pFloat>(count) / 180) * M_PI;
		// needs to be offset for center to match and then resized
		const pFloat accel_x = cos(rad) * 1000000.0;
		const pFloat accel_y = sin(rad) * 1000000.0;

		*cx = center_x;
		*cy = center_y;
		*ax = accel_x;
		*ay = accel_y;
		*_radius = info->particle_radius;

		if (info->color_feed == nullptr) {
			// GLfloat HSV[3] = {
			// 	static_cast<GLfloat>(1 + (count % 359)),
			// 	1.0f,
			// 	1.0f
			// },
			// RGBA[4];
			// RGBA[3] = 1.0f;
			// HSV_to_RGB(HSV, RGBA);

			color[0] = 1.0f;
			color[1] = 1.0f;
			color[2] = 1.0f;
			color[3] = 1.0f;
		} else {
			color[0] = info->color_feed[(ID * 4) + 0];
			color[1] = info->color_feed[(ID * 4) + 1];
			color[2] = info->color_feed[(ID * 4) + 2];
			color[3] = info->color_feed[(ID * 4) + 3];
		}
		return true;
	}

	return false;
}

// spawns particles in place and applies acceleration present in info
bool fixedSpawner(unsigned long int count, spawnerInfo *info, unsigned int ID, pFloat *cx, pFloat *cy, pFloat *ax, pFloat *ay, pFloat *_radius, GLfloat color[4]) {
	if (count % 2 == 0) {
		// GLfloat HSV[3] = {
		// 	static_cast<GLfloat>(1 + (count % 359)),
		// 	1.0f,
		// 	1.0f
		// },
		// RGBA[4];
		// RGBA[3] = 1.0f;
		// HSV_to_RGB(HSV, RGBA);

		*cx = info->cx;
		*cy = info->cy;
		*ax = info->ax;
		*ay = info->ay;
		*_radius = info->particle_radius;

		if (info->color_feed == nullptr) {
			// is this needed? wont even get drawn
			color[0] = 1.0f;
			color[1] = 1.0f;
			color[2] = 1.0f;
			color[3] = 1.0f;

		} else {
			color[0] = info->color_feed[(ID * 4) + 0];
			color[1] = info->color_feed[(ID * 4) + 1];
			color[2] = info->color_feed[(ID * 4) + 2];
			color[3] = info->color_feed[(ID * 4) + 3];
		}
		return true;
	}
	return false;
}

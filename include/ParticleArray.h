#ifndef PARTICLEARRAY_H
#define PARTICLEARRAY_H

#include "common.h"
#include <cstring>

struct ParticleArray {
	// does restrict here even do anything??????????????????????????????????
	pFloat * __restrict__ current_x; 
	pFloat * __restrict__ current_y;
	pFloat * __restrict__ old_x;
	pFloat * __restrict__ old_y;
	// size_t *radius;
	pFloat * __restrict__ accel_x;
	pFloat * __restrict__ accel_y;
	GLfloat * __restrict__ color;

	// len is not recorded but used for alocations
	ParticleArray(size_t max_len);

	~ParticleArray();

	void updateParticlePosition(size_t ID, pFloat dt);
	void accelerate(size_t ID, pFloat x, pFloat y);
};

#endif

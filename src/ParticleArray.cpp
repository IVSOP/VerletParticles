#include "ParticleArray.h"

#include <stdlib.h>

ParticleArray::ParticleArray(size_t max_len) {
	current_x = (pFloat *)aligned_alloc(8 * sizeof(pFloat), max_len * sizeof(pFloat));
	current_y = (pFloat *)aligned_alloc(8 * sizeof(pFloat), max_len * sizeof(pFloat));

	old_x = (pFloat *)aligned_alloc(8 * sizeof(pFloat), max_len * sizeof(pFloat)); // does this need to be memset to 0???
	old_y = (pFloat *)aligned_alloc(8 * sizeof(pFloat), max_len * sizeof(pFloat)); // does this need to be memset to 0???

	// radius = (size_t *)malloc(max_len * sizeof(size_t));
	accel_x = (pFloat *)aligned_alloc(8 * sizeof(pFloat), max_len * sizeof(pFloat));
	accel_y = (pFloat *)aligned_alloc(8 * sizeof(pFloat), max_len * sizeof(pFloat));
	// memset(accel_x, 0, max_len * sizeof(pFloat));
	// memset(accel_y, 0, max_len * sizeof(pFloat));

	color = (GLfloat *)malloc(max_len * sizeof(GLfloat) * 4);
}

ParticleArray::~ParticleArray() {
	free(current_x);
	free(current_y);
	free(old_x);
	free(old_y);
	// free(radius);
	free(accel_x);
	free(accel_y);
	free(color);
}

void ParticleArray::updateParticlePosition(size_t ID, pFloat dt) {
	const pFloat velocity_x = current_x[ID] - old_x[ID];
	const pFloat velocity_y = current_y[ID] - old_y[ID];

	old_x[ID] = current_x[ID];
	old_y[ID] = current_y[ID];

	current_x[ID] += velocity_x + (accel_x[ID] * dt * dt);
	current_y[ID] += velocity_y + (accel_y[ID] * dt * dt);

	accel_x[ID] = 0;
	accel_y[ID] = 0;
}

void ParticleArray::accelerate(size_t ID, pFloat x, pFloat y) {
	this->accel_x[ID] += x;
	this->accel_y[ID] += y;
}

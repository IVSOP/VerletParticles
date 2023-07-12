#include "ParticleArray.h"

#include <stdlib.h>

ParticleArray::ParticleArray(size_t max_len) {
	current_x = (double *)malloc(max_len * sizeof(double));
	current_y = (double *)malloc(max_len * sizeof(double));
	old_x = (double *)calloc(max_len,  sizeof(double));
	old_y = (double *)calloc(max_len,  sizeof(double));
	// radius = (size_t *)malloc(max_len * sizeof(size_t));
	accel_x = (double *)calloc(max_len, sizeof(double));
	accel_y = (double *)calloc(max_len, sizeof(double));
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

void ParticleArray::updateParticlePosition(size_t ID, double dt) {
	const double velocity_x = current_x[ID] - old_x[ID];
	const double velocity_y = current_y[ID] - old_y[ID];

	old_x[ID] = current_x[ID];
	old_y[ID] = current_y[ID];

	current_x[ID] += velocity_x + (accel_x[ID] * dt * dt);
	current_y[ID] += velocity_y + (accel_y[ID] * dt * dt);

	accel_x[ID] = 0;
	accel_y[ID] = 0;
}

void ParticleArray::accelerate(size_t ID, double x, double y) {
	this->accel_x[ID] += x;
	this->accel_y[ID] += y;
}

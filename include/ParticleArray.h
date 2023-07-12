#ifndef PARTICLEARRAY_H
#define PARTICLEARRAY_H

// for GLfloat, need to rework this
#include <GL/glew.h>
#include <cstring>

struct ParticleArray {
	double *current_x;
	double *current_y;
	double *old_x;
	double *old_y;
	// size_t *radius;
	double *accel_x;
	double *accel_y;
	GLfloat *color;

	// len is not recorded but used for alocations
	ParticleArray(size_t max_len);

	~ParticleArray();

	void updateParticlePosition(size_t ID, double dt);
	void accelerate(size_t ID, double x, double y);
};

#endif

#include "Spawner.h"
#include <cmath>

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

bool inCircle(Particle *p, unsigned long int count, void *userData) {
	double radius = 500.0f;
	pVec2 center = {500.0f, 500.0f};
	
	GLfloat color[4] = {1.0f, 0.0f, 0.0f, 1.0f};
	// counter will act as degrees
	double rad = (static_cast<double>(count) / 180) * M_PI;
	// needs to be offset for center to match and then resized
	pVec2 pos = {cos(rad), sin(rad)};
	pos *= radius;
	pos += center;
	// std::cout << pos.x << "," << pos.y << std::endl;

	*p = Particle(pos, 20, color);
	return true;
}

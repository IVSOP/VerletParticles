#include "Particle.h"

Particle::Particle(pVec2 current_pos, size_t radius, pVec2 accel)
{
	this->current_pos = current_pos;
	this->old_pos = current_pos;
	this->accel = accel;
	this->radius = radius;
}

Particle::Particle(pVec2 current_pos, size_t radius)
{
	this->current_pos = current_pos;
	this->old_pos = current_pos;
	this->radius = radius;
	this->accel = {0, 0};
}

// wont use memcpy so compiler can optimize whatever it wants
Particle::Particle(const Particle &particle) {
	this->current_pos = particle.current_pos;
	this->old_pos = particle.old_pos;
	this->accel = particle.accel;
	this->radius = particle.radius;
}
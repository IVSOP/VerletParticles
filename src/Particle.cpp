#include "Particle.h"

Particle::Particle(vec2<ParticleData> current_pos, ParticleData radius, vec2<ParticleData> accel)
{
	this->current_pos = current_pos;
	this->accel = accel;
	this->radius = radius;
}

Particle::Particle(vec2<ParticleData> current_pos, ParticleData radius)
{
	this->current_pos = current_pos;
	this->radius = radius;
}

// wont use memcpy so compiler can optimize whatever it wants
Particle::Particle(const Particle &particle) {
	this->current_pos = particle.current_pos;
	this->accel = particle.accel;
	this->radius = particle.radius;
}
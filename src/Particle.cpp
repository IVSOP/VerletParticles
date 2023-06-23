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

void Particle::updatePosition(double dt) {
	glm::dvec2  velocity = current_pos - old_pos; // const???

	// std::cout << "Velocity: " << velocity.x << " " << velocity.y << std::endl;
	// std::cout << "Accel: " << accel.x << " " << accel.y << std::endl;

	this->old_pos = current_pos;
	// std::cout << "Before: " << current_pos.x << " " << current_pos.y << std::endl;
	
	this->current_pos = current_pos + velocity + (accel * dt * dt);

	// std::cout << "After: " << current_pos.x << " " << current_pos.y << std::endl;
	// std::cout << "Accel was " << accel.x << " " << accel.y << std::endl;

	this->accel = {0, 0};
}

void Particle::accelerate(pVec2 accel) {
	this->accel += accel;
}
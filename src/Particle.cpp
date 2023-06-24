#include "Particle.h"

Particle::Particle(pVec2 current_pos, size_t radius, pVec2 accel)
{
	this->current_pos = current_pos;
	this->old_pos = current_pos;
	this->accel = accel;
	this->radius = radius;
	GLfloat new_color[] = {10.0f, 1.0f, 1.0f, 1.0f };
	std::memcpy(this->color, new_color, 4 * sizeof(GLfloat));
}

Particle::Particle(pVec2 current_pos, size_t radius, pVec2 accel, GLfloat new_color[4])
{
	this->current_pos = current_pos;
	this->old_pos = current_pos;
	this->accel = accel;
	this->radius = radius;
	std::memcpy(this->color, new_color, 4 * sizeof(GLfloat));
}

Particle::Particle(pVec2 current_pos, size_t radius)
{
	this->current_pos = current_pos;
	this->old_pos = current_pos;
	this->radius = radius;
	this->accel = {0, 0};
	GLfloat new_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	std::memcpy(this->color, new_color, 4 * sizeof(GLfloat));
}

Particle::Particle(pVec2 current_pos, size_t radius, GLfloat new_color[4])
{
	this->current_pos = current_pos;
	this->old_pos = current_pos;
	this->radius = radius;
	this->accel = {0, 0};
	std::memcpy(this->color, new_color, 4 * sizeof(GLfloat));

}

// wont use memcpy so compiler can optimize whatever it wants
Particle::Particle(const Particle &particle) {
	this->current_pos = particle.current_pos;
	this->old_pos = particle.old_pos;
	this->accel = particle.accel;
	this->radius = particle.radius;
	memcpy(this->color, particle.color, 4 * sizeof(GLfloat));
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
#include "RegularSandbox.h"

#include <glm/geometric.hpp>

RegularSandbox::RegularSandbox(size_t max_particles, size_t pixelsX, size_t pixelsY)
	: Sandbox(max_particles, pixelsX, pixelsY)
	{
}

void RegularSandbox::addParticle(Particle &particle) {
	particles[len_particles ++] = particle; // will this copy the struct???
}

void RegularSandbox::updatePositions(double dt) {
	size_t i;

	for (i = 0; i < len_particles; i++) {
		particles[i].updatePosition(dt);
	}
}

void RegularSandbox::applyCircleConstraint() {
	const pVec2 center = {500.0f, 500.0f};
	const double radius = 500.0f;

	pVec2 to_center, n;
	Particle *particle;
	double dist_to_center;
	size_t i;

	for (i = 0; i < this->len_particles; i++) {
		particle = &(this->particles[i]);
		to_center = particle->current_pos - center;
		dist_to_center = glm::length(to_center);
		// check if particle is clipping constraint bounds
		if (dist_to_center > radius - particle->radius) {
			n = to_center / dist_to_center;
			particle->current_pos = center + (n * (radius - particle->radius));
		}
	}
}

void RegularSandbox::applyRectangleConstraint() {
	Particle *p; size_t i;
	const double size_x = 1000.0f / 2.0f; // divide by 2 here to save time
	const double size_y = 1000.0f / 2.0f;
	// const double margin = 2.0f;
	pVec2 center = {500.0f, 500.0f};
	double radius;

	// this is worse than the circle one
	for (i = 0; i < this->len_particles; i++) {
		p = &(this->particles[i]);
		radius = p->radius;

		if (p->current_pos.x + radius > center.x + size_x) {
			p->current_pos.x = center.x + size_x - radius;
		} else if (p->current_pos.x - radius < center.x - size_x) {
			p->current_pos.x = center.x - size_x + radius;
		}

		if (p->current_pos.y + radius > center.y + size_y) {
			p->current_pos.y = center.y + size_y - radius;
		} else if (p->current_pos.y - radius < center.y - size_y) {
			p->current_pos.y = center.y - size_y + radius;
		}
	}
}

// brute force approach
// I have no idea how the math involved works
void RegularSandbox::solveCollisions() {
	size_t i, j;
	Particle *p1, *p2;

	for (i = 0; i < this->len_particles; i++) {
		p1 = &(this->particles[i]);
		for (j = i + 1; j < this->len_particles; j++) {
			p2 = &(this->particles[j]);
			collideParticles(p1, p2);
		}
	}
}

void RegularSandbox::collideParticles(Particle *p1, Particle *p2) {
	pVec2 collisionAxis, n;
	double dist, min_dist;
	const double response_coef = 0.75f;
	double mass_ratio_1, mass_ratio_2, delta;
	double p1_radius, p2_radius;

	p1_radius = p1->radius;
	p2_radius = p2->radius;

	collisionAxis = p1->current_pos - p2->current_pos;
	min_dist = p1_radius + p2_radius;
	dist = (collisionAxis.x * collisionAxis.x) + (collisionAxis.y * collisionAxis.y);
	// avoid srqt as long as possible

	
	if (dist < min_dist * min_dist) {
		dist = sqrt(dist);
		n = collisionAxis / dist;
		
		mass_ratio_1 = p1_radius / (p1_radius + p2_radius);
		mass_ratio_2 = p2_radius / (p1_radius + p2_radius);

		delta = 0.5f * response_coef * (dist - min_dist);

		p1->current_pos -= n * (mass_ratio_2 * delta);
		p2->current_pos += n * (mass_ratio_1 * delta);
	}
}

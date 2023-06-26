#include "SortedSandbox.h"

#include <glm/geometric.hpp>
#include<algorithm>

SortedSandbox::SortedSandbox(size_t max_particles, size_t pixelsX, size_t pixelsY)
	: Sandbox(max_particles, pixelsX, pixelsY)
	{
	this->arr_x = new Particle*[getMaxNumberParticles()]; len_x = 0;
	// this->arr_y = new Particle*[getMaxNumberParticles()]; len_y = 0;
}

SortedSandbox::~SortedSandbox() {
	delete[] arr_x;
	// delete[] arr_y;
}

void SortedSandbox::addParticle(Particle &particle) {
	particles[len_particles] = particle; // will this copy the struct???

	arr_x[len_x++] = particles + len_particles;
	// arr_y[len_y++] = particles + len_particles;

	addColorToParticle(len_particles, particle.color);

	len_particles++;
}

void SortedSandbox::updatePositions(double dt) {
	size_t i;

	for (i = 0; i < len_particles; i++) {
		particles[i].updatePosition(dt);
	}
}

void SortedSandbox::applyCircleConstraint() {
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

void SortedSandbox::applyRectangleConstraint() {
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

// approach here is different since I need to get distance between particles anyway
// check nearest along x axis and y axis
// for now they look at the front but not back (they assume everything at the back was already treated in previous calls)
void SortedSandbox::solveCollisions() {
	size_t i, j;
	Particle *p1, *p2;
	double radius_p1, radius_p2;

	sortArrays();

	// if (len_particles > 0)
	// 	printf("particle 1 is in (%f,%f)\n", particles[1].current_pos.x, particles[1].current_pos.y);
	

	// will optimize how this gets done in the future, prob with a bitmap or something

	// colliding along x axis
	for (i = 0; i < len_x; i++) {
		p1 = arr_x[i];
		radius_p1 = p1->radius;
		for (j = i + 1; j < len_x; j++) {
			p2 = arr_x[j];
			radius_p2 = p2->radius;

			// check if particles COULD be colliding according to X axis
			if (collisionIsPossibleX(p1, p2, radius_p1, radius_p2) == true) {
				collideParticles(p1, p2);
			}
			else break;
		}
	}

	// // colliding along y axis
	// for (i = 0; i < len_y; i++) {
	// 	p1 = arr_y[i];
	// 	radius_p1 = p1->radius;
	// 	for (j = i + 1; j < len_y; j++) {
	// 		p2 = arr_y[j];
	// 		radius_p2 = p2->radius;

	// 		// check if particles COULD be colliding according to Y axis
	// 		if (! collisionIsPossibleY(p1, p2, radius_p1, radius_p2)) {
	// 			break;
	// 		}
	// 		collideParticles(p1, p2);
	// 	}
	// }
}

bool SortedSandbox::collisionIsPossibleX(const Particle *p1, const Particle *p2, double radius_p1, double radius_p2) {
	double sum_radius = radius_p1 + radius_p2;
	double distance_x = p1->current_pos.x - p2->current_pos.x;

	return (abs(distance_x) < sum_radius);
}

// bool SortedSandbox::collisionIsPossibleY(const Particle *p1, const Particle *p2, double radius_p1, double radius_p2) {
// 	double sum_radius = radius_p1 + radius_p2;
// 	double distance_y = p1->current_pos.y - p2->current_pos.y;

// 	if (distance_y < sum_radius) { // collision is possible
// 		return true;
// 	}
// 	return false;
// }

void SortedSandbox::collideParticles(Particle *p1, Particle *p2) {
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
		// printf("colliding particles %ld and %ld\n", p1 - particles, p2 - particles);
		
		dist = sqrt(dist);
		n = collisionAxis / dist;
		
		mass_ratio_1 = p1_radius / (p1_radius + p2_radius);
		mass_ratio_2 = p2_radius / (p1_radius + p2_radius);

		delta = 0.5f * response_coef * (dist - min_dist);

		p1->current_pos -= n * (mass_ratio_2 * delta);
		p2->current_pos += n * (mass_ratio_1 * delta);
	}
}

inline bool cmpParticlesX(const Particle *p1, const Particle *p2) {
	return (p1->current_pos.x < p2->current_pos.x);
}

inline bool cmpParticlesY(const Particle *p1, const Particle *p2) {
	return (p1->current_pos.y < p2->current_pos.y);
}

void SortedSandbox::sortArrays() {
	std::sort(arr_x, arr_x + len_x, cmpParticlesX);
	// std::sort(arr_y, arr_y + len_y, cmpParticlesY);
}

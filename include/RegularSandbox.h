#ifndef REGULARSANDBOX_H
#define REGULARSANDBOX_H

#include "Sandbox.h"

class RegularSandbox : public Sandbox {

	private:
		// Particle *particles; ???????????
		// size_t len_particles;

	public:
		RegularSandbox() = delete;
		RegularSandbox(size_t n_particles, size_t pixelsX, size_t pixelsY);
		~RegularSandbox() = default;

		void addParticle(Particle &particle);

		void updatePositions(double dt);
		// void applyGravity();
		void applyCircleConstraint();
		void applyRectangleConstraint();
		void solveCollisions();

		void collideParticles(Particle *p1, Particle *p2);

};

#endif

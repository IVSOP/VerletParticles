#ifndef SORTEDSANDBOX_H
#define SORTEDSANDBOX_H

#include "Sandbox.h"

class SortedSandbox : public Sandbox {

	private:
		// Particle *particles; ???????????
		// size_t len_particles;

		// particles are sorted according to their position in the grid of pixels
		// this is at best Nlog(N) (sorting) + lookup neighboring cells, better than O^2 for a large ammount of particles
		uint32_t *indices;

	public:
		SortedSandbox() = delete;
		SortedSandbox(size_t n_particles, size_t pixelsX, size_t pixelsY);
		~SortedSandbox();

		void addParticle(Particle &particle);

		void updatePositions(double dt);
		// void applyGravity();
		void applyCircleConstraint();
		void applyRectangleConstraint();
		void solveCollisions();

		void collideParticles(Particle *p1, Particle *p2);

};

#endif

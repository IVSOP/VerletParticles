#ifndef SORTEDSANDBOX_H
#define SORTEDSANDBOX_H

#include "Sandbox.h"

// sweep and prune strategy
// sort particles according to X axis and Y axis separately
// check for collisions in both

// bool cmp(const Type1 &a, const Type2 &b);

class SortedSandbox : public Sandbox {

	private:
		// had to use particle * because comparison functions could not acces particles due to not being part of the class
		Particle **arr_x; size_t len_x;
		Particle **arr_y; size_t len_y;

		// this is messy because functions need to get access to particles
		void sortArrays();

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

#ifndef SORTEDSANDBOX_H
#define SORTEDSANDBOX_H

#include "Sandbox.h"

#include <iostream>

// sweep and prune strategy
// sort particles according to X axis and Y axis separately
// check for collisions in both

// bool cmp(const Type1 &a, const Type2 &b);


// IMPORTANT: for now, this works great for particles of the same size
// however, when they have different sizes, the loop that checks collisions
// stops suddenly when the collision is not possible with a small particle,
// while it should still be possible if the particle was bigger
// I am trying to think of a way to fix this, but if the particles have the same size it should be pretty good

class SortedSandbox : public Sandbox {

	private:
		// had to use particle * because comparison functions could not acces particles due to not being part of the class
		Particle **arr_x; size_t len_x;
		// Particle **arr_y; size_t len_y;

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
		// these will return true if there was a collision
		// but they will also solve the collision!!!!!!!!!!!!!!!!!!!!
		bool collisionIsPossibleX(const Particle *p1, const Particle *p2, double radius_p1, double radius_p2);
		// bool collisionIsPossibleY(const Particle *p1, const Particle *p2, double radius_p1, double radius_p2);

};

#endif

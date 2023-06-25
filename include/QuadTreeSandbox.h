#ifndef QUADTREESANDBOX_H
#define QUADTREESANDBOX_H

#include "Sandbox.h"
#include "QuadTree.h"

class QuadTreeSandbox : public Sandbox {

	private:
		// Particle *particles; ???????????
		// size_t len_particles;

		QuadTree tree;

	public:
		QuadTreeSandbox() = delete;
		QuadTreeSandbox(size_t n_particles, size_t pixelsX, size_t pixelsY);
		~QuadTreeSandbox() = default;

		void addParticle(Particle &particle);

		void updatePositions(double dt);
		// void applyGravity();
		void applyCircleConstraint();
		void applyRectangleConstraint();
		void solveCollisions();

};

void collideParticles(Particle *p1, Particle *p2);

#endif

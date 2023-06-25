#ifndef GRIDSANDBOX_H
#define GRIDSANDBOX_H

#include "Grid.h"
#include "Sandbox.h"

class GridSandbox : public Sandbox {

	private:
		// Particle *particles; ???????????
		// size_t len_particles;

		Grid grid;

	public:
		GridSandbox() = delete;
		GridSandbox(size_t n_particles, size_t pixelsX, size_t pixelsY);
		~GridSandbox() = default;

		void addParticle(Particle &particle);

		void updatePositions(double dt);
		// void applyGravity();
		void applyCircleConstraint();
		void applyRectangleConstraint();
		void solveCollisions();

		void collideParticlesGrid(GridCell *centerCell, GridCell *secondCell);
};

#endif

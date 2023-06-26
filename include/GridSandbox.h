#ifndef GRIDSANDBOX_H
#define GRIDSANDBOX_H

#include "Grid.h"
#include "Sandbox.h"

#define GRID_PARTICLE_SIZE 20

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

		void collideParticlesBetweenCells(GridCell *centerCell, GridCell *secondCell);
		void collideParticlesSameCell(GridCell *cell);
		void collideParticles(Particle *p1, Particle *p2);
};

#endif

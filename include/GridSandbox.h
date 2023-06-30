#ifndef GRIDSANDBOX_H
#define GRIDSANDBOX_H

#include "Grid.h"
#include "Sandbox.h"

#define GRID_PARTICLE_SIZE 10

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
		void collideParticlesBetweenCellsV2(GridCell *centerCell, size_t row, size_t col);
		void collideParticlesSameCell(GridCell *cell);
		void collideParticles(Particle *p1, Particle *p2);

		void dumpGridToFile();

		GLfloat *convert_png(const char *path);

		void clear();

		GLfloat *parseColorsByGrid(GLfloat *colors);
};

#endif

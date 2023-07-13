#ifndef GRIDSANDBOX_H
#define GRIDSANDBOX_H

#include "Grid.h"
#include "Sandbox.h"

#include "thpool.h"
#define MAX_THREADS 8


class GridSandbox : public Sandbox {

	private:
		// Particle *particles; ???????????
		// size_t len_particles;

		threadpool thpool;

	public:
		Grid grid; // temporary
		GridSandbox() = delete;
		GridSandbox(size_t n_particles, size_t pixelsX, size_t pixelsY);
		~GridSandbox();

		void addParticle(pFloat cx, pFloat cy, pFloat ax, pFloat ay, GLfloat color[4]);

		void updatePositions(pFloat dt);
		// void applyGravity();
		// void applyCircleConstraint();
		// void applyRectangleConstraint();
		void solveCollisions();

		void collideParticlesBetweenCells(GridCell *centerCell, GridCell *secondCell);
		void collideParticlesBetweenCellsV2(GridCell *centerCell, size_t row, size_t col);
		void collideParticlesSameCell(GridCell *cell);
		void collideParticles(size_t ID_A, size_t ID_B);

		void dumpGridToFile();

		GLfloat *convert_png(const char *path);

		void clear();

		GLfloat *parseColorsByGrid(GLfloat *colors);

		void insertIntoGrid();
};

struct Args {
	size_t row_start, row_end;
	GridSandbox *sandbox;
};

struct UpdateArgs {
	size_t start, end;
	ParticleArray *particles;
	pFloat dt;
	size_t pixelsX, pixelsY;
	Grid *grid;

	UpdateArgs(size_t _start, size_t _end, ParticleArray *_particles, pFloat _dt, size_t _pixelsX, size_t _pixelsY, Grid *_grid)
	: start(_start), end(_end), particles(_particles), dt(_dt), pixelsX(_pixelsX), pixelsY(_pixelsY), grid(_grid)
	{}
};

void collideParticlesFromTo(void *args);
void updatePositionsThread(void *args);

#endif

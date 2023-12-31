#include "Grid.h"

#include <stdlib.h>
#include <stdio.h>

#include <string>

#define GRID_DEBUG

Grid::Grid(size_t pixel_width, size_t pixel_height, size_t particle_radius) {
	// have to divide or multiply by 2, radius of X means diameter is 2X
	cols = pixel_width / (2 * particle_radius);
	rows = pixel_height / (2 * particle_radius);
	size = rows * cols;
	cells = new GridCell[size];
	size_t i;

	for (i = 0; i < size; i++) {
		(&cells[i])->len_particles = 0;
	}

	this->square_diameter = particle_radius * 2;

	// extra division (1 /...) but allows for every insert to only have to multiply by this
	inverse_square_diameter = 1.0 / square_diameter;

	if (pixel_width % (2 * particle_radius) != 0 || pixel_height % (2 * particle_radius) != 0) {
		printf("Cannot correctly divide grid\n");
		exit(5);
	}

	printf("Creating grid. pixelsX:%ld pixelsY:%ld square_diameter:%f cols:%ld rows:%ld\n", pixel_width, pixel_height, square_diameter, rows, cols);	
}

// removes a given index, shifting remaining array
// if removed anything returns true
// only removes one, assumes repetitions are not possible
bool removeFromArr(size_t particleIndex, size_t arr[GRID_CELL_CAPACITY]) {
	int i;
	for (i = 0; i < GRID_CELL_CAPACITY; i++) {
		if (arr[i] == particleIndex) {
			// shift remaining array
			while (i < GRID_CELL_CAPACITY - 1) {
				arr[i] = arr[i + 1];
				i++;
			}
			return true;
		}
	}
	return false;
}

void Grid::insertIntoGrid(size_t particleIndex, size_t row, size_t col) {
	GridCell *cell = get(row, col);

#ifdef GRID_DEBUG
	if (cell->len_particles == GRID_CELL_CAPACITY) {
		fprintf(stderr, "Too many particles in %ld, %ld\n", row, col);
		exit(5);
	}
#endif

	cell->particle_idx[cell->len_particles ++] = particleIndex;
}

void Grid::insertIntoGrid(size_t particleIndex, size_t pos) {
	GridCell *cell = get(pos);

#ifdef GRID_DEBUG
	if (cell->len_particles == GRID_CELL_CAPACITY) {
		fprintf(stderr, "Too many particles in %ld\n", pos);
		exit(5);
	}
#endif
	cell->particle_idx[cell->len_particles ++] = particleIndex;
}

void Grid::insertIntoGrid(size_t particleIndex, GridCell *cell) {

#ifdef GRID_DEBUG
	if (cell->len_particles == GRID_CELL_CAPACITY) {
		// fprintf(stderr, "Too many particles in %ld, %ld\n", row, col);
		exit(5);
	}
#endif

	cell->particle_idx[cell->len_particles ++] = particleIndex;
}

void Grid::insertIntoGrid(size_t particleIndex, double pos_x, double pos_y) {
	size_t i = getGridIndexFromParticlePos(pos_x, pos_y);
	GridCell *cell = &(cells[i]);

#ifdef GRID_DEBUG
	if (cell->len_particles == GRID_CELL_CAPACITY) {
		fprintf(stderr, "Too many particles when adding particle in %f %f\n", pos_x, pos_y);
		exit(5);
	}
	
#endif
	// printf("Inserting into grid at %ld, from (%f, %f) which corresponds to (%ld,%ld)\n", i, particlePos.x, particlePos.y, i / cols, i % cols);
	cell->particle_idx[cell->len_particles ++] = particleIndex;
}

// to avoid overhead, insert many at once from an array
void Grid::insertIntoGrid(size_t start, size_t end, ParticleArray *particles) {
	GridCell *cell;
	double x, y;

	for (; start < end; start ++) {			
		x = particles->current_x[start];
		y = particles->current_y[start];
		cell = &(cells[ getGridIndexFromParticlePos(x, y) ]);

#ifdef GRID_DEBUG
		if (cell->len_particles == GRID_CELL_CAPACITY) {
			fprintf(stderr, "Too many particles in %lf, %lf\n", x, y);
			exit(5);
		}
#endif

		cell->particle_idx[cell->len_particles ++] = start;
	}
}

void Grid::removeFromGrid(size_t particleIndex, size_t row, size_t col) {
	GridCell *cell = get(row, col);
	if (removeFromArr(particleIndex, cell->particle_idx)) {
		cell->len_particles --;
	}
}

void Grid::removeFromGrid(size_t particleIndex, size_t pos) {
	GridCell *cell = get(pos);
	if (removeFromArr(particleIndex, cell->particle_idx)) {
		cell->len_particles --;
	}
}

void Grid::removeFromGrid(size_t particleIndex, GridCell *cell) {
	if (removeFromArr(particleIndex, cell->particle_idx)) {
		cell->len_particles --;
	}
}

void Grid::removeFromGrid(size_t particleIndex, double pos_x, double pos_y) {
	size_t i = getGridIndexFromParticlePos(pos_x, pos_y);
	GridCell *cell = &(cells[i]);
	if (removeFromArr(particleIndex, cell->particle_idx)) {
		cell->len_particles --;
	}
}

void Grid::move(size_t particleIndex, size_t old_row, size_t old_col, size_t new_row, size_t new_col) {

}

void Grid::move(size_t particleIndex, size_t old_pos, size_t new_pos) {

}

// checks if difference from old_pos to current_pos is enought to change to another cell and returns index of the new cell if found
// bool Grid::particleChangedCells(Particle *p, size_t *old_pos, size_t *new_pos) {
// 	size_t pos1, pos2;

// 	pos1 = getGridIndexFromParticlePos(p->old_pos);
// 	pos2 = getGridIndexFromParticlePos(p->current_pos);

// 	if (pos1 != pos2) {
// 		*old_pos = pos1;
// 		*new_pos = pos1;
// 		return true;
// 	}
// 	return false;
// }

#ifndef GRID_H
#define GRID_H

#include "Particle.h"

#define GRID_CELL_CAPACITY 1

typedef struct {
	size_t particle_idx[GRID_CELL_CAPACITY];
	uint8_t len_particles;
} GridCell;

// VERY IMPORTANT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// grid is a matrix but allocated as an array
// since the particle coordinates are (0,0) at bottom left,
// the matrix is left upside down, left to right but bottom to top
// [0] is (0,0) and corresponds to bottom left
struct Grid {
		GridCell *cells;
		size_t rows, cols, size;
		// so that to transform into grid coordinates all you need to do is multiply by this
		pVec2 transform;

		Grid() = delete;
		Grid(size_t pixel_width, size_t pixel_height, unsigned min_radius) {
			cols = pixel_width / min_radius;
			rows = pixel_height / min_radius;
			size = rows * cols;
			cells = new GridCell[size];
			size_t i;
			for (i = 0; i < size; i++) {
				cells[i].len_particles = 0;
			}

			transform = {
				pixel_width / cols,
				pixel_height / rows
			};
		}

		~Grid() {
			delete[] this->cells;
		}

		void insertIntoGrid(size_t particleIndex, size_t row, size_t col);
		void removeFromGrid(size_t particleIndex, size_t row, size_t col);
		void insertIntoGrid(size_t particleIndex, size_t pos);
		void removeFromGrid(size_t particleIndex, size_t pos);
		void move(size_t particleIndex, size_t old_row, size_t old_col, size_t new_row, size_t new_col);
		void move(size_t particleIndex, size_t old_pos, size_t new_pos);

		inline size_t getGridIndexFromParticlePos(pVec2 pos) {
			return (static_cast<size_t>(pos.x) * rows) + static_cast<size_t>(pos.y);
		}

		inline size_t getGridIndexFromRowCol(size_t row, size_t col) {
			return (row * cols) + col;
		}

		inline GridCell *get(size_t row, size_t col) {
			return &(this->cells[getGridIndexFromRowCol(row, col)]);
		}

		inline GridCell *get(size_t pos) {
			return &(this->cells[pos]);
		}
};

#endif

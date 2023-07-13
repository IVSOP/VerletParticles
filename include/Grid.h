#ifndef GRID_H
#define GRID_H

#include "ParticleArray.h"

#define GRID_CELL_CAPACITY 4

typedef struct {
	size_t particle_idx[GRID_CELL_CAPACITY];
	uint8_t len_particles;
} GridCell;

// VERY IMPORTANT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// grid is a matrix but allocated as an array
// since the particle coordinates are (0,0) at bottom left,
// the matrix is upside down, left to right but bottom to top
// [0] is (0,0) and corresponds to bottom left

// basically, row 0 is the floor

// particle radius is fixed!!!!!!!!!
struct Grid {
	public:
		GridCell *cells;
		size_t rows, cols, size;

		// some things assume square, others rectangle, for now I just want this to work
		double square_diameter;
		// so that to transform into grid coordinates all you need to do is multiply by this
		double inverse_square_diameter;

		Grid() = delete;
		Grid(size_t pixel_width, size_t pixel_height, size_t radius);

		~Grid() {
			delete[] this->cells;
		}

		void clear() {
			size_t i;
			for (i = 0; i < size; i++) {
				cells[i].len_particles = 0;
			}
		}

		void insertIntoGrid(size_t particleIndex, size_t row, size_t col);
		void insertIntoGrid(size_t particleIndex, size_t pos);
		void insertIntoGrid(size_t particleIndex, GridCell *cell);
		void insertIntoGrid(size_t particleIndex, double pos_x, double pos_y);
		void insertIntoGrid(size_t start, size_t end, ParticleArray *particles);

		void removeFromGrid(size_t particleIndex, size_t row, size_t col);
		void removeFromGrid(size_t particleIndex, size_t pos);
		void removeFromGrid(size_t particleIndex, GridCell *cell);
		void removeFromGrid(size_t particleIndex, double pos_x, double pos_y);

		void move(size_t particleIndex, size_t old_row, size_t old_col, size_t new_row, size_t new_col);
		void move(size_t particleIndex, size_t old_pos, size_t new_pos);

		// bool particleChangedCells(Particle *p, size_t *old_pos, size_t *new_pos);

		size_t getGridIndexFromParticlePos(double pos_x, double pos_y) {
			const double new_pos_x = pos_x * inverse_square_diameter;
			const double new_pos_y = pos_y * inverse_square_diameter;
			return getGridIndexFromRowCol(static_cast<size_t>(new_pos_y), static_cast<size_t>(new_pos_x));
		}

		constexpr size_t getGridIndexFromRowCol(size_t row, size_t col) {
			return (row * cols) + col;
		}

		constexpr GridCell *get(size_t row, size_t col) {
			return &(this->cells[getGridIndexFromRowCol(row, col)]);
		}

		constexpr GridCell *get(size_t pos) {
			return &(this->cells[pos]);
		}
};

#endif

#ifndef GRID_H
#define GRID_H

#include "Particle.h"

// #include <iostream>

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
		// so that to transform into grid coordinates all you need to do is multiply by this
		pVec2 transform;

		double radius;

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

		// !!!!!! all the inserts assume the partile is not in that cell, if you mess this up there will be repetitions

		void insertIntoGrid(size_t particleIndex, size_t row, size_t col);
		void insertIntoGrid(size_t particleIndex, size_t pos);
		void insertIntoGrid(size_t particleIndex, GridCell *cell);
		void insertIntoGrid(size_t particleIndex, const pVec2& particlePos);

		void removeFromGrid(size_t particleIndex, size_t row, size_t col);
		void removeFromGrid(size_t particleIndex, size_t pos);
		void removeFromGrid(size_t particleIndex, GridCell *cell);
		void removeFromGrid(size_t particleIndex, const pVec2& particlePos);

		void move(size_t particleIndex, size_t old_row, size_t old_col, size_t new_row, size_t new_col);
		void move(size_t particleIndex, size_t old_pos, size_t new_pos);

		bool particleChangedCells(Particle *p, size_t *old_pos, size_t *new_pos);

		inline size_t getGridIndexFromParticlePos(pVec2 pos) {
			pVec2 new_pos = pos * transform; // contains row and col
			// std::cout << "inserting into grid, turning " << pos.x << "," << pos.y << " into " << static_cast<size_t>(new_pos.x) << "," << static_cast<size_t>(new_pos.y) << std::endl;
			return getGridIndexFromRowCol(static_cast<size_t>(new_pos.x), static_cast<size_t>(new_pos.y));
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

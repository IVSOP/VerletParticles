#ifndef GRID_H
#define GRID_H

typedef struct {
	size_t particle_idx; // [GRID_CELL_CAPACITY];
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
		// so that to transform into need coordinates all you need to do is multiply by this
		pVec2 transform;

		Grid() = delete;
		Grid(size_t pixel_width, size_t pixel_height, unsigned min_radius) {
			cols = pixel_width / min_radius;
			rows = pixel_height / min_radius;
			size = rows * cols;
			cells = new(std::nothrow) GridCell[size];
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

		// grid index, not particle index
		size_t getAt(pVec2 pos) {
			size_t x = static_cast<size_t>(pos.x),
			y = static_cast<size_t>(pos.y);

			return (y * rows) + x;
		}
};

#endif

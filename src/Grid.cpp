#include "Grid.h"

Grid::Grid(size_t pixel_width, size_t pixel_height, size_t min_radius) {
	cols = pixel_width / min_radius;
	rows = pixel_height / min_radius;
	size = rows * cols;
	cells = new GridCell[size];
	size_t i;
	for (i = 0; i < size; i++) {
		cells[i].len_particles = 0;
	}

	// extra division (1 /...) but allows for every insert to only have to multiply by this
	transform = {
		1.0f / (pixel_width / cols),
		1.0f / (pixel_height / rows)
	};
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
	cell->particle_idx[cell->len_particles ++] = particleIndex;
}

void Grid::insertIntoGrid(size_t particleIndex, size_t pos) {
	GridCell *cell = get(pos);
	cell->particle_idx[cell->len_particles ++] = particleIndex;
}

void Grid::insertIntoGrid(size_t particleIndex, GridCell *cell) {
	cell->particle_idx[cell->len_particles ++] = particleIndex;
}

void Grid::insertIntoGrid(size_t particleIndex, pVec2 particlePos) {

	size_t i = getGridIndexFromParticlePos(particlePos);
	GridCell *cell = &(cells[i]);
	// std::cout << "Inserting into grid at " << i << " which corresponds to " << i / cols << "," << i % cols << std::endl;
	cell->particle_idx[cell->len_particles ++] = particleIndex;
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

void Grid::move(size_t particleIndex, size_t old_row, size_t old_col, size_t new_row, size_t new_col) {

}

void Grid::move(size_t particleIndex, size_t old_pos, size_t new_pos) {

}

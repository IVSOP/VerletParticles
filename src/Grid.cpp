#include "Grid.h"

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

void Grid::removeFromGrid(size_t particleIndex, size_t row, size_t col) {
	GridCell *cell = get(row, col);
	if (removeFromArr(particleIndex, cell->particle_idx)) {
		cell->len_particles --;
	}
}

void Grid::insertIntoGrid(size_t particleIndex, size_t pos) {
	GridCell *cell = get(pos);
	cell->particle_idx[cell->len_particles ++] = particleIndex;
}

void Grid::removeFromGrid(size_t particleIndex, size_t pos) {
	GridCell *cell = get(pos);
	if (removeFromArr(particleIndex, cell->particle_idx)) {
		cell->len_particles --;
	}
}

void Grid::move(size_t particleIndex, size_t old_row, size_t old_col, size_t new_row, size_t new_col) {

}

void Grid::move(size_t particleIndex, size_t old_pos, size_t new_pos) {

}

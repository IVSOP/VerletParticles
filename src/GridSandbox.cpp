#include "GridSandbox.h"

#include <glm/geometric.hpp>

GridSandbox::GridSandbox(size_t max_particles, size_t pixelsX, size_t pixelsY)
	: Sandbox(max_particles, pixelsX, pixelsY),
	  grid(pixelsX, pixelsY, 20) // !!!!!!!!!!!!!!!!!! for now grid does not take into account max particles
	{
}

void GridSandbox::addParticle(Particle &particle) {
	this->particles[this->len_particles] = particle; // will this copy the struct???
	grid.insertIntoGrid(this->len_particles, particle.current_pos);
	this->len_particles ++;
}

void GridSandbox::updatePositions(double dt) {
	size_t i, new_pos, old_pos;
	Particle *p;

	for (i = 0; i < this->len_particles; i++) {
		p = &(this->particles[i]);
		p->updatePosition(dt);
		if (grid.particleChangedCells(p, &old_pos, &new_pos)) {
			grid.removeFromGrid(i, p->old_pos);
			grid.insertIntoGrid(i, p->current_pos);
		}
	}
}

void GridSandbox::applyCircleConstraint() {
	const pVec2 center = {500.0f, 500.0f};
	const double radius = 500.0f;

	pVec2 to_center, n;
	Particle *particle;
	double dist_to_center;
	size_t i;

	for (i = 0; i < len_particles; i++) {
		particle = &(particles[i]);
		to_center = particle->current_pos - center;
		dist_to_center = glm::length(to_center);
		// check if particle is clipping constraint bounds
		if (dist_to_center > radius - particle->radius) {
			n = to_center / dist_to_center;
			particle->current_pos = center + (n * (radius - particle->radius));
		}
	}
}

void GridSandbox::applyRectangleConstraint() {
	Particle *p; size_t i;
	const double size_x = 1000.0f / 2.0f; // divide by 2 here to save time
	const double size_y = 1000.0f / 2.0f;
	// const double margin = 2.0f;
	pVec2 center = {500.0f, 500.0f};
	double radius;

	// this is worse than the circle one
	for (i = 0; i < len_particles; i++) {
		p = &(particles[i]);
		radius = p->radius;

		if (p->current_pos.x + radius > center.x + size_x) {
			p->current_pos.x = center.x + size_x - radius;
		} else if (p->current_pos.x - radius < center.x - size_x) {
			p->current_pos.x = center.x - size_x + radius;
		}

		if (p->current_pos.y + radius > center.y + size_y) {
			p->current_pos.y = center.y + size_y - radius;
		} else if (p->current_pos.y - radius < center.y - size_y) {
			p->current_pos.y = center.y - size_y + radius;
		}
	}
}


void GridSandbox::solveCollisions() {
	size_t row, col, lookup_row, lookup_col;
	GridCell *centerCell;

	// the borders are not iterated over
	for (row = 1; row < grid.rows - 1; row++) {
		for (col = 1; col < grid.cols - 1; col++) {
			
			centerCell = grid.get(row, col);
			// compare with all surrounding cells

			collideParticlesGrid(centerCell, centerCell); // there may be more than one particle in a grid, and they will collide with each other
			for (lookup_row = row - 1; lookup_row < row + 2; lookup_row ++) {
				for (lookup_col = col - 1; lookup_col < col + 2; lookup_col ++) {
					collideParticlesGrid(centerCell, grid.get(lookup_row, lookup_col));
				}
			}
		}
	}
}

// checks all particles from centerCell vs all particles from secondCell, as long as particles do not have the same index
void GridSandbox::collideParticlesGrid(GridCell *centerCell, GridCell *secondCell) {
	pVec2 collisionAxis, n;
	double dist, min_dist;
	const double response_coef = 0.75f;
	double mass_ratio_1, mass_ratio_2, delta;
	double p1_radius, p2_radius;

	size_t index_p1, index_p2;

	Particle *p1, *p2;

	int i, j;
	for (i = 0; i < centerCell->len_particles; i++) {
		index_p1 = centerCell->particle_idx[i];
		p1 = &(particles[index_p1]);
		p1_radius = p1->radius;
		for (j = 0; j < secondCell->len_particles; j++) {
			index_p2 = secondCell->particle_idx[j];

			if (index_p1 != index_p2) { // particle can't collide with itself
				p2 = &(particles[index_p2]);
				p2_radius = p2->radius;

				collisionAxis = p1->current_pos - p2->current_pos;
				min_dist = p1_radius + p2_radius;
				dist = (collisionAxis.x * collisionAxis.x) + (collisionAxis.y * collisionAxis.y);
				// avoid srqt as long as possible

				// collision ocurred, need to update grid to reflect this
				if (dist < min_dist * min_dist) {
					dist = sqrt(dist);
					n = collisionAxis / dist;
					
					mass_ratio_1 = p1_radius / (p1_radius + p2_radius);
					mass_ratio_2 = p2_radius / (p1_radius + p2_radius);

					delta = 0.5f * response_coef * (dist - min_dist);

					grid.removeFromGrid(index_p1, centerCell);
					grid.removeFromGrid(index_p2, secondCell);

					p1->current_pos -= n * (mass_ratio_2 * delta);
					p2->current_pos += n * (mass_ratio_1 * delta);

					grid.insertIntoGrid(index_p1, p1->current_pos);
					grid.insertIntoGrid(index_p2, p2->current_pos);
				}

			}
		}
	}
}

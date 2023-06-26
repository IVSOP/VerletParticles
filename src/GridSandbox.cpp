#include "GridSandbox.h"

#include <glm/geometric.hpp>

#include <stdio.h>

GridSandbox::GridSandbox(size_t max_particles, size_t pixelsX, size_t pixelsY)
	: Sandbox(max_particles, pixelsX, pixelsY),
	  grid(pixelsX, pixelsY, GRID_PARTICLE_SIZE) // !!!!!!!!!!!!!!!!!! for now grid does not take into account max particles
	{
}

void GridSandbox::addParticle(Particle &particle) {
	this->particles[this->len_particles] = particle; // will this copy the struct???
	addColorToParticle(len_particles, particle.color);

	grid.insertIntoGrid(this->len_particles, particle.current_pos);
	this->len_particles ++;
}

void GridSandbox::updatePositions(double dt) {
	size_t i; // , new_pos, old_pos;
	Particle *p;

	for (i = 0; i < this->len_particles; i++) {
		p = &(this->particles[i]);
		p->updatePosition(dt);
		// if (grid.particleChangedCells(p, &old_pos, &new_pos)) {
		// 	grid.removeFromGrid(i, p->old_pos);
		// 	grid.insertIntoGrid(i, p->current_pos);
		// }
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

// change this so that instead of colliding between cells
// I take one particle at a time from center and compare to all other particles???
void GridSandbox::solveCollisions() {
	size_t row, col;
	GridCell *centerCell;

	// rebuild entire grid
	grid.clear();
	size_t i;
	for (i = 0; i < len_particles; i++) {
		grid.insertIntoGrid(i, particles[i].current_pos);
	}

	for (row = 1; row < grid.rows - 1; row++) {
		for (col = 1; col < grid.cols - 1; col++) {
			
			centerCell = grid.get(row, col);
			// compare with all surrounding cells

			// decided to unroll loop for simplicity, idk
			collideParticlesBetweenCells(centerCell, grid.get(row + 1, col - 1));
			collideParticlesBetweenCells(centerCell, grid.get(row + 1, col));
			collideParticlesBetweenCells(centerCell, grid.get(row + 1, col + 1));

			collideParticlesBetweenCells(centerCell, grid.get(row, col - 1));
			collideParticlesSameCell(centerCell);
			collideParticlesBetweenCells(centerCell, grid.get(row, col + 1));

			collideParticlesBetweenCells(centerCell, grid.get(row - 1, col - 1));
			collideParticlesBetweenCells(centerCell, grid.get(row - 1, col));
			collideParticlesBetweenCells(centerCell, grid.get(row - 1, col + 1));
		}
	}

	// floor and ceiling
	row = grid.rows - 1; // last row
	for (col = 1; col < grid.cols - 1; col ++) {

		// ceiling
		centerCell = grid.get(row, col);
		collideParticlesBetweenCells(centerCell, grid.get(row, col - 1));
		collideParticlesSameCell(centerCell);
		collideParticlesBetweenCells(centerCell, grid.get(row, col + 1));
		
		// floor
		centerCell = grid.get(0, col);
		collideParticlesBetweenCells(centerCell, grid.get(0, col - 1));
		collideParticlesSameCell(centerCell);
		collideParticlesBetweenCells(centerCell, grid.get(0, col + 1));
	}

	// walls
	col = grid.cols - 1;
	for (row = 1; row < grid.rows - 1; row ++) {

		// left
		centerCell = grid.get(row, 0);
		collideParticlesBetweenCells(centerCell, grid.get(row - 1, 0));
		collideParticlesSameCell(centerCell);
		collideParticlesBetweenCells(centerCell, grid.get(row + 1, 0));
		
		// right
		centerCell = grid.get(row, col);
		collideParticlesBetweenCells(centerCell, grid.get(row - 1, col));
		collideParticlesSameCell(centerCell);
		collideParticlesBetweenCells(centerCell, grid.get(row + 1, col));
	}

	// corners only need to check themselves
	// collideParticlesSameCell(grid.get(0, 0));
	// collideParticlesSameCell(grid.get(0, grid.cols - 1));
	// collideParticlesSameCell(grid.get(grid.rows - 1, grid.cols - 1));
	// collideParticlesSameCell(grid.get(grid.rows - 1, 0));
	
}

// checks all particles from centerCell vs all particles from secondCell, as long as particles do not have the same index
void GridSandbox::collideParticlesBetweenCells(GridCell *centerCell, GridCell *secondCell) {
	size_t i, j;
	Particle *p1, *p2;

	for (i = 0; i < centerCell->len_particles; i++) {
		p1 = &(particles[centerCell->particle_idx[i]]);

		for (j = 0; j < secondCell->len_particles; j++) {
			p2 = &(particles[secondCell->particle_idx[j]]);

			collideParticles(p1, p2);
		}
	}
}

// assumes radiuses are the same
void GridSandbox::collideParticles(Particle *p1, Particle *p2) {
	pVec2 collisionAxis, n;
	double dist;
	const double response_coef = 0.75f;
	double mass_ratio_1, mass_ratio_2, delta;
	const double p1_radius = grid.radius;
	const double p2_radius = grid.radius;
	const double min_dist = grid.radius * 2;

	collisionAxis = p1->current_pos - p2->current_pos;
	dist = (collisionAxis.x * collisionAxis.x) + (collisionAxis.y * collisionAxis.y);
	// avoid srqt as long as possible

	
	if (dist < min_dist * min_dist) {
		dist = sqrt(dist);
		n = collisionAxis / dist;
		
		mass_ratio_1 = p1_radius / (p1_radius + p2_radius);
		mass_ratio_2 = p2_radius / (p1_radius + p2_radius);

		delta = 0.5f * response_coef * (dist - min_dist);

		p1->current_pos -= n * (mass_ratio_2 * delta);
		p2->current_pos += n * (mass_ratio_1 * delta);
	}
}

// separate funciton to avoid an extra if in the normal one
void GridSandbox::collideParticlesSameCell(GridCell *cell) {
	size_t i, j;
	Particle *p1, *p2;
	for (i = 0; i < cell->len_particles; i++) {
		p1 = &(particles[i]);
		for (j = i + 1; j < cell->len_particles; j++) {
			p2 = &(particles[j]);
			collideParticles(p1, p2);
		}
	}
}

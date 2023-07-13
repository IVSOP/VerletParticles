#include "GridSandbox.h"

#include <glm/geometric.hpp>

#include <stdio.h>

GridSandbox::GridSandbox(size_t max_particles, size_t pixelsX, size_t pixelsY)
	: Sandbox(max_particles, pixelsX, pixelsY),
	  grid(pixelsX, pixelsY, GRID_PARTICLE_SIZE) // !!!!!!!!!!!!!!!!!! for now grid does not take into account max particles
	{

	thpool = thpool_init(MAX_THREADS);
}

GridSandbox::~GridSandbox() {
	thpool_destroy(thpool);
}

void GridSandbox::addParticle(pFloat cx, pFloat cy, pFloat ax, pFloat ay, GLfloat color[4]) {
	particles.current_x[len_particles] = cx;
	particles.current_y[len_particles] = cy;
	particles.old_x[len_particles] = cx;
	particles.old_y[len_particles] = cy;
	particles.accel_x[len_particles] = ax;
	particles.accel_y[len_particles] = ay;

	// is this needed??????????????????????????????????/ I don't think it's ever used
	particles.color[len_particles] = color[0];
	particles.color[len_particles + 1] = color[1];
	particles.color[len_particles + 2] = color[2];
	particles.color[len_particles + 3] = color[3];
	
	addColorToParticle(len_particles, color);

	grid.insertIntoGrid(this->len_particles, cx, cy);
	this->len_particles ++;
}

// I tried making it so when particle is updated it gets inserted into grid
// However this makes it non deterministic
// Making update + grid insertions at the same time but all in one core was actually slower as well
void GridSandbox::updatePositions(pFloat dt) {
	size_t i, start, end;

	const size_t particlesPerThread = this->len_particles / MAX_THREADS;
	// printf("%ld\n", particlesPerThread);
	
	UpdateArgs *args = (UpdateArgs *)alloca(sizeof(UpdateArgs) * MAX_THREADS);

	start = 0;
	// thread 0 is used to fix the bad division, the math to the right is the remainder
	end = particlesPerThread + (this->len_particles - MAX_THREADS * particlesPerThread);
	// printf("There are %ld particles\n", len_particles);
	for (i = 0; i < MAX_THREADS; i++) {
		// printf("[%d] start %d end %d\n", i, start, end);
		args[i] = UpdateArgs(start, end, &this->particles, dt, this->pixelsX, this->pixelsY, &this->grid);
		thpool_add_work(this->thpool, updatePositionsThread, args + i);
		// updatePositionsThread(args + i);

		// start += particlesPerThread;
		start = end; // easy fix to the fact that first iteration is a bit different to compensate for remainder
		end += particlesPerThread;
	}

	// // printf("\n\n\n");

	thpool_wait(this->thpool);

	// rebuild entire grid, can be done anywhere apparently so I put it here
	grid.clear();
	insertIntoGrid();
}

void updatePositionsThread(void *args) {
	const UpdateArgs *info = (UpdateArgs *)args;
	
	size_t i;
	constexpr pFloat radius = GRID_PARTICLE_SIZE;
	ParticleArray *p = info->particles;

	// printf("Parsing from %ld to %ld\n", info->start, info->end);

	for (i = info->start; i < info->end; i++) {
		p->updateParticlePosition(i, info->dt);

		// applying constraint
		if (p->current_x[i] + radius > info->pixelsX) {
			p->current_x[i] = info->pixelsX - radius;
		} else if (p->current_x[i] - radius < 0) {
			p->current_x[i] = 0 + radius;
		}

		if (p->current_y[i] + radius > info->pixelsY) {
			p->current_y[i] = info->pixelsY - radius;
		} else if (p->current_y[i] - radius < 0) {
			p->current_y[i] = 0 + radius;
		}

	}

}

// void GridSandbox::applyCircleConstraint() {
// 	const pVec2 center = {500.0f, 500.0f};
// 	const pFloat radius = 500.0f;

// 	pVec2 to_center, n;
// 	Particle *particle;
// 	pFloat dist_to_center;
// 	size_t i;

// 	for (i = 0; i < len_particles; i++) {
// 		particle = &(particles[i]);
// 		to_center = particle->current_pos - center;
// 		dist_to_center = to_center.length();
// 		// check if particle is clipping constraint bounds
// 		if (dist_to_center > radius - particle->radius) {
// 			n = to_center / dist_to_center;
// 			particle->current_pos = center + (n * (radius - particle->radius));
// 		}
// 	}
// }

struct GridArgs {
	int start, end;
	ParticleArray *particles;
	Grid *grid;

	GridArgs(int _start, int _end, ParticleArray *_particles, Grid *_grid)
	: start(_start), end(_end), particles(_particles), grid(_grid)
	{}
};

void insertIntoGrid(void *args) {
	GridArgs *info = (GridArgs *)args;
	info->grid->insertIntoGrid(info->start, info->end, info->particles);
}

// cannot just be multithreaded blindly
// if more than one particle is in a cell,
// the order of insertion matters
void GridSandbox::insertIntoGrid() {
	grid.insertIntoGrid(0, this->len_particles, &this->particles);
}

// NOTE: I assume the math here checks out
// in the future might fix
// for 50x50 and 100x100 grids it works, else grid might not be correctly spread between threads
// this is a temporary solution, did not have the brainpower to come up with a better way
void GridSandbox::solveCollisions() {
	size_t i;

	const size_t rows_per_thread = grid.rows / MAX_THREADS;
	const size_t half_rows_per_thread = rows_per_thread / 2;

	size_t start, end;

	Args args[MAX_THREADS];

	start = 0;
	// first thread makes up for bad division
	// unrolled first iteration since it is simpler
	const size_t remainder = (grid.rows - rows_per_thread * MAX_THREADS) / 2;
	end = half_rows_per_thread + remainder;
	args[0].row_start = start;
	args[0].row_end = end;
	args[0].sandbox = this;
	start += remainder * 2;
	end += remainder;
	thpool_add_work(thpool, collideParticlesFromTo, args + 0);


	for (i = 1; i < MAX_THREADS; i++) {
		start += rows_per_thread;
		end += rows_per_thread;
		args[i].row_start = start;
		args[i].row_end = end;
		args[i].sandbox = this;
		thpool_add_work(thpool, collideParticlesFromTo, args + i);
	}

	thpool_wait(thpool);

	start = half_rows_per_thread + ((grid.rows - rows_per_thread * MAX_THREADS) / 2);
	end = start * 2;

	args[0].row_start = start;
	args[0].row_end = end;
	args[0].sandbox = this;
	start += remainder;
	thpool_add_work(thpool, collideParticlesFromTo, args + 0);

	for (i = 1; i < MAX_THREADS; i++) {
		start += rows_per_thread;
		end += rows_per_thread;
		args[i].row_start = start;
		args[i].row_end = end;
		args[i].sandbox = this;
		thpool_add_work(thpool, collideParticlesFromTo, args + i);
	}


	thpool_wait(thpool);
}

void collideParticlesFromTo(void *args) {
	// there were glitches due to the order the particles were being processed
	// decided to simplify and just put an if into collideParticlesBetweenCells to check for out of bounds

	size_t row, col;
	GridCell *centerCell;
	const Args *info = (Args *)args;

	for (row = info->row_start; row < info->row_end; row++) {
		for (col = 0; col < info->sandbox->grid.cols; col++) {
			centerCell = info->sandbox->grid.get(row, col); // unecessary????????????????????????????????
			// compare with all surrounding cells

			info->sandbox->collideParticlesBetweenCellsV2(centerCell, row + 1, col - 1); // without this one, becomes unstable
			info->sandbox->collideParticlesBetweenCellsV2(centerCell, row + 1, col);
			info->sandbox->collideParticlesBetweenCellsV2(centerCell, row + 1, col + 1);

#ifdef CHECK_ALL_CELL_COLLISIONS
			info->sandbox->collideParticlesBetweenCellsV2(centerCell, row, col - 1);
#endif
			info->sandbox->collideParticlesSameCell(centerCell);
			info->sandbox->collideParticlesBetweenCellsV2(centerCell, row, col + 1);

#ifdef CHECK_ALL_CELL_COLLISIONS

			info->sandbox->collideParticlesBetweenCellsV2(centerCell, row - 1, col - 1);
			info->sandbox->collideParticlesBetweenCellsV2(centerCell, row - 1, col);
			info->sandbox->collideParticlesBetweenCellsV2(centerCell, row - 1, col + 1);
#endif
		}
	}
}

// checks all particles from centerCell vs all particles from secondCell, as long as particles do not have the same index
void GridSandbox::collideParticlesBetweenCells(GridCell *centerCell, GridCell *secondCell) {
	size_t i, j;
	size_t ID_A, ID_B;

	for (i = 0; i < centerCell->len_particles; i++) {
		ID_A = centerCell->particle_idx[i];

		for (j = i + 1; j < secondCell->len_particles; j++) {
			ID_B = secondCell->particle_idx[j];

			collideParticles(ID_A, ID_B);
		}
	}
}

void GridSandbox::collideParticlesBetweenCellsV2(GridCell *centerCell, size_t row, size_t col) {
	size_t i, j;
	size_t ID_A, ID_B;

	// if it is 0 - 1 will overflow and end up bigger anyway, no need to check
	if (row >= grid.rows || col >= grid.cols) {
		return;
	}

	GridCell *secondCell = grid.get(row, col);

	for (i = 0; i < centerCell->len_particles; i++) {
		ID_A = centerCell->particle_idx[i];

		for (j = 0; j < secondCell->len_particles; j++) {
			ID_B = secondCell->particle_idx[j];

			collideParticles(ID_A, ID_B);
		}
	}
}

// assumes radiuses are the same
void GridSandbox::collideParticles(size_t ID_A, size_t ID_B) {
	pFloat collisionAxis_x, collisionAxis_y;
	pFloat dist;
	constexpr pFloat response_coef = 0.75f;
	pFloat delta;
	const pFloat min_dist = grid.square_diameter; // minimum distance between each other for there to be a collision

	collisionAxis_x = particles.current_x[ID_A] - particles.current_x[ID_B];
	collisionAxis_y = particles.current_y[ID_A] - particles.current_y[ID_B];;

	// avoid srqt as long as possible
	dist = (collisionAxis_x * collisionAxis_x) + (collisionAxis_y * collisionAxis_y);
	
	if (dist < min_dist * min_dist) {

		dist = sqrt(dist);
		collisionAxis_x /= dist;
		collisionAxis_y /= dist;

		delta = 0.5f * response_coef * (dist - min_dist);

		particles.current_x[ID_A] -= collisionAxis_x * (0.5 * delta);
		particles.current_y[ID_A] -= collisionAxis_y * (0.5 * delta);

		particles.current_x[ID_B] += collisionAxis_x * (0.5 * delta);
		particles.current_y[ID_B] += collisionAxis_y * (0.5 * delta);
	}
}

// separate funciton to avoid an extra if in the normal one
void GridSandbox::collideParticlesSameCell(GridCell *cell) {
	size_t i, j;
	size_t ID_A, ID_B;
	for (i = 0; i < cell->len_particles; i++) {
		ID_A = cell->particle_idx[i];
		for (j = i + 1; j < cell->len_particles; j++) {
			ID_B = cell->particle_idx[j];
			collideParticles(ID_A, ID_B);
		}
	}
}

// dumps each row into a file with the ID of each particle
void GridSandbox::dumpGridToFile() {
	exit(10);
	// FILE *file = fopen("grid_dump.csv", "w");
	// GridCell *cell;
	// Particle *p;
	// int len;

	// char buff[16];

	// size_t row, col;
	// for (row = 0; row < grid.rows; row++) {
	// 	for (col = 0; col < grid.cols - 1; col++) {
	// 		// for now I assume in the end 1 particle per grid
	// 		cell = grid.get(row, col);
	// 		if (cell->len_particles != 0) {
	// 			p = &(particles[cell->particle_idx[0]]);
	// 			// printf("printing %ld %ld, particle is in %f,%f\n", row, col, particles[cell->particle_idx[0]].current_pos.x, particles[cell->particle_idx[0]].current_pos.y);
	// 			len = snprintf(buff, 16, "%d,", getParticleID(p));
	// 			fwrite(buff, 1, len, file);
	// 		} else {
	// 			if (cell->len_particles > 1) { // used as a check to make sure
	// 				fprintf(stderr, "More than one particle in cell row %ld col %ld\n", row, col);
	// 				exit(10);
	// 			}
	// 			buff[0] = ',';
	// 			fwrite(buff + 0, 1, 1, file);
	// 		}

	// 		// itoa()?? idc if it is slow
	// 	}
	// 	// final iteration is out of loop
	// 	cell = grid.get(row, col);
	// 	if (cell->len_particles != 0) {
	// 		p = &(particles[cell->particle_idx[0]]);
	// 		len = snprintf(buff, 16, "%d\n", getParticleID(p));
	// 		fwrite(buff, 1, len, file);
	// 	} else {
	// 		buff[0] = ','; buff[1] = '\n';
	// 		fwrite(buff + 0, 1, 2, file);
	// 	}
	// }

	// fclose(file);
}

// #define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include "stb_image_write.h"

// for now I assume it is squared
GLfloat *GridSandbox::convert_png(const char *path) {
	stbi_set_flip_vertically_on_load(1);
	int width, height, BPP;
	// load png
	unsigned char *image =	stbi_load(path, &width, &height, &BPP, 4); // 4 -> RGBA

	if (!image) {
		fprintf(stderr, "Error loading image\n");
		exit(1);
	}

	if (width != height) {
		fprintf(stderr, "Non-squared images not supported for now\n");
		exit(1);
	}

	if (BPP != 4) {
		fprintf(stderr, "PNG is not RGBA\n");
		exit(1);
	}

	// divide png into grid and average values for each cell
	GLfloat *buff = (GLfloat *)calloc(4 * grid.size, sizeof(GLfloat));
	// to simplify things, will do 2 passes
	// first just sums things into buff

	// need to divide png into same number of cells as grid
	// const unsigned int size = width * height * BPP;
	// everything here assumes squared image and grid
	// size / grid.size would probably work
	const unsigned int pixel_width_cell = width / grid.cols;


	GLfloat R, G, B;//, A;
	// need to normalize into floats, 0 == 0, 255 == 1

	// current_pixel == i * 4
	// to process values for a single cell, need to iterate over pixel_width_cell^2 pixels
	// first pixel_width_cell pixels are [0], next are [1], ...
	// until it reaches max image width or grid width
	// then, counter restarts

	// offsets are to avoid repeating calculations
	unsigned int pixel_col, pixel_row, offset2;
	unsigned int row, col, cell_offset, cell_offset2; // these offsets expand to row * cols + col, turning (row, col) into position in buff



	// wtf is this
	for (row = 0; row < grid.rows; row++) {
		cell_offset = row * grid.cols;
		for (pixel_row = 0; pixel_row < pixel_width_cell; pixel_row ++) {
			// offset = pixel_row * width;
			// every pixel_width_cell, move to next cell
			for (col = 0; col < grid.cols; col++) {
				for (pixel_col = 0; pixel_col < pixel_width_cell; pixel_col ++) {
					// offset2 = offset + (pixel_col * 4); // is += 4 in the loop faster? idc

					offset2 = ((pixel_col) + (pixel_row * width) + (col * pixel_width_cell) + (row * grid.cols * pixel_width_cell)) * 4;
					cell_offset2 = cell_offset + col;

					// printf("%d\n", offset2);

					R = static_cast<GLfloat>(image[offset2 + 0]) / 255.0f;
					G = static_cast<GLfloat>(image[offset2 + 1]) / 255.0f;
					B = static_cast<GLfloat>(image[offset2 + 2]) / 255.0f;
					// A = static_cast<GLfloat>(image[offset2 + 3]) / 255.0f;

					buff[(cell_offset2 * 4) + 0] += R;
					buff[(cell_offset2 * 4) + 1] += G;
					buff[(cell_offset2 * 4) + 2] += B;
					// buff[cell_offset2 + 3] += A;
				}
			}
		}
	}
	// exit(1);

	// total number of pixels per cell, so I can get the average
	const float pixel_width_cell_squared = static_cast<float>(pixel_width_cell) * static_cast<float>(pixel_width_cell);

	unsigned int i;
	for (i = 0; i < grid.size; i ++) {
		buff[(i * 4) + 0] /= pixel_width_cell_squared;
		buff[(i * 4) + 1] /= pixel_width_cell_squared;
		buff[(i * 4) + 2] /= pixel_width_cell_squared;
		// buff[i + 3] /= pixel_width_cell_squared;
		buff[(i * 4) + 3] = 1.0f;

		// printf("buff[%d] (R) is %f\n", i * 4, buff[i * 4]);
	}

	// exit(1);

	// char filename[] = "img.png";
	// stbi_write_png(filename, grid.cols, grid.rows, 4, buff, grid.cols * 4);

	stbi_image_free(image);
	return buff;
}

void GridSandbox::clear() {
	Sandbox::len_particles = 0;
	Sandbox::current_tick = 0;
	grid.clear();

	int i, size = Sandbox::spawners.size();

	for (i = 0; i < size; i++) {
		(&spawners[i])->reset();
	}
}

// input is colors averaged in a grid over the image
// output is colors orderer by the ID of particles
// i.e. particle #69 will get its color in ...[69]

// due to erros like particle being .005 away from next cell, there might be more than one particle per cell
// in those cases, remaining particles get black since color array was calloc'd
GLfloat *GridSandbox::parseColorsByGrid(GLfloat *colors) {
	exit(10);
	return(nullptr);
	// GridCell *cell;
	// unsigned int ID;

	// GLfloat *final_colors = (GLfloat *)calloc(4 * grid.size, sizeof(GLfloat));

	// size_t row, col, offset, offset2;
	// for (row = 0; row < grid.rows; row++) {
	// 	offset = row * grid.cols;
	// 	for (col = 0; col < grid.cols; col++) {
	// 		offset2 = offset + col;
	// 		cell = grid.get(row, col);
			
	// 		if (cell->len_particles != 0) {
	// 			ID = getParticleID(&(particles[cell->particle_idx[0]]));

	// 			final_colors[ID + 0] = colors[offset2 + 0];
	// 			final_colors[ID + 1] = colors[offset2 + 1];
	// 			final_colors[ID + 2] = colors[offset2 + 2];
	// 			final_colors[ID + 3] = colors[offset2 + 3];


	// 			// printf("setting colors for ID %d %f %f %f %f\n", ID, final_colors[ID + 0], final_colors[ID + 1], final_colors[ID + 2], final_colors[ID + 3]);
	// 		}

	// 		// if (cell->len_particles > 1) { // used as a check to make sure 1 particle per cell
	// 			// fprintf(stderr, "More than one particle in cell row %ld col %ld\n", row, col);
	// 			// printf("particle locations are:\n");
	// 			// for (int i = 1; i < cell->len_particles; i++) {
	// 				// printf("%f %f\n", particles[cell->particle_idx[i]].current_pos.x, particles[cell->particle_idx[i]].current_pos.y);
	// 			// }
	// 			// exit(10);
	// 		// }
	// 	}
	// }

	// return final_colors;
}


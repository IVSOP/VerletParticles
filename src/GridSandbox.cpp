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

// max MAX_THREADS threads
// each thread needs to start in a position divisible by 8 (align to 32 bits)

void GridSandbox::updatePositions(pFloat dt) {
	const pFloat dtSquared = dt * dt;

	if (len_particles > 8) {
		const size_t requiredThreads = std::min(static_cast<size_t>(MAX_THREADS), len_particles / 8);

		const size_t avgParticlesPerThread = len_particles / requiredThreads;
		// const size_t chunksOf8 = avgParticlesPerThread / 8; // this would require me to keep multiplying by 8, will just round it
		const size_t particlesPerThread = avgParticlesPerThread & (~7u); // round down to nearest multiple of 8

		UpdateArgs *args = (UpdateArgs *)alloca(sizeof(UpdateArgs) * requiredThreads);

		// the last thread can do (requiredThreads - 1) * 8 more work than the others, will fix in the future, insignificant for now
		// will need to distribute remainder through other threads in chunks of 8

		size_t start = 0, end = particlesPerThread, i;

		for (i = 0; i < requiredThreads - 1; i++) {
			args[i] = UpdateArgs(start, end, &this->particles, dtSquared, this->pixelsX, this->pixelsY, &this->grid);
																															// thpool_add_work(this->thpool, updatePositionsThreadVectorOnly, args + i);
			thpool_add_work(this->thpool, updatePositionsThreadVector, args + i);
			start += particlesPerThread;
			end += particlesPerThread;
		}

		// last thread takes the remaining work
		args[i] = UpdateArgs(start, len_particles, &this->particles, dtSquared, this->pixelsX, this->pixelsY, &this->grid);
		thpool_add_work(this->thpool, updatePositionsThreadVector, args + i);

		thpool_wait(this->thpool);
	} else {
		UpdateArgs args = UpdateArgs(0, len_particles, &this->particles, dtSquared, this->pixelsX, this->pixelsY, &this->grid);
		updatePositionsThreadVector(&args);
	}

	// rebuild entire grid, can be done anywhere apparently so I put it here
	grid.clear();
	insertIntoGrid();
}

void updatePositionsThread(void *args) {
	const UpdateArgs *info = (UpdateArgs *)args;
	
	size_t i;
	constexpr pFloat radius = GRID_PARTICLE_SIZE;
	ParticleArray *p = info->particles;

	for (i = info->start; i < info->end; i++) {
		const pFloat velocity_x = p->current_x[i] - p->old_x[i];
		const pFloat velocity_y = p->current_y[i] - p->old_y[i];

		p->old_x[i] = p->current_x[i];
		p->old_y[i] = p->current_y[i];

		p->current_x[i] += velocity_x + (p->accel_x[i] * info->dtsquared);
		p->current_y[i] += velocity_y + (p->accel_y[i] * info->dtsquared);

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

// will also apply square constraint
// problem: might request a thread to do a random interval like 9 to 17
// this is not aligned to 32 bytes
// required changing algorithm to attribute work to threads
void updatePositionsThreadVector(void *args) {
	const UpdateArgs *info = (UpdateArgs *)args;
	
	size_t i;
	constexpr pFloat radius = GRID_PARTICLE_SIZE;
	ParticleArray *p = info->particles;

	const size_t len = info->end - info->start;
	const size_t simdSize = info->start + len % 8;

	// not needed to separate x and y, will asume compiler will take care of it
	__m256 velocity_x;
	__m256 velocity_y;
	__m256 current_x;
	__m256 current_y;
	__m256 old_x;
	__m256 old_y;
	__m256 accel_x;
	__m256 accel_y;
	__m256 newx;
	__m256 newy;
	__m256 mask;

	const __m256 dtsquaredVec = _mm256_set1_ps(info->dtsquared);
	const __m256 radiusVec = _mm256_set1_ps(radius);
	const __m256 pixelsXVec = _mm256_set1_ps(info->pixelsX);
	const __m256 pixelsYVec = _mm256_set1_ps(info->pixelsY);

	for (i = info->start; i < simdSize; i += 8) {
		////////////////////////////////////////// process X
		current_x = _mm256_load_ps(p->current_x + i);
		old_x = _mm256_load_ps(p->old_x + i);
		accel_x = _mm256_load_ps(p->accel_x + i);
		
		velocity_x = _mm256_sub_ps(current_x, old_x); // current_x - old_x
		_mm256_store_ps(p->old_x + i, current_x); // p->old_x[i] = p->current_x[i];
		current_x = _mm256_add_ps(current_x, _mm256_add_ps(velocity_x, _mm256_mul_ps(accel_x, dtsquaredVec))); // p->current_x[i] += velocity_x + (p->accel_x[i] * info->dtsquared);

		// is it faster to just memset the entire array????????????????????????? need to test
		_mm256_store_ps(p->accel_x + i, _mm256_setzero_ps()); // p->accel_x[i] = 0;
	
		// makes a mask of 0s and 1s, where 1 means the value was greater
		// if (p->current_x[i] > info->pixelsX - radius)
		mask = _mm256_cmp_ps(current_x, _mm256_sub_ps(pixelsXVec, radiusVec), _CMP_GT_OQ); // _CMP_GT_OS signals exception for NaN values
		newx = _mm256_blendv_ps(current_x, _mm256_sub_ps(pixelsXVec, radiusVec), mask);
		
		mask = _mm256_cmp_ps(current_x, radiusVec, _CMP_LT_OQ);
		// this works but it is not like an else if, it performs checks even if previous condition did not fail
		newx = _mm256_blendv_ps(newx, radiusVec, mask);

		_mm256_store_ps(p->current_x + i, newx);


		/*possible alternative that has more math but not reduntant checks
		__m256 xGreaterVec = _mm256_cmp_ps(currXVec, _mm256_sub_ps(pixelsXVec, radiusVec), _CMP_GT_OQ);
		__m256 xLessVec = _mm256_cmp_ps(currXVec, radiusVec, _CMP_LT_OQ);

		__m256 newXVec = _mm256_blendv_ps(currXVec, _mm256_sub_ps(pixelsXVec, radiusVec), _mm256_and_ps(xGreaterVec, _mm256_cmp_ps(currXVec, radiusVec, _CMP_GT_OQ)));
		newXVec = _mm256_blendv_ps(newXVec, currXVec, _mm256_and_ps(xLessVec, _mm256_cmp_ps(currXVec, _mm256_setzero_ps(), _CMP_LT_OQ)));
		
		will benchmark in the future
		*/



		////////////////////////////////////////// process Y
		current_y = _mm256_load_ps(p->current_y + i);
		old_y = _mm256_load_ps(p->old_y + i);
		accel_y = _mm256_load_ps(p->accel_y + i);
		
		velocity_y = _mm256_sub_ps(current_y, old_y);
		_mm256_store_ps(p->old_y + i, current_y);
		current_y = _mm256_add_ps(current_y, _mm256_add_ps(velocity_y, _mm256_mul_ps(accel_y, dtsquaredVec)));

		_mm256_store_ps(p->accel_y + i, _mm256_setzero_ps());

		mask = _mm256_cmp_ps(current_y, _mm256_sub_ps(pixelsYVec, radiusVec), _CMP_GT_OQ); // _CMP_GT_OS signals exception for NaN values
		newy = _mm256_blendv_ps(current_y, _mm256_sub_ps(pixelsYVec, radiusVec), mask);
		mask = _mm256_cmp_ps(current_y, radiusVec, _CMP_LT_OQ);
		newy = _mm256_blendv_ps(newy, radiusVec, mask);

		_mm256_store_ps(p->current_y + i, newy);

		// printf("alignment: %ld\n", ((size_t)(p->accel_x + i)) % 32); // must be 0 allways, otherwise segfault
	}

	for (; i < info->end; i++) {
			// basically p->updateParticlePosition(i, info->dt); but with dtsquared
			const pFloat velocity_x = p->current_x[i] - p->old_x[i];
			const pFloat velocity_y = p->current_y[i] - p->old_y[i];

			p->old_x[i] = p->current_x[i];
			p->old_y[i] = p->current_y[i];

			p->current_x[i] += velocity_x + (p->accel_x[i] * info->dtsquared);
			p->current_y[i] += velocity_y + (p->accel_y[i] * info->dtsquared);

			p->accel_x[i] = 0;
			p->accel_y[i] = 0;

		// applying constraint
		if (p->current_x[i] > info->pixelsX - radius) {
			p->current_x[i] = info->pixelsX - radius;
		} else if (p->current_x[i] < radius) {
			p->current_x[i] = radius;
		}

		if (p->current_y[i] > info->pixelsY - radius) {
			p->current_y[i] = info->pixelsY - radius;
		} else if (p->current_y[i] < radius) {
			p->current_y[i] = radius;
		}
	}
}

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

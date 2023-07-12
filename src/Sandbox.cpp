#include "Sandbox.h"

#include <stdlib.h>
#include <iostream>
#include "GLErrors.h"
#include <string.h>
#include <glm/geometric.hpp>

Sandbox::Sandbox(size_t max_particles, size_t pixelsX, size_t pixelsY)
	: particles(max_particles), pixelsX(pixelsX), pixelsY(pixelsY)
	{

	this->len_particles = 0;
	this->vertices = new Vertex[max_particles * 4];
	this->max_particles = max_particles;
	this->current_tick = 0;
}

Sandbox::~Sandbox() {
	delete[] this->vertices;
}

void Sandbox::calculateVertices() {
	size_t i;
	GLfloat x, y, z = 1.0f;
	GLfloat *position, *texCoords;
	Vertex *_vertices;
	GLfloat radius;

	/*
		3---2
		|   |
		0---1
	*/

	// i is particle ID
	for (i = 0; i < this->len_particles; i++)
	{

		// particle has the coordinates of its center
		// need to calculate other ones from it, rotation does not matter

		// offset by (-1, -1)
		x = ((static_cast<GLfloat>(particles.current_x[i]) * 2) / static_cast<GLfloat>(this->pixelsX)) - 1.0f;
		y = ((static_cast<GLfloat>(particles.current_y[i]) * 2) / static_cast<GLfloat>(this->pixelsY)) - 1.0f;
		radius = (static_cast<GLfloat>(GRID_PARTICLE_SIZE) * 2) / static_cast<GLfloat>(this->pixelsX); // ??????????????????????????????????????????????????????????????????

		/*
		3(-radius, +radius)				2(+radius, +radius)
		
		       				  center
		
		0(-radius, -radius)				1(+radius, -radius)
		*/

		_vertices = &(this->vertices[i * 4]);
		
		position = (&_vertices[0])->position;
		texCoords = (&_vertices[0])->texCoords;

		position[0] = x - radius;
		position[1] = y - radius;
		position[2] = z;
		texCoords[0] = 0.0f;
		texCoords[1] = 0.0f;
		// std::cout << "x:" << position[0] << " y:" << position[1] << std::endl;
		
		position = (&_vertices[1])->position;
		texCoords = (&_vertices[1])->texCoords;

		position[0] = x + radius;
		position[1] = y - radius;
		position[2] = z;
		texCoords[0] = 1.0f;
		texCoords[1] = 0.0f;

		// std::cout << "x:" << position[0] << " y:" << position[1] << std::endl;

		position = (&_vertices[2])->position;
		texCoords = (&_vertices[2])->texCoords;

		position[0] = x + radius;
		position[1] = y + radius;
		position[2] = z;
		texCoords[0] = 1.0f;
		texCoords[1] = 1.0f;

		// std::cout << "x:" << position[0] << " y:" << position[1] << std::endl;

		position = (&_vertices[3])->position;
		texCoords = (&_vertices[3])->texCoords;

		position[0] = x - radius;
		position[1] = y + radius;
		position[2] = z;
		texCoords[0] = 0.0f;
		texCoords[1] = 1.0f;
		// std::cout << "x:" << position[0] << " y:" << position[1] << std::endl;
	}
}

// returns the position in the particle matrix using the mouse pointer coordinates
void Sandbox::getMouseClickRelativePos(size_t *row, size_t *col, double xpos, double ypos) const {

}

void Sandbox::handleMouseClickAt(int button, int action, int mods, double xpos, double ypos) {
	size_t row, col;
	getMouseClickRelativePos(&row, &col, xpos, ypos);

	// if (button == GLFW_MOUSE_BUTTON_LEFT) {
	// 	if (action == GLFW_PRESS) {
	// 		Particle p = Particle(this->currently_selected);
	// 		addParticle(p, row, col);
	// 	}
	// } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
	// 	if (action == GLFW_PRESS) {
	// 		dumpParticles();
	// 	}
	// }
}

void Sandbox::handleKeyPress(int key, int scancode, int action, int mods) {
	// if (action == GLFW_RELEASE) {
	// 	if (key == GLFW_KEY_SPACE) {
	// 		saveFrame();
	// 	}
	// }
}

// #include <chrono>
// #include <thread>

// got lazy, assumes it went through a tick when this is called
void Sandbox::onUpdate(double sub_dt) {
	int i, size = this->spawners.size();
	// double sub_dt = dt / SUBSTEPS;
	double cx, cy, ax, ay, radius;
	GLfloat color[4];
	
	// spawn particles from all spawners
	for (i = 0; i < size; i++) {							// messy workaround, even messier now, need to improve in the future
		// will pass radius to it just so I don't break it, but will not be used for now
		if (this->spawners[i].nextParticle(current_tick, len_particles, &cx, &cy, &ax, &ay, &radius, color) == true) {
			addParticle(cx, cy, ax, ay, color);
		}
	}
	
	// calculate physics many times per frame for better result

	for (i = 0; i < SUBSTEPS; i++) {
		applyGravity();
		solveCollisions();
		// applyCircleConstraint();
		// applyRectangleConstraint(); removed, for better performance it is now perfomed in updatePositions
		updatePositions(sub_dt);

	}
	// std::this_thread::sleep_for(std::chrono::milliseconds(75));

	current_tick ++;
}

void Sandbox::applyGravity() {
	// TEMPORARY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	constexpr double gravity_x = 0.0;
	constexpr double gravity_y = -1000.0;

	size_t i;
	for (i = 0; i < this->len_particles; i++) {
		this->particles.accelerate(i, gravity_x, gravity_y);
	}
}

void Sandbox::addSpawner(Spawner &sp) {
	this->spawners.push_back(sp);
}

void Sandbox::addColorToParticle(size_t index, GLfloat *color) {
	int i;
	Vertex *vertex;
	for (i = 0; i < 4; i++) {
		vertex = this->vertices + (index * 4) + i;
		memcpy(vertex->color, color, 4 * sizeof(GLfloat));
	}
}

// #define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include "stb_image_write.h"

// brute force method, but works in any sandbox
GLfloat * Sandbox::getParticleColorsFromImage(const char *filename) {
	stbi_set_flip_vertically_on_load(1);
	int width, height, BPP;
	// load png
	unsigned char *image =	stbi_load(filename, &width, &height, &BPP, 4); // 4 -> RGBA

	if (!image) {
		fprintf(stderr, "Error loading image\n");
		exit(1);
	}

	// if (width != height) {
	// 	fprintf(stderr, "Non-squared images not supported for now\n");
	// 	exit(1);
	// }

	// if (BPP != 4) {
	// 	fprintf(stderr, "PNG is not RGBA\n");
	// 	exit(1);
	// }

	GLfloat *colors = (GLfloat *)calloc(len_particles * 4, sizeof(GLfloat));

	// the color of each particle results from an average of a square around it
	// inefficient but simple
	size_t i;
	for (i = 0; i < len_particles; i++) {
		getAverageColor(image, width, height, colors, i);
	}

	stbi_image_free(image);
	return colors;
}

// given a particle, will fill out its position in colors (relative to ID)
// according to average of RGBA values around it in the image
void Sandbox::getAverageColor(unsigned char *image, int width, int height, GLfloat *colors, size_t particleID) {
	// for now I assume size of 1000x1000 to be easier (same as window size and sandbox size)

	const double center_x = particles.current_x[particleID];
	const double center_y = particles.current_y[particleID];

	const int radius = static_cast<int>(GRID_PARTICLE_SIZE),
	diameter = 2 * radius;

	GLfloat *target_colors = colors + (particleID * 4);

	// for loops start at 0, need to apply this offset to be in the bottom left corner of square
	// each X value is worth 1 pixel
	// each Y value is worth <width> pixels
	// is in pixels
	const int centerX = static_cast<int>(center_x) - radius;
	const int centerY = static_cast<int>(center_y) - radius;
	
	int offsetx;
	int offsety;

	if (centerX < 0) {
		offsetx = 0;
	} else if (centerX > width - radius) {
		offsetx = width - radius;
	} else {
		offsetx = centerX;
	}

	if (centerY < 0) {
		offsety = 0;
	} else if (centerY > height - radius) {
		offsety = height - radius;
	} else {
		offsety = centerY;
	}

	const int offset = offsetx + offsety * width;

	GLfloat R = 0.0f, G = 0.0f, B = 0.0f, A = 0.0f;
	int count = 0;
	int final_offset;

	int pixel_row, pixel_col;
	for (pixel_row = 0; pixel_row < diameter; pixel_row ++) {
		for (pixel_col = 0; pixel_col < diameter; pixel_col ++) {
			final_offset = (offset + pixel_col + (pixel_row * width)) * 4;
			// printf("final offset is %d\n", final_offset);

			R += static_cast<GLfloat>(image[final_offset + 0]) / 255.0f;
			G += static_cast<GLfloat>(image[final_offset + 1]) / 255.0f;
			B += static_cast<GLfloat>(image[final_offset + 2]) / 255.0f;
			// A...

			count ++; // needed??????
		}
	}

	R /= static_cast<float>(count);
	G /= static_cast<float>(count);
	B /= static_cast<float>(count);
	A = 1.0f;

	target_colors[0] = R;
	target_colors[1] = G;
	target_colors[2] = B;
	target_colors[3] = A;
}

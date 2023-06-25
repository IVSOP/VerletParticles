#include "Sandbox.h"

#include <stdlib.h>
#include <iostream>
#include "GLErrors.h"
#include <string.h>
#include <glm/geometric.hpp>

#define SUBSTEPS 8

void Sandbox::makeVAO() {
	GLuint VAO;
	GLCall(glGenVertexArrays(1, &VAO));
	GLCall(glBindVertexArray(VAO));
	this->VAO = VAO;
}

void Sandbox::makeVBO() {
	GLuint VBO;
	GLCall(glGenBuffers(1, &VBO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * getMaxVertexCount(), nullptr, GL_DYNAMIC_DRAW));
	this->VBO = VBO;
}

void Sandbox::makeLayouts() const {
	GLuint pos_layout = 0;
	GLCall(glEnableVertexAttribArray(pos_layout));
	GLCall(glVertexAttribPointer(pos_layout, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, position)));

	GLuint color_layout = 1;
	GLCall(glEnableVertexAttribArray(color_layout));
	GLCall(glVertexAttribPointer(color_layout, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, color)));

	GLuint texture_layout = 2;
	GLCall(glEnableVertexAttribArray(texture_layout));
	GLCall(glVertexAttribPointer(texture_layout, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, texCoords)));
}

void Sandbox::makeIBO() {
	GLuint *indices = new GLuint[this->max_particles * 6];
	size_t offset = 0;

	for (size_t i = 0; i < this->max_particles * 6; i += 6) {
		indices[i + 0] = 0 + offset;
		indices[i + 1] = 1 + offset;
		indices[i + 2] = 2 + offset;

		indices[i + 3] = 2 + offset;
		indices[i + 4] = 3 + offset;
		indices[i + 5] = 0 + offset;

		offset += 4;
	}

	GLuint IBO;
	GLCall(glGenBuffers(1, &IBO));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * getMaxIndexCount(), indices, GL_STATIC_DRAW));

	this->IBO = IBO;

	delete[] indices;
}

Sandbox::Sandbox(size_t max_particles, size_t pixelsX, size_t pixelsY)
	: pixelsX(pixelsX), pixelsY(pixelsY)
	{

	this->particles = new Particle[max_particles];
	this->len_particles = 0;
	this->vertices = new Vertex[max_particles * 4];
	this->max_particles = max_particles;

	makeVAO();
	makeVBO();
	makeLayouts();
	makeIBO();
}

Sandbox::~Sandbox() {
	delete[] this->particles;
	delete[] this->vertices;

	GLCall(glDeleteVertexArrays(1, &this->VAO));
	GLCall(glDeleteBuffers(1, &this->VBO));
	GLCall(glDeleteBuffers(1, &this->IBO));
}

size_t Sandbox::getNumberOfVertices() const {
	return this->len_particles * 4;
}

size_t Sandbox::getNumberOfParticles() const {
	return this->len_particles;
}

size_t Sandbox::getNumberOfIndices() const {
	return this->len_particles * 6;
}

size_t Sandbox::getMaxNumberParticles() const {
	return this->max_particles;
}

Vertex * Sandbox::getVertices() const {
	return this->vertices;
}

size_t Sandbox::getMaxIndexCount() const {
	return this->max_particles * 6;
}

size_t Sandbox::getMaxVertexCount() const {
	return this->max_particles * 4; 
}

void Sandbox::calculateVertices() {
	size_t i;
	GLfloat x, y, z = 1.0f;
	GLfloat *position, *texCoords;
	Particle *particle;
	Vertex *_vertices;
	GLfloat radius;
	GLfloat *color; //[] = { 1.0f, 0.0f, 0.0f, 1.0f };

	/*
		3---2
		|   |
		0---1
	*/
	for (i = 0; i < this->len_particles; i++)
	{
		particle = &(this->particles[i]);

		color = particle->color;

		// particle has the coordinates of its center
		// need to calculate other ones from it, rotation does not matter

		// std::cout << "Before: " << particle->current_pos.x << " " << particle->current_pos.y << " " << particle->radius << std::endl;

		// offset by (-1, -1)
		x = ((static_cast<GLfloat>(particle->current_pos.x) * 2) / static_cast<GLfloat>(this->pixelsX)) - 1.0f;
		y = ((static_cast<GLfloat>(particle->current_pos.y) * 2) / static_cast<GLfloat>(this->pixelsY)) - 1.0f;
		radius = (static_cast<GLfloat>(particle->radius) * 2) / static_cast<GLfloat>(this->pixelsX); // ??????????????????????????????????????????????????????????????????

		// std::cout << "After: " << x << " " << y << " " << radius << std::endl;

		/*
		3(-radius, +radius)				2(+radius, +radius)
		
		       				  center
		
		0(-radius, -radius)				1(+radius, -radius)
		*/

		_vertices = &(this->vertices[i * 4]);
		
		position = (&_vertices[0])->position;
		texCoords = (&_vertices[0])->texCoords;
		
		memcpy(((&_vertices[0])->color), color, 4 * sizeof(GLfloat));

		position[0] = x - radius;
		position[1] = y - radius;
		position[2] = z;
		texCoords[0] = 0.0f;
		texCoords[1] = 0.0f;
		// std::cout << "x:" << position[0] << " y:" << position[1] << std::endl;
		
		position = (&_vertices[1])->position;
		texCoords = (&_vertices[1])->texCoords;
		
		memcpy(((&_vertices[1])->color), color, 4 * sizeof(GLfloat));

		position[0] = x + radius;
		position[1] = y - radius;
		position[2] = z;
		texCoords[0] = 1.0f;
		texCoords[1] = 0.0f;

		// std::cout << "x:" << position[0] << " y:" << position[1] << std::endl;

		position = (&_vertices[2])->position;
		texCoords = (&_vertices[2])->texCoords;
		
		memcpy(((&_vertices[2])->color), color, 4 * sizeof(GLfloat));

		position[0] = x + radius;
		position[1] = y + radius;
		position[2] = z;
		texCoords[0] = 1.0f;
		texCoords[1] = 1.0f;

		// std::cout << "x:" << position[0] << " y:" << position[1] << std::endl;

		position = (&_vertices[3])->position;
		texCoords = (&_vertices[3])->texCoords;
		
		memcpy(((&_vertices[3])->color), color, 4 * sizeof(GLfloat));

		position[0] = x - radius;
		position[1] = y + radius;
		position[2] = z;
		texCoords[0] = 0.0f;
		texCoords[1] = 1.0f;
		// std::cout << "x:" << position[0] << " y:" << position[1] << std::endl;
	}
}

void Sandbox::loadData() {
	calculateVertices();

	// VAO
	GLCall(glBindVertexArray(this->VAO));

	// VBO and load its data
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->VBO));
	GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * getNumberOfVertices(), getVertices()));
}

// returns the position in the particle matrix using the mouse pointer coordinates
void Sandbox::getMouseClickRelativePos(size_t *row, size_t *col, double xpos, double ypos) const {
	// *row = ypos / this->height_constant;
	// *col = xpos / this->width_constant;
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

	// if (action == GLFW_PRESS) {

	// }
}

void Sandbox::onUpdate(double dt) {
	int i, size = this->spawners.size();
	Particle p; // is there a better way to do this?????
	double sub_dt = dt / SUBSTEPS;
	
	// spawn particles from all spawners
	for (i = 0; i < size; i++) {
		if (this->spawners[i].nextParticle(&p) == true) {
			addParticle(p);
		}
	}
	
	// calculate physics many times per frame for better result

	// THIS IS MESSY WHEN SPAWNING PARTICLES INSIDE OF EACH OTHER
	// can also only keep the solve collisions in a loop and the rest out of the loop
	for (i = 0; i < SUBSTEPS; i++) {
		applyGravity();
		solveCollisions();
		// applyCircleConstraint();
		applyRectangleConstraint();
		updatePositions(sub_dt);
	}
}

void Sandbox::applyGravity() {
	// TEMPORARY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	pVec2 gravity = {0.0f, -1000.0f};
	size_t i;

	for (i = 0; i < this->len_particles; i++) {
		this->particles[i].accelerate(gravity);
	}
}

void Sandbox::addSpawner(Spawner &sp) {
	this->spawners.push_back(sp);
}

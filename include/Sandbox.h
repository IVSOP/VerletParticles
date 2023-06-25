#ifndef SANDBOX_H
#define SANDBOX_H

#include "Spawner.h"
#include <vector>
#include "Grid.h"

typedef struct {
	GLfloat position[3];
	GLfloat color[4]; // RGBA
	// texture ID
	// GLuint texSlot; // for now I don't set it as I can assume it is 0
	// texture coords
	GLfloat texCoords[2];
} Vertex;

class Sandbox {

	private:
		GLuint VAO;
		GLuint VBO;
		GLuint IBO;

		size_t pixelsX, pixelsY;

		Particle *particles;
		size_t len_particles;
		Vertex *vertices;
		// size_t len_vertices; not needed, it is always len_particles * 4
		size_t max_particles;

		Grid grid;

		std::vector<Spawner> spawners;

		void makeVAO();
		void makeVBO();
		void makeLayouts() const;
		void makeIBO();
	
		void calculateVertices();

	public:
		Sandbox() = delete;
		Sandbox(size_t n_particles, size_t pixelsX, size_t pixelsY);
		~Sandbox();

		void addParticle(Particle &particle);

		size_t getNumberOfVertices() const;
		size_t getNumberOfParticles() const;
		size_t getNumberOfIndices() const;
		size_t getMaxNumberParticles() const;

		Vertex * getVertices() const;

		size_t getMaxIndexCount() const;
		size_t getMaxVertexCount() const;

		// loads VAO and sets all data it needs to
		void loadData();

		void getMouseClickRelativePos(size_t *row, size_t *col, double xpos, double ypos) const;
		void handleMouseClickAt(int button, int action, int mods, double xpos, double ypos);
		void handleKeyPress(int key, int scancode, int action, int mods);

		void onUpdate(double dt);
		void updatePositions(double dt);
		void applyGravity();
		void applyCircleConstraint();
		void applyRectangleConstraint();
		void solveCollisions();
		void solveCollisionsGrid();
		void collideParticles(Particle *p1, Particle *p2); // could receive p1 radius to be faster
		void collideParticlesGrid(GridCell *centerCell, GridCell *secondCell);

		void addSpawner(Spawner &sp);




		void gridify();
};

#endif

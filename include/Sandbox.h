#ifndef SANDBOX_H
#define SANDBOX_H

#include "Spawner.h"
#include <vector>

#define SUBSTEPS 8

typedef struct {
	GLfloat position[3];
	GLfloat color[4]; // RGBA
	// texture ID
	// GLuint texSlot; // for now I don't set it as I can assume it is 0
	// texture coords
	GLfloat texCoords[2];
} Vertex;

class Sandbox {
	protected:
		Particle *particles;
		size_t len_particles;
		
		Vertex *vertices;
		// size_t len_vertices; not needed, it is always len_particles * 4
		size_t max_particles;

		void addColorToParticle(size_t index, GLfloat *color);

		size_t pixelsX, pixelsY;

	private:
		GLuint VAO;
		GLuint VBO;
		GLuint IBO;

		size_t current_tick;

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

		virtual void addParticle(Particle &particle) = 0;

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

		void onUpdate(double sub_dt);
		virtual void updatePositions(double dt) = 0;
		void applyGravity();
		virtual void applyCircleConstraint() = 0;
		virtual void applyRectangleConstraint() = 0;
		virtual void solveCollisions() = 0;

		void addSpawner(Spawner &sp);
};

#endif

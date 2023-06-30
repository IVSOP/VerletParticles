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
		
		Vertex *vertices;
		// size_t len_vertices; not needed, it is always len_particles * 4

		void addColorToParticle(size_t index, GLfloat *color);

		size_t pixelsX, pixelsY;
		size_t current_tick;

		std::vector<Spawner> spawners; // should be private
	private:
		GLuint VAO;
		GLuint VBO;
		GLuint IBO;
	

	public:
		Sandbox() = delete;
		Sandbox(size_t n_particles, size_t pixelsX, size_t pixelsY);
		~Sandbox();

		size_t len_particles;
		size_t max_particles;

		virtual void addParticle(Particle &particle) = 0;

		virtual void clear() = 0; // allways needs to set len_particles = 0, current_tick = 0 and reset spawners, maybe wrap this with something and dont make the entire thing virtual

		size_t getNumberOfParticles() const;
		size_t getMaxNumberParticles() const;

		Vertex * getVertices() const;

		void calculateVertices();

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

		GLfloat * getParticleColorsFromImage(const char *filename);
		void getAverageColor(unsigned char *image, int width, int height, GLfloat *colors, Particle *p);
};

#endif

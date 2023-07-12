#ifndef SANDBOX_H
#define SANDBOX_H

#include "Spawner.h"
#include <vector>
#include "ParticleArray.h"

#define GRID_PARTICLE_SIZE 5 // VERY bad workaround, in the future change mentions to this

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
		ParticleArray particles;

		Vertex *vertices;
		// size_t len_vertices; not needed, it is always len_particles * 4

		void addColorToParticle(size_t index, GLfloat *color);

		size_t pixelsX, pixelsY;
		size_t current_tick;

		std::vector<Spawner> spawners; // should be private, this is temporary

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

		// ineficient, temporary
		virtual void addParticle(double cx, double cy, double ax, double ay, GLfloat color[4]) = 0;

		virtual void clear() = 0; // allways needs to set len_particles = 0, current_tick = 0 and reset spawners, maybe wrap this with something and dont make the entire thing virtual

		constexpr size_t getNumberOfParticles() {
			return this->len_particles;
		}

		constexpr size_t getMaxNumberParticles() {
			return this->max_particles;
		}

		constexpr Vertex * getVertices() {
			return this->vertices;
		}

		void calculateVertices();

		void getMouseClickRelativePos(size_t *row, size_t *col, double xpos, double ypos) const;
		void handleMouseClickAt(int button, int action, int mods, double xpos, double ypos);
		void handleKeyPress(int key, int scancode, int action, int mods);

		void onUpdate(double sub_dt);
		virtual void updatePositions(double dt) = 0;
		void applyGravity();
		// virtual void applyCircleConstraint() = 0;
		// virtual void applyRectangleConstraint() = 0;
		virtual void solveCollisions() = 0;

		void addSpawner(Spawner &sp);

		GLfloat * getParticleColorsFromImage(const char *filename);
		void getAverageColor(unsigned char *image, int width, int height, GLfloat *colors, size_t particleID);
};

#endif

#ifndef PARTICLE_H
#define PARTICLE_H

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <cstring>

typedef glm::dvec2 pVec2;

struct Particle {
	public:
		// values are in pixels
		// positions (0,0) start at bottom left corner (opengl (-1, -1))
		// does not make sense for radius to be in half pixels
		// position does not either but whatever it simplifies the math
		size_t radius;
		pVec2 old_pos;
		pVec2 current_pos;
		pVec2 accel;
		GLfloat color[4];

		unsigned int ID;

		// double timeLived;


		// GLuint textID;

		// measures are in pixels
		Particle(pVec2 current_pos, size_t radius, pVec2 accel);
		Particle(pVec2 current_pos, size_t radius, pVec2 accel, GLfloat color[4]);
		Particle(pVec2 current_pos, size_t radius);
		Particle(pVec2 current_pos, size_t radius, GLfloat color[4]);

		Particle(const Particle &particle);

		Particle() = default;
		~Particle() = default;

		Particle& operator = (const Particle& particle) {
			this->current_pos = particle.current_pos;
			this->old_pos = particle.old_pos;
			this->accel = particle.accel;
			this->radius = particle.radius;
			this->ID = particle.ID;
			std::memcpy(this->color, particle.color, 4 * sizeof(GLfloat));
			return *this;
		}

		void updatePosition(double dt);
		void accelerate(pVec2 accel);
};

#endif

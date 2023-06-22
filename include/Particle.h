#ifndef PARTICLE_H
#define PARTICLE_H

#include <GL/glew.h>

class Particle {
	public:
		size_t posX, posY, radius;
		// GLuint textID;

		// measures are in pixels
		Particle(size_t posX, size_t posY, size_t radius)
		: posX(posX), posY(posY), radius(radius)
		{}

		Particle()
		: posX(0), posY(0), radius(0)
		{}

		Particle(const Particle &particle) {
			this->posX = particle.posX;
			this->posY = particle.posY;
			this->radius = particle.radius;
		}

		// wtf is going on here
		// Particle& operator = (Particle&& p) { type=p.type; p.type=None; return *this; }	// transfer ownership
		Particle& operator = (const Particle& p) {
			this->posX = p.posX;
			this->posY = p.posY;
			this->radius = p.radius;
			return *this;
		}

};

#endif

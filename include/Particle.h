#ifndef PARTICLE_H
#define PARTICLE_H

#include <GL/glew.h>
#include "Vec2.h"
#include <iostream>

typedef size_t ParticleData;

class Particle {
	public:
		ParticleData radius;
		vec2<ParticleData> old_pos;
		vec2<ParticleData> current_pos;
		vec2<ParticleData> accel;


		// GLuint textID;

		// measures are in pixels
		Particle(vec2<ParticleData> current_pos, ParticleData radius, vec2<ParticleData> accel);

		Particle(vec2<ParticleData> current_pos, ParticleData radius);

		Particle(const Particle &particle);

		Particle() = default;
		~Particle() = default;

		// wtf is going on here
		// Particle& operator = (Particle&& p) { type=p.type; p.type=None; return *this; }	// transfer ownership
		Particle& operator = (const Particle& particle) {
			this->current_pos = particle.current_pos;
			this->accel = particle.accel;
			this->radius = particle.radius;
			return *this;
		}

		void updatePosition(double dt) {
			vec2<ParticleData> velocity = current_pos - old_pos; // const???

			this->old_pos = current_pos;
			std::cout << "Before: " << current_pos.x << " " << current_pos.y << std::endl;
			this->current_pos = (current_pos + velocity + accel) * dt * dt;
			std::cout << "After: " << current_pos.x << " " << current_pos.y << std::endl;
			std::cout << "Accel was " << accel.x << " " << accel.y << std::endl;
			this->accel = {0, 0};
		}

		void accelerate(vec2<ParticleData> accel) {
			this->accel += accel;
		}
};

#endif

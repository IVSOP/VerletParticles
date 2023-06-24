#ifndef SPAWNER_H
#define SPAWNER_H

#include "Particle.h"
#include <optional>

typedef bool spawnerFunc (Particle *p, unsigned long int count, void *userData);

// some examples of spawner funcs
// goes in a circle
bool inCircle(Particle *p, unsigned long int count, void *userData);
// from center rotates in a circle
bool centerSpawner(Particle *p, unsigned long int count, void *userData);

class Spawner {
	private:
		unsigned long int count;
		spawnerFunc *func;
		void *userData;

	public:
		Spawner() = delete;
		Spawner(spawnerFunc *func);
		Spawner(spawnerFunc *func, void *userData);
		~Spawner() = default;

		Spawner(const Spawner &spawner);

		Spawner& operator = (const Spawner& spawner) {
			this->count = spawner.count;
			this->func = spawner.func;
			this->userData = spawner.userData;
			return *this;
		}

		bool nextParticle(Particle *p);
};


#endif

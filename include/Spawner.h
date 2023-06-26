#ifndef SPAWNER_H
#define SPAWNER_H

#include "Particle.h"

typedef bool spawnerFunc (Particle *p, unsigned long int count, void *userData);

// some examples of spawner funcs


bool inCircle(Particle *p, unsigned long int count, void *userData);

struct centerSpawnerInfo {
	pVec2 center;

	centerSpawnerInfo(pVec2 _center)
		: center(_center)
	{}
};

// from center rotates in a circle
bool centerSpawner(Particle *p, unsigned long int count, void *userData);
bool centerSpawnerFixedSize(Particle *p, unsigned long int count, void *userData);

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

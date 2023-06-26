#ifndef SPAWNER_H
#define SPAWNER_H

#include "Particle.h"


struct spawnerInfo {
	pVec2 center;
	double particle_radius;

	spawnerInfo(pVec2 _center, double _particle_radius)
		: center(_center), particle_radius(_particle_radius)
	{}
};

typedef bool spawnerFunc (Particle *p, unsigned long int count, spawnerInfo *info);

// some examples of spawner funcs


bool inCircle(Particle *p, unsigned long int count, spawnerInfo *info);
// from center rotates in a circle
bool centerSpawner(Particle *p, unsigned long int count, spawnerInfo *info);
bool centerSpawnerFixedSize(Particle *p, unsigned long int count, spawnerInfo *info);

class Spawner {
	private:
		unsigned long int count;
		spawnerFunc *func;
		spawnerInfo *info;

	public:
		Spawner() = delete;
		// Spawner(spawnerFunc *func);
		Spawner(spawnerFunc *func, spawnerInfo *info);
		~Spawner() = default;

		Spawner(const Spawner &spawner);

		Spawner& operator = (const Spawner& spawner) {
			this->count = spawner.count;
			this->func = spawner.func;
			this->info = spawner.info;
			return *this;
		}

		bool nextParticle(Particle *p);
};


#endif

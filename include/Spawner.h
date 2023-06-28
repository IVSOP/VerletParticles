#ifndef SPAWNER_H
#define SPAWNER_H

#include "Particle.h"


struct spawnerInfo {
	pVec2 center;
	double particle_radius;
	GLfloat *color_feed; // color lookup for each particle ID

	spawnerInfo(pVec2 _center, double _particle_radius, GLfloat *_color_feed)
		: center(_center), particle_radius(_particle_radius), color_feed(_color_feed)
	{}
};

typedef bool spawnerFunc (Particle *p, unsigned long int count, spawnerInfo *info);

// some examples of spawner funcs


bool inCircle(Particle *p, unsigned long int count, spawnerInfo *info);
bool inCircleReverse(Particle *p, unsigned long int count, spawnerInfo *info);
// from center rotates in a circle
bool centerSpawner(Particle *p, unsigned long int count, spawnerInfo *info);
bool centerSpawnerFixedSize(Particle *p, unsigned long int count, spawnerInfo *info);

class Spawner {
	private:
		unsigned long int count;
		spawnerFunc *func;
		spawnerInfo *info;

		// particles start spawning on start_tick, end in end_tick
		size_t start_tick, end_tick;

	public:
		
		Spawner() = delete;
		// Spawner(spawnerFunc *func);
		Spawner(size_t start_tick, size_t end_tick, spawnerFunc *func, spawnerInfo *info);
		~Spawner() = default;

		Spawner(const Spawner &spawner);

		Spawner& operator = (const Spawner& spawner) {
			this->count = spawner.count;
			this->func = spawner.func;
			this->info = spawner.info;
			this->start_tick = spawner.start_tick;
			this->end_tick = spawner.end_tick;
			return *this;
		}

		// if it chose to spawn particle, returns true
		// else returns false
		// it would be more efficient to delete spawner when ticks run out, but wont do it for now
		bool nextParticle(size_t current_tick, Particle *p);
};


#endif

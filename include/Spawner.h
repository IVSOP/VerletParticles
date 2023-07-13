#ifndef SPAWNER_H
#define SPAWNER_H

// for GLfloat, need to rework this
#include <GL/glew.h>
#include "ParticleArray.h"

struct spawnerInfo {
	pFloat cx, cy, ax, ay;
	pFloat particle_radius;
	GLfloat *color_feed; // color lookup for each particle ID

	spawnerInfo(pFloat _cx, pFloat _cy, pFloat _particle_radius, GLfloat *_color_feed)
		: cx(_cx), cy(_cy), particle_radius(_particle_radius), color_feed(_color_feed)
	{}

	spawnerInfo(pFloat _cx, pFloat _cy, pFloat _ax, pFloat _ay, pFloat _particle_radius, GLfloat *_color_feed)
		: cx(_cx), cy(_cy), ax(_ax), ay(_ay), particle_radius(_particle_radius), color_feed(_color_feed)
	{}
};

typedef bool spawnerFunc (unsigned long int count, spawnerInfo *info, unsigned int ID, pFloat *cx, pFloat *cy, pFloat *ax, pFloat *ay, pFloat *radius, GLfloat color[4]);

// some examples of spawner funcs


bool inCircle(unsigned long int count, spawnerInfo *info, unsigned int ID, pFloat *cx, pFloat *cy, pFloat *ax, pFloat *ay, pFloat *radius, GLfloat color[4]);
bool inCircleReverse(unsigned long int count, spawnerInfo *info, unsigned int ID, pFloat *cx, pFloat *cy, pFloat *ax, pFloat *ay, pFloat *radius, GLfloat color[4]);
// from center rotates in a circle
bool centerSpawner(unsigned long int count, spawnerInfo *info, unsigned int ID, pFloat *cx, pFloat *cy, pFloat *ax, pFloat *ay, pFloat *radius, GLfloat color[4]);
bool centerSpawnerFixedSize(unsigned long int count, spawnerInfo *info, unsigned int ID, pFloat *cx, pFloat *cy, pFloat *ax, pFloat *ay, pFloat *radius, GLfloat color[4]);

bool fixedSpawner(unsigned long int count, spawnerInfo *info, unsigned int ID, pFloat *cx, pFloat *cy, pFloat *ax, pFloat *ay, pFloat *radius, GLfloat color[4]);

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

		void reset() {
			this->count = 0;
		}

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
		bool nextParticle(size_t current_tick, unsigned int ID, pFloat *cx, pFloat *cy, pFloat *ax, pFloat *ay, pFloat *radius, GLfloat color[4]);
};


#endif

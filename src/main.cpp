#include <iostream>
#include "Renderer.h"
#include "GLErrors.h"

// void saveFrame() {
// 	char filename[] = "frame.tga";
// 	int* buffer = new int[ 1000 * 1000 * 3 ];
// 					 // width, height
// 	glReadPixels( 0, 0, 1000, 1000, GL_BGR, GL_UNSIGNED_BYTE, buffer );

// 	FILE   *out = fopen(filename, "w");
// 	short  TGAhead[] = {0, 2, 0, 0, 0, 0, 1000, 1000, 24};
// 	fwrite(&TGAhead, sizeof(TGAhead), 1, out);
// 	fwrite(buffer, 3 * 1000 * 1000, 1, out);
// 	fclose(out);
// }

int main() {

	// because I'm pretty sure my cpu doesn't support them
	puts("Will use 256 bit vector registers and not 128 or 512. Checking for AVX2 support");

	// #if defined(__AVX2__)
	//      this checks if it has been compiled with support for avx2
	// #endif

	if(__builtin_cpu_supports("avx2")){
        puts("It is supported");
    } else {
		puts("Not supported, exiting");
		exit(1);
	}


	// int width, height;
	// glfwGetWindowSize(window, &width, &height);
#define CHECK_ALL_CELL_COLLISIONS // if you desable this, it is much faster but at a risk on jankyness
	GridSandbox sandbox(20000, 1000, 1000); // why tf can I change this and not window pixels and it just works???????? i dont even know anymore, it works too well its just inconsistent

	Renderer renderer(1000, 1000, &sandbox);
	// renderer.addSandbox(&sandbox);

	// center point and particle radius, if they decide to use it
	// spawnerInfo info1(pVec2(500, 500), GRID_PARTICLE_SIZE, nullptr);
	// spawnerInfo info2(pVec2(500, 500), GRID_PARTICLE_SIZE, nullptr);
	spawnerInfo info3(GRID_PARTICLE_SIZE, 1000 - GRID_PARTICLE_SIZE, 325000.0, 0, GRID_PARTICLE_SIZE, nullptr);
	spawnerInfo info4(GRID_PARTICLE_SIZE, 1000 - (GRID_PARTICLE_SIZE * 3), 320000.0, 0, GRID_PARTICLE_SIZE, nullptr);
	spawnerInfo info5(GRID_PARTICLE_SIZE, 1000 - (GRID_PARTICLE_SIZE * 5), 315000.0, 0, GRID_PARTICLE_SIZE, nullptr);
	spawnerInfo info6(1000 - GRID_PARTICLE_SIZE, 1000 - GRID_PARTICLE_SIZE, - 325000.0, 0, GRID_PARTICLE_SIZE, nullptr);
	spawnerInfo info7(GRID_PARTICLE_SIZE, 1000 - (GRID_PARTICLE_SIZE * 7), 305000.0, 0, GRID_PARTICLE_SIZE, nullptr);
	spawnerInfo info8(GRID_PARTICLE_SIZE, 1000 - (GRID_PARTICLE_SIZE * 9), 295000.0, 0, GRID_PARTICLE_SIZE, nullptr);
	spawnerInfo info9(GRID_PARTICLE_SIZE, 1000 - (GRID_PARTICLE_SIZE * 11), 285000.0, 0, GRID_PARTICLE_SIZE, nullptr);
	spawnerInfo info10(GRID_PARTICLE_SIZE, 1000 - (GRID_PARTICLE_SIZE * 13), 275000.0, 0, GRID_PARTICLE_SIZE, nullptr);

	// Spawner spawner1(0, 2200, centerSpawnerFixedSize, &info2);
	// sandbox.addSpawner(spawner1);

	// Spawner spawner2(0, 2200, inCircle, &info2);
	// sandbox.addSpawner(spawner2);
	
	// Spawner spawner3(0, 2200, inCircleReverse, &info2);
	// sandbox.addSpawner(spawner3);

	Spawner spawner4(0, 3700, fixedSpawner, &info3);
	sandbox.addSpawner(spawner4);
	Spawner spawner5(50, 3700, fixedSpawner, &info4);
	sandbox.addSpawner(spawner5);
	Spawner spawner6(100, 3600, fixedSpawner, &info5);
	sandbox.addSpawner(spawner6);
	Spawner spawner7(150, 3700, fixedSpawner, &info6);
	sandbox.addSpawner(spawner7);
	Spawner spawner8(200, 3300, fixedSpawner, &info7);
	sandbox.addSpawner(spawner8);
	Spawner spawner9(250, 3200, fixedSpawner, &info8);
	sandbox.addSpawner(spawner9);
	Spawner spawner10(300, 3100, fixedSpawner, &info9);
	sandbox.addSpawner(spawner10);
	Spawner spawner11(350, 3000, fixedSpawner, &info10);
	sandbox.addSpawner(spawner11);

	// done through renderer for now because only it knopws the simulation substep
	renderer.simulate(3700);

		// // get colors according to particle position
		std::cout << "Calculating colors from this position" << std::endl;
		GLfloat * color_feed = sandbox.getParticleColorsFromImage("res/radiation_symbol.png");
		// GLfloat * color_feed = nullptr;

		std::cout << "Finished, clearing and starting visual simulation" << std::endl;

		sandbox.clear();

		// info2.color_feed = color_feed;
		info3.color_feed = color_feed;
		info4.color_feed = color_feed;
		info5.color_feed = color_feed;
		info6.color_feed = color_feed;
		info7.color_feed = color_feed;
		info8.color_feed = color_feed;
		info9.color_feed = color_feed;
		info10.color_feed = color_feed;



	renderer.setup();
	renderer.mainLoop();



	// free(color_feed);

	return 0;
}

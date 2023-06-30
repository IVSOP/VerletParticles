# VerletParticles

## Very simple particle physics simulation, with image rendering

Inspiration: https://youtu.be/lS_qeBy3aQI

Example:
<div id="Installing"/>
<p float="left">
  <img src="res/Screenshot1.png" width="175" />
  <img src="res/Screenshot2.png" width="175" /> 
  <img src="res/Screenshot3.png" width="175" />
</p>

*******
 1. [Installing](#Installing)
 2. [Compiling/Running](#Compiling/Running)
 3. [Current state](#Current_state)
 4. [Code structure](#Code_structure)
 5. [Documentation](#Documentation)
*******


## Installing

Currently make install has not been implemented

Download source code **recursivelly**, i.e. `git clone git@github.com:IVSOP/VerletParticles.git --recursive`

*******

<div id="Compiling/Running"/>

## Compiling/Running

   ### Dependendies
   * CMake

   ### Compiling and Running
   * Use the script `run.sh` to compile and run
   * For now, image is not passed as argument but its path is used in main, near sandboxes and spawners

*******

<div id="Current_state"/>

## Current state
This is a personal project I created to learn OpenGL and physics simulations. It is coded in C++ and simulates particle movement using Verlet integration.

It now works in rendering an image from particles, but only in a square simulation space with all particles having the same size.

Note: only tested RGBA (4 channel) squared PNG, with 8 bit pixels

Working algorithms:
* grid (GridSandbox) (particles are placed in a static grid, and must ALLWAYS be of the same size (use GRID_PARTICLE_SIZE in GridSandbox.h))

Stopped working due to some changes, will fix in the future
* brute force (RegularSandbox)
* sorting (SortedSandbox) (glitches when particles have different sizes)
* quad tree (QuadTreeSandbox) (glitches when particles collide near quadrant borders)

Algorithms being worked on:
* quad tree array (quad tree is allocated as an array)

For now there is no SIMD, gpu calculations, etc.

The constraint is currently a square but there is also a circle constraint, commented out in Sandbox.cpp onUpdate()

Spawners are easy to make, currently there is only one being used but other examples were provided

*******

<div id="Code_structure"/>

## Code structure

The structure is currently messy but will be fixed

To run, you need a Renderer, Sandbox and (optional) Spawners

The sandbox can have many implementations, currently GridSandbox is the only one working and also the fastest

Spawners will, given a spawnFunc, spawn particles in a given certain place, acceleration, color, etc.

Better explanation in the future

*******

<div id="Documentation"/>

## Documentation

What?
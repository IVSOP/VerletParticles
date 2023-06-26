# VerletParticles

## Very simple particle physics simulation

*******
 1. [Installing](#Installing)
 2. [Compiling/Running](#Compiling/Running)
 3. [Current state](#Current_state)
 4. [Code structure](#Code_structure)
 5. [Documentation](#Documentation)
*******

<div id="Installing"/>

## Installing

Currently make install was not implemented

Download source code **recursivelly**, i.e. `git clone git@github.com:IVSOP/VerletParticles.git --recursive`

*******

<div id="Compiling/Running"/>

## Compiling/Running

   ### Dependendies
   * CMake

   ### Compiling and Running
   * Use the script `run.sh` to compile and run

*******

<div id="Current_state"/>

## Current state
This is a personal project I created to learn OpenGL and physics simulations. It is coded in C++ and simulates particle movement using Verlet integration.

Working algorithms:
* brute force (RegularSandbox)
* quad tree (QuadTreeSandbox) (glitches when particles collide near quadrant borders)
* sorting (SortedSandbox) (glitches when particles have different sizes)

Algorithms being worked on:
* grid (particles are placed in a static grid)
* quad tree array (quad tree is allocated as an array)

For now there is no SIMD, gpu calculations, etc.

The constraint is currently a square but there is also a circle constraint, commented out in Sandbox.cpp onUpdate()

Spawners are easy to make, currently there is only one being used but other examples were provided

*******

<div id="Code_structure"/>

## Code structure

The structure is currently messy but will be fixed

In main, I make a Renderer, a Sandbox and some Spawners. The sandbox is added to the renderer and the spawners are added to the sandbox

The renderer, every frame, requests an update from the sandbox, that must spawn particles from the spawner, set the GPU data needed for the display, and solve collisions

The Sandbox class provides some of these functionalities, while collisions and updates must be handled in a child class. For example, RegularSandbox, SortedSandbox, etc. are all children of Sandbox

*******

<div id="Documentation"/>

## Documentation

What?
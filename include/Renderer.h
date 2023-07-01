#ifndef RENDERER_H
#define RENDERER_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Sandbox.h"
#include "GridSandbox.h"
// #include "RegularSandbox.h"
// #include "QuadTreeSandbox.h"
// #include "SortedSandbox.h"

#include <vector>
#include <GLFW/glfw3.h>

#define FPS 60

class Renderer {
	private:
		GLFWwindow *window;
		Sandbox * sandbox;
		ImGuiIO io;

		void setCallbacks(GLFWwindow* window);
		static void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void onMouseMove(GLFWwindow* window, double xpos, double ypos);
		static void onMouseClick(GLFWwindow* window, int button, int action, int mods);

		double timestep_acc;
		double timestep;
		double substep;

		GLuint VAO;
		GLuint VBO;
		GLuint IBO;
		GLuint program;

		void makeVAO();
		void makeVBO();
		void makeLayouts() const;
		void makeIBO();
		void loadSandboxData();

		constexpr size_t getNumberOfVertices() {
			return sandbox->len_particles * 4;
		}

		constexpr size_t getMaxIndexCount() {
			return sandbox->max_particles * 6;
		}

		constexpr size_t getMaxVertexCount() {
			return sandbox->max_particles * 4; 
		}

		constexpr size_t getNumberOfIndices() {
			return sandbox->len_particles * 6;
		}

	public:

		void mainLoop();

		void tick() {
			this->sandbox->onUpdate(substep);
		}

		Renderer(int pixel_width, int pixel_height, Sandbox *sandbox);

		~Renderer();

		// pointer to a sandbox, so that changing it changes the render
		// void addSandbox(Sandbox *sandbox);

		void renderSandbox(double frameDeltaTime);
		void renderSandboxWithoutTick(double frameDeltaTime);
		void renderSandboxWithoutTick();

		void onKeyPress(int key, int scancode, int action, int mods);
		void onMouseMove(double xpos, double ypos);
		void onMouseClick(int button, int action, int mods);
};

#endif
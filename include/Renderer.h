#ifndef RENDERER_H
#define RENDERER_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Sandbox.h"
#include "GridSandbox.h"
#include "RegularSandbox.h"
#include "QuadTreeSandbox.h"
#include "SortedSandbox.h"

#include <vector>
#include <GLFW/glfw3.h>

#define FPS 60

class Renderer {
	private:
		GLFWwindow *window;
		Sandbox * sandbox;
		ImGuiIO& io = ImGui::GetIO();

		void setCallbacks(GLFWwindow* window);
		static void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void onMouseMove(GLFWwindow* window, double xpos, double ypos);
		static void onMouseClick(GLFWwindow* window, int button, int action, int mods);

		double timestep_acc;
		double timestep;
		double substep;

	public:

		void tick() {
			this->sandbox->onUpdate(substep);
		}

		Renderer(GLFWwindow* window) { 
			this->timestep_acc = 0;
			this->window = window;
			setCallbacks(window);
			timestep = 1.0f / FPS;
			substep = timestep / SUBSTEPS;
		}

		~Renderer() = default;

		// pointer to a sandbox, so that changing it changes the render
		void addSandbox(Sandbox *sandbox);

		// void renderSandbox(Sandbox * sandbox) const;
		// void renderAllSandboxes(double dt);
		void renderSandbox(double frameDeltaTime);

		void onKeyPress(int key, int scancode, int action, int mods);
		void onMouseMove(double xpos, double ypos);
		void onMouseClick(int button, int action, int mods);
};

#endif
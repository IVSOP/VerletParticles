#ifndef RENDERER_H
#define RENDERER_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Sandbox.h"
#include "GridSandbox.h"
#include "RegularSandbox.h"
#include "SortedSandbox.h"
#include "QuadTreeSandbox.h"

#include <vector>
#include <GLFW/glfw3.h>

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

	public:

		void tick(double dt) {
			this->sandbox->onUpdate(dt);
		}

		static const double timestep;

		Renderer(GLFWwindow* window) { this->timestep_acc = 0; this->window = window; setCallbacks(window); };
		~Renderer() = default;

		// pointer to a sandbox, so that changing it changes the render
		void addSandbox(Sandbox *sandbox);

		// void renderSandbox(Sandbox * sandbox) const;
		// void renderAllSandboxes(double dt);
		void renderSandbox(double dt);

		void onKeyPress(int key, int scancode, int action, int mods);
		void onMouseMove(double xpos, double ypos);
		void onMouseClick(int button, int action, int mods);
};

#endif
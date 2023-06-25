#include "Renderer.h"
#include "GLErrors.h"
#include <iostream>

const double Renderer::timestep = 1.0f / (60.0f * 8.0f);

void Renderer::setCallbacks(GLFWwindow* window) {
	glfwSetWindowUserPointer(window, this);
	
	glfwSetKeyCallback(window, onKeyPress);
	glfwSetCursorPosCallback(window, onMouseMove);
	glfwSetMouseButtonCallback(window, onMouseClick);
}

void Renderer::addSandbox(Sandbox *sandbox) {
	this->sandbox = sandbox;
}

// void Renderer::renderSandbox(Sandbox * sandbox) const {
// 	sandbox->loadData();
// 	GLCall(glDrawElements(GL_TRIANGLES, sandbox->getNumberOfIndices(), GL_UNSIGNED_INT, 0));
// }

// void Renderer::renderAllSandboxes(double dt) {
// 	this->timestep_acc += this->timestep;
// 	if (timestep_acc >= timestep) {
// 		timestep_acc = 0;
// 		tick(dt);
// 	}

// 	for (Sandbox *sandbox : this->sandboxes) {
// 		renderSandbox(sandbox);
// 	}
// }

void Renderer::renderSandbox(double dt) {
	this->timestep_acc += this->timestep;
	if (timestep_acc >= timestep) {
		timestep_acc = 0;
		tick(dt);
	}

	sandbox->loadData();
	GLCall(glDrawElements(GL_TRIANGLES, sandbox->getNumberOfIndices(), GL_UNSIGNED_INT, 0));
}

// called by setKeyCallback
inline void Renderer::onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Renderer *renderer = (Renderer *) glfwGetWindowUserPointer(window);
	renderer->onKeyPress(key, scancode, action, mods);
}

// called when a key is pressed, for the renderer instance in question
void Renderer::onKeyPress(int key, int scancode, int action, int mods) {
	if (!this->io.WantCaptureKeyboard) {
		if (action == GLFW_PRESS) {
			switch (key) {
				case (GLFW_KEY_ESCAPE):
					glfwSetWindowShouldClose(this->window, GL_TRUE);
					break;
				
				default:
					// for (Sandbox * sandbox : this->sandboxes) {
						sandbox->handleKeyPress(key, scancode, action, mods);
					// }
			}
		}
	}
}

inline void Renderer::onMouseMove(GLFWwindow* window, double xpos, double ypos) {
	Renderer *renderer = (Renderer *) glfwGetWindowUserPointer(window);
	renderer->onMouseMove(xpos, ypos);
}

void Renderer::onMouseMove(double xpos, double ypos) {
	// ImGuiIO& io = ImGui::GetIO(); // should I get this every frame or just once???
	if (!this->io.WantCaptureMouse) {
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			// mouse being dragged while clicked
			// for (Sandbox *sandbox : this->sandboxes) {
				sandbox->handleMouseClickAt(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, 0, xpos, ypos);
			// }
		}

	}

	this->io.MousePos = ImVec2(xpos, ypos);

}

inline void Renderer::onMouseClick(GLFWwindow* window, int button, int action, int mods) {
	Renderer *renderer = (Renderer *) glfwGetWindowUserPointer(window);
	renderer->onMouseClick(button, action, mods);
}

#include <iostream>
void Renderer::onMouseClick(int button, int action, int mods) {
	if (!this->io.WantCaptureMouse) {
		// How do I know which sandbox was clicked???
		// For now just pass it on to all of them I guess
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		// for (Sandbox *sandbox : this->sandboxes) {
			sandbox->handleMouseClickAt(button, action, mods, xpos, ypos);
		// }
	} else {
		this->io.AddMouseButtonEvent(button, action == GLFW_PRESS ? true : false);
	}
}

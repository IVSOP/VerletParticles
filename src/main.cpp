#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include "GLErrors.h"
#include "Particle.h"
#include "Sandbox.h"
#include "Renderer.h"
#include "Texture.h"

// will break on huge files, bad error checking
const GLchar *readFromFile(char *filepath) {
	FILE *fp = fopen(filepath, "r");
	if (fp == NULL) {
		fprintf(stderr, "%s\n", filepath);
		perror("Invalid file path");
		exit(1);
	}
	fseek(fp, 0L, SEEK_END);
	unsigned long size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	GLchar *ret = new GLchar[size + 1];
	size_t charread = fread(ret, sizeof(GLchar), size, fp);
	if (charread != size) {
		fprintf(stderr, "Did not read all chars: %ld vs %ld\n", size, charread);
		exit(1);
	}
	ret[charread] = '\0';
	fclose(fp);
	return ret;
}

int main() {
	if (!glfwInit()) {
		perror("GLFW window failed to initiate");
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(1000, 1000, "Hello World", NULL, NULL);

	if (window == NULL) {
		perror("GLFW window failed to create");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW failed\n";
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 450");
	// io.ConfigFlags |= .....

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	std::cout << glGetString(GL_VERSION) << std::endl;

	// During init, enable debug output
	glEnable( GL_DEBUG_OUTPUT );
	glDebugMessageCallback( openglCallbackFunction, NULL );

	//////////////////////////////////////////// Sandbox and renderer instance

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	Sandbox sandbox(1000, 1000, 1000);

	Renderer renderer(window);
	renderer.pushSandbox(&sandbox);

	//////////////////////////////////////////// Creating shaders and making program out of the shaders
	GLCall(const GLuint program = glCreateProgram());

	GLuint VS, FS;
	{
		char filename[] = "res/default.vert";
		const GLchar *vertex_shader = readFromFile(filename);
		GLCall(VS = glCreateShader(GL_VERTEX_SHADER));
		GLCall(glShaderSource(VS, 1, &vertex_shader, NULL));
		GLCall(glCompileShader(VS));
		GLCall(checkErrorInShader(VS));
		GLCall(glAttachShader(program, VS));
		delete[] vertex_shader;
	}

	{
		char filename[] = "res/default.frag";
		const GLchar *fragment_shader = readFromFile(filename);
		GLCall(FS = glCreateShader(GL_FRAGMENT_SHADER));
		GLCall(glShaderSource(FS, 1, &fragment_shader, NULL));
		GLCall(glCompileShader(FS));
		GLCall(checkErrorInShader(FS));
		GLCall(glAttachShader(program, FS));
	}
	
	GLCall(glLinkProgram(program));

	int error;
	glGetProgramiv(program, GL_LINK_STATUS, &error);
	char idk[1000];
	if (!error) {
		int len;
		glGetProgramInfoLog(program, sizeof(idk), &len, idk);
		std::cout << "ERROR! " << idk << std::endl;
	}
	
	GLCall(glValidateProgram(program));


	//////////////////////////////////////////// Creating the circle texture. This can andle many textures but I will only need this one
	GLuint texID, slot = 0;
	makeTexture(&texID, "res/circle.png", slot);
	bindTexture(texID, slot);

	// add particles
	Particle p(vec2<ParticleData>(0, 0), 100);

	sandbox.addParticle(p);

	// cleanup
	GLCall(glDeleteShader(VS));
	GLCall(glDeleteShader(FS));
	// unbind
	GLCall(glBindVertexArray(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

	GLCall(glfwSwapInterval(1)); // hardcoded sync with monitor fps

	GLCall(glUseProgram(program));
	// this is not really needed (???) since it is allways 0 but I made it anyway
	GLCall(GLint loc = glGetUniformLocation(program, "u_texture"));
	// slot is allways 0
	GLCall(glUniform1i(loc, (GLint)0));

	float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	int radius = 50, posX = 0, posY = 0;
	while (!glfwWindowShouldClose(window)) {

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// this sets a constant color
		// glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui::Begin("Spawn a circle");

		
		ImGui::ColorPicker4("##ColorPicker", color, false | ImGuiColorEditFlags_DisplayRGB, NULL);

		ImGui::Text("x:");
		ImGui::SameLine();
		ImGui::InputInt("##x", &posX);
		ImGui::Text("y:");
		ImGui::SameLine();
		ImGui::InputInt("##y", &posY);
		ImGui::Text("radius:");
		ImGui::SameLine();
		ImGui::InputInt("##radius", &radius);

		if (ImGui::Button("Add")) {
			std::cout << "Adding particle " << posX << " " << posY << " " << radius << std::endl;
			p = Particle(vec2<ParticleData>(posX, posY), radius);
			sandbox.addParticle(p);
		}

		if (ImGui::Button("Tick")) {
			renderer.tick(1.0f / 60.0f);
		}

		ImGui::ShowDemoWindow();

		ImGui::End();
		ImGui::Render();

		renderer.renderAllSandboxes(1.0f / 60.0f);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// end cleanup
	glDeleteProgram(program);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

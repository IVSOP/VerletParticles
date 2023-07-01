#include "Renderer.h"
#include "GLErrors.h"
#include <iostream>
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

void Renderer::setCallbacks(GLFWwindow* window) {
	glfwSetWindowUserPointer(window, this);
	
	glfwSetKeyCallback(window, onKeyPress);
	glfwSetCursorPosCallback(window, onMouseMove);
	glfwSetMouseButtonCallback(window, onMouseClick);
}

Renderer::Renderer(int pixel_width, int pixel_height, Sandbox *sandbox) {
	this->sandbox = sandbox;
	this->timestep_acc = 0;
	
	if (!glfwInit()) {
		perror("GLFW window failed to initiate");
		exit(1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(pixel_width, pixel_height, "Hello World", NULL, NULL);
	this->window = window;

	if (window == NULL) {
		perror("GLFW window failed to create");
		glfwTerminate();
		exit(1);
	}
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW failed\n" << std::endl;
		exit(1);
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	this->io = io;
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
	
	setCallbacks(window);







	makeVAO();
	makeVBO();
	makeLayouts();
	makeIBO();









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
	if (!error) {
		char idk[1000];
		int len;
		glGetProgramInfoLog(program, sizeof(idk), &len, idk);
		std::cout << "ERROR! " << idk << std::endl;
	}
	
	GLCall(glValidateProgram(program));
	this->program = program;

	//////////////////////////////////////////// Creating the circle texture. This can andle many textures but I will only need this one
	GLuint texID, slot = 0;
	makeTexture(&texID, "res/circle.png", slot);
	bindTexture(texID, slot);

	// cleanup to ensure everything has to be correctly bound to work
	GLCall(glDeleteShader(VS));
	GLCall(glDeleteShader(FS));
	// unbind
	GLCall(glBindVertexArray(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

	timestep = 1.0f / FPS;
	substep = timestep / SUBSTEPS;
}

Renderer::~Renderer() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	GLCall(glDeleteVertexArrays(1, &this->VAO));
	GLCall(glDeleteBuffers(1, &this->VBO));
	GLCall(glDeleteBuffers(1, &this->IBO));
	GLCall(glDeleteProgram(program));

	glfwDestroyWindow(window);
	glfwTerminate();

}

void Renderer::mainLoop() {




	GLCall(glUseProgram(program));
	// this is not really needed (???) since it is allways 0 but I made it anyway
	GLCall(GLint loc = glGetUniformLocation(program, "u_texture"));
	// slot is allways 0
	GLCall(glUniform1i(loc, (GLint)0));

	GLCall(glfwSwapInterval(1)); // hardcoded sync with monitor fps
	GLfloat color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	int radius = GRID_PARTICLE_SIZE, posX = 100, posY = 500;

	double previousFrameTime = glfwGetTime(), newFrameTime;
	// wtf???????????????????????????????????????????????????? twice??? and without this it doesnt work??????????????????????????????????????????
	double lastFrameTime = glfwGetTime(),
	currentFrameTime;

	Particle p;

	while (!glfwWindowShouldClose(window))
	{
		currentFrameTime = glfwGetTime();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// this sets a constant color
		// glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui::Begin("Menu");

		
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
			// std::cout << "Adding particle " << posX << " " << posY << " " << radius << " R " << color[0] << " G " << color[1] << " B " << color[2] << std::endl;
			p = Particle(pVec2(posX, posY), radius, color);
			sandbox->addParticle(p);
		}

		if (ImGui::Button("Tick")) {
			tick();
		}

		if (ImGui::Button("Render")) {
			renderSandbox(currentFrameTime - lastFrameTime);
		}

		// if (ImGui::Button("Dump grid")) {
		// 	// will only compile if sandbox is grid sandbox
		// 	sandbox.dumpGridToFile();
		// }

		// ImGui::ShowDemoWindow();

		ImGui::End();
		ImGui::Render();

		renderSandbox(currentFrameTime - lastFrameTime);
		// renderer.renderSandboxWithoutTick(currentFrameTime - lastFrameTime);

		// saveFrame();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		newFrameTime = glfwGetTime();
		std::cout << "FPS: " << 1 / (newFrameTime - previousFrameTime) << std::endl;
		previousFrameTime = newFrameTime;

		glfwSwapBuffers(window);
		glfwPollEvents();

		lastFrameTime = currentFrameTime;
	}
}

void Renderer::makeVAO() {
	GLuint VAO;
	GLCall(glGenVertexArrays(1, &VAO));
	GLCall(glBindVertexArray(VAO));
	this->VAO = VAO;
}

void Renderer::makeVBO() {
	GLuint VBO;
	GLCall(glGenBuffers(1, &VBO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * getMaxVertexCount(), nullptr, GL_DYNAMIC_DRAW));
	this->VBO = VBO;
}

void Renderer::makeLayouts() const {
	GLuint pos_layout = 0;
	GLCall(glEnableVertexAttribArray(pos_layout));
	GLCall(glVertexAttribPointer(pos_layout, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, position)));

	GLuint color_layout = 1;
	GLCall(glEnableVertexAttribArray(color_layout));
	GLCall(glVertexAttribPointer(color_layout, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, color)));

	GLuint texture_layout = 2;
	GLCall(glEnableVertexAttribArray(texture_layout));
	GLCall(glVertexAttribPointer(texture_layout, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, texCoords)));
}

void Renderer::makeIBO() {
	GLuint *indices = new GLuint[sandbox->max_particles * 6];
	size_t offset = 0;

	for (size_t i = 0; i < sandbox->max_particles * 6; i += 6) {
		indices[i + 0] = 0 + offset;
		indices[i + 1] = 1 + offset;
		indices[i + 2] = 2 + offset;

		indices[i + 3] = 2 + offset;
		indices[i + 4] = 3 + offset;
		indices[i + 5] = 0 + offset;

		offset += 4;
	}

	GLuint IBO;
	GLCall(glGenBuffers(1, &IBO));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * getMaxIndexCount(), indices, GL_STATIC_DRAW));

	this->IBO = IBO;

	delete[] indices;
}

constexpr size_t Renderer::getNumberOfVertices() const {
	return sandbox->len_particles * 4;
}

constexpr size_t Renderer::getMaxIndexCount() const {
	return sandbox->max_particles * 6;
}

constexpr size_t Renderer::getMaxVertexCount() const {
	return sandbox->max_particles * 4; 
}

constexpr size_t Renderer::getNumberOfIndices() const {
	return sandbox->len_particles * 6;
}

// void Renderer::addSandbox(Sandbox *sandbox) {
// 	this->sandbox = sandbox;
// }

void Renderer::loadSandboxData() {
	sandbox->calculateVertices(); // should it be sandbox doing this????????????????????????????????????

	// VAO
	GLCall(glBindVertexArray(this->VAO)); // does not need to happen!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// VBO and load its data
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->VBO)); // does not need to happen!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * getNumberOfVertices(), sandbox->getVertices()));
}

void Renderer::renderSandbox(double frameDeltaTime) {
	this->timestep_acc += frameDeltaTime;
	if (timestep_acc >= timestep) {
		timestep_acc -= timestep;
		tick();
	}

	loadSandboxData();
	GLCall(glDrawElements(GL_TRIANGLES, getNumberOfIndices(), GL_UNSIGNED_INT, 0));
}

void Renderer::renderSandboxWithoutTick(double frameDeltaTime) {
	this->timestep_acc += frameDeltaTime;

	loadSandboxData();
	GLCall(glDrawElements(GL_TRIANGLES, getNumberOfIndices(), GL_UNSIGNED_INT, 0));
}

void Renderer::renderSandboxWithoutTick() {
	loadSandboxData();
	GLCall(glDrawElements(GL_TRIANGLES, getNumberOfIndices(), GL_UNSIGNED_INT, 0));
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
					sandbox->handleKeyPress(key, scancode, action, mods);
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
			sandbox->handleMouseClickAt(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, 0, xpos, ypos);
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
		sandbox->handleMouseClickAt(button, action, mods, xpos, ypos);
	} else {
		this->io.AddMouseButtonEvent(button, action == GLFW_PRESS ? true : false);
	}
}

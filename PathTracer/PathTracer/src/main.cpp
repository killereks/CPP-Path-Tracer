#include "imgui_impl_opengl3.h"
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <filesystem>

#include <Engine.h>
#include <Shader.h>
#include <Mesh.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

static void GLAPIENTRY MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

int main(int argc, char* argv[])
{
	GLFWwindow* window;

	if (!glfwInit())
	{
		return -1;
	}

	int width = 1080;
	int height = 720;

	window = glfwCreateWindow(width, height, "Title", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

	io.DisplaySize = ImVec2((float)width, (float)height);

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();

	ImGui::StyleColorsDark();

	glDebugMessageCallback(MessageCallback, 0);

	std::string dir = std::string(std::filesystem::path(argv[0]).parent_path().u8string().c_str());
	dir += "\\";

	std::cout << "Shader path: " << dir;

	Engine engine(dir);

	int frameCount = 0;

	double previousTime = glfwGetTime();

	engine.OnResize(width, height);

	while (!glfwWindowShouldClose(window))
	{
		double now = glfwGetTime();
		double deltaTime = now - previousTime;

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		frameCount++;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode, nullptr);

		ImGui::Begin("Stats");
		ImGui::Text("Frame time: %f ms", deltaTime);
		ImGui::Text("Estimated fps: %f", (1.0 / deltaTime));
		ImGui::End();

		glfwGetWindowSize(window, &width, &height);

		engine.Update();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		glfwSwapBuffers(window);
		glfwPollEvents();

		engine.InputHandler(window, deltaTime);

		previousTime = now;

		// check for window resize
		int newWidth, newHeight;
		glfwGetWindowSize(window, &newWidth, &newHeight);

		if (newWidth != width || newHeight != height)
		{
			width = newWidth;
			height = newHeight;

			glViewport(0, 0, width, height);

			engine.OnResize(width, height);
		}
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();

	return 0;
}
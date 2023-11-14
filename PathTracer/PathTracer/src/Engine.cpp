#include "Engine.h"

#include <string>
#include <imgui.h>

#include <algorithm>
#include <execution>

Engine::Engine(std::string executablePath){
	std::string vertex = executablePath + "shaders\\vertex.glsl";
	std::string fragment = executablePath + "shaders\\fragment.glsl";

	defaultShader = new Shader(vertex.c_str(), fragment.c_str());

	std::string computeShaderPath = executablePath + "shaders\\compute.glsl";

	computeShader = new ComputeShader(computeShaderPath.c_str());

	mesh = new Mesh();
	camera = new Camera(80.0f);

	// setup hashes
	cameraPositionHash = defaultShader->getUniformLocation("cameraPos");
	cameraFwdHash = defaultShader->getUniformLocation("cameraFwd");
	cameraUpHash = defaultShader->getUniformLocation("cameraUp");
	cameraRightHash = defaultShader->getUniformLocation("cameraRight");

	maxBouncesHash = defaultShader->getUniformLocation("maxBounces");
	samplesPerPixelHash = defaultShader->getUniformLocation("samplesPerPixel");

	timeHash = defaultShader->getUniformLocation("time");
}

Engine::~Engine()
{
	delete defaultShader;
	delete mesh;
	delete camera;
}

void Engine::Update()
{
	frames++;

	DrawRayTracedImage();
	Accumulate();

	camera->DrawUI();
}

void Engine::Accumulate() {
	computeShader->use();

	computeShader->setInt("width", screenWidth);
	computeShader->setInt("height", screenHeight);
	computeShader->setFloat("totalFrames", static_cast<float>(frames));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shaderTexture);

	glBindImageTexture(1, finalImageTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glDispatchCompute((GLuint)screenWidth, (GLuint)screenHeight, 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	ImGui::Begin("Final Image");
	ImVec2 windowSize = ImGui::GetContentRegionAvail();
	ImGui::Image((void*)finalImageTexture, windowSize);
	ImGui::End();
}

void Engine::DrawRayTracedImage() {
	glBindFramebuffer(GL_FRAMEBUFFER, shaderFrameBuffer);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	defaultShader->use();

	defaultShader->setVec3(cameraPositionHash, camera->transform.GetPosition());
	defaultShader->setVec3(cameraFwdHash, camera->transform.GetForward());
	defaultShader->setVec3(cameraRightHash, camera->transform.GetRight());
	defaultShader->setVec3(cameraUpHash, camera->transform.GetUp());

	defaultShader->setInt(maxBouncesHash, maxBounces);
	defaultShader->setInt(samplesPerPixelHash, samplesPerPixel);

	defaultShader->setFloat(timeHash, glfwGetTime());

	mesh->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ImGui::Begin("Shader Image");
	ImVec2 windowSize = ImGui::GetContentRegionAvail();
	ImGui::Image((void*)shaderTexture, windowSize);
	ImGui::End();

	ImGui::Begin("Settings");
	ImGui::SliderInt("Max Bounces", &maxBounces, 1, 30);
	ImGui::SliderInt("Samples Per Pixel", &samplesPerPixel, 1, 500);
	ImGui::End();
}

void Engine::InputHandler(GLFWwindow* window, float dt)
{
	bool cameraChanged = false;

	float speed = 2.0f;

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed *= 5.0f;
	}

	speed *= dt;

	int mouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

	if (mouseState == GLFW_PRESS)
	{
		double mousePosXNow = 0.0f;
		double mousePosYNow = 0.0f;

		glfwGetCursorPos(window, &mousePosXNow, &mousePosYNow);

		double mouseDeltaX = mousePosXNow - mousePosX;
		double mouseDeltaY = mousePosYNow - mousePosY;

		mousePosX = mousePosXNow;
		mousePosY = mousePosYNow;

		camera->AddPitch(mouseDeltaY * 0.25f);
		camera->AddYaw(mouseDeltaX * 0.25f);

		cameraChanged = true;

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else if (mouseState == GLFW_RELEASE)
	{
		glfwGetCursorPos(window, &mousePosX, &mousePosY);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera->transform.Translate(camera->transform.GetForward() * speed);
		cameraChanged = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera->transform.Translate(camera->transform.GetForward() * -speed);
		cameraChanged = true;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera->transform.Translate(camera->transform.GetRight() * speed);
		cameraChanged = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera->transform.Translate(camera->transform.GetRight() * -speed);
		cameraChanged = true;
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		camera->transform.Translate(camera->transform.GetUp() * speed);
		cameraChanged = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		camera->transform.Translate(camera->transform.GetUp() * -speed);
		cameraChanged = true;
	}

	if (cameraChanged)
	{
		// clear accumulation buffer
		frames = 0;
	}
}

void Engine::OnResize(int width, int height)
{
	screenWidth = width;
	screenHeight = height;
	frames = 0;

	glDeleteFramebuffers(1, &shaderFrameBuffer);
	glDeleteTextures(1, &shaderTexture);

	// FRAMEBUFFER
	glGenFramebuffers(1, &shaderFrameBuffer);

	// TEXTURE
	glGenTextures(1, &shaderTexture);

	// setup textures
	glBindTexture(GL_TEXTURE_2D, shaderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, shaderFrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shaderTexture, 0);

	// FINAL IMAGE TEXTURE
	glDeleteTextures(1, &finalImageTexture);

	glCreateTextures(GL_TEXTURE_2D, 1, &finalImageTexture);
	glTextureParameteri(finalImageTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(finalImageTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureStorage2D(finalImageTexture, 1, GL_RGBA32F, width, height);
	glBindImageTexture(0, finalImageTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	// SSBO
	glDeleteBuffers(1, &ssbo);

	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, width * height * sizeof(glm::vec3), NULL, GL_DYNAMIC_COPY);
}

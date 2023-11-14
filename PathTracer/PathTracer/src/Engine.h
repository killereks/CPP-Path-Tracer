#pragma once

#include <Shader.h>
#include <Mesh.h>
#include "GLFW/glfw3.h"
#include "Camera.h"
#include <ComputeShader.h>

class Engine
{
	Shader* defaultShader;
	Mesh* mesh;

	Camera* camera;

	double mousePosX, mousePosY;

	unsigned int shaderFrameBuffer;
	unsigned int shaderTexture;

	unsigned int finalImageTexture;

	unsigned int ssbo;

	int frames = 0;

	ComputeShader* computeShader;

	// SETTINGS
	int maxBounces = 5;
	int samplesPerPixel = 1;

	int screenWidth = 1080;
	int screenHeight = 720;

	// HASHES
	int cameraPositionHash = 0;
	int cameraFwdHash = 0;
	int cameraUpHash = 0;
	int cameraRightHash = 0;

	int maxBouncesHash = 0;
	int samplesPerPixelHash = 0;

	int timeHash = 0;

public:
	Engine(std::string executablePath);
	~Engine();

	void Update();

	void DrawRayTracedImage();
	void Accumulate();

	void InputHandler(GLFWwindow* window, float dt);

	void OnResize(int width, int height);
};


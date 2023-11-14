#pragma once

#include <vector>
#include <glm.hpp>

class Mesh
{
	std::vector<glm::vec3> vertices = {
		glm::vec3(1.0f,   1.0f, 0.0f),  // top right
		glm::vec3(1.0f,  -1.0f, 0.0f),  // bottom right
		glm::vec3(-1.0f, -1.0f, 0.0f),  // bottom left
		glm::vec3(-1.0f,  1.0f, 0.0f)   // top left 
	};

	std::vector<unsigned int> indices = {
		0, 1, 3,
		1, 2, 3
	};

	unsigned int VAO;

	unsigned int VBO;
	unsigned int IBO;

public:
	Mesh();
	~Mesh();

	void Draw();
};


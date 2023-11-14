#pragma once

#include <glm.hpp>
#include <gtc/quaternion.hpp>

class Transform
{

	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

public:

	Transform()
	{
		position = glm::vec3(0.0, 0.0, 0.0);
		rotation = glm::quat();
		scale = glm::vec3(1.0, 1.0, 1.0);
	}

	const inline void Translate(glm::vec3 WorldDelta)
	{
		position += WorldDelta;
	}

	const inline glm::vec3 GetForward() const
	{
		return rotation * glm::vec3(0, 0, 1);
	}
	const inline glm::vec3 GetRight() const
	{
		return rotation * glm::vec3(1, 0, 0);
	}
	const inline glm::vec3 GetUp() const
	{
		return rotation * glm::vec3(0, 1, 0);
	}

	const inline glm::vec3 GetPosition() const { return position; }
	const inline glm::vec3 GetEulerAngles() const { return glm::eulerAngles(rotation) * 3.14159f / 180.0f; }
	const inline glm::vec3 GetScale() const { return scale; }

	const inline glm::vec3 SetPosition(glm::vec3 pos)
	{
		position = pos;
	}

	const inline void SetEulerAngles(float pitch, float yaw, float roll)
	{
		pitch = glm::radians(pitch);
		yaw = glm::radians(yaw);
		roll = glm::radians(roll);

		rotation = glm::quat(glm::vec3(pitch, yaw, roll));
	}
};


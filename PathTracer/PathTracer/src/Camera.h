#pragma once

#include <glm.hpp>
#include "Transform.h"

class Camera
{
	float pitch;
	float yaw;

	float fov;

public:
	Transform transform;

	Camera(float fov)
	{
		this->fov = fov;

		this->pitch = 0.0f;
		this->yaw = 0.0f;
	}

	inline void SetPitch(float _pitch)
	{
		_pitch = glm::clamp(_pitch, -89.0f, 89.0f);
		pitch = _pitch;

		transform.SetEulerAngles(pitch, yaw, 0.0f);
	}

	inline void SetYaw(float _yaw)
	{
		yaw = _yaw;

		transform.SetEulerAngles(pitch, yaw, 0.0f);
	}

	inline void AddPitch(float _deltaPitch)
	{
		SetPitch(pitch + _deltaPitch);
	}

	inline void AddYaw(float _deltaYaw)
	{
		SetYaw(yaw + _deltaYaw);
	}

	void DrawUI();
};


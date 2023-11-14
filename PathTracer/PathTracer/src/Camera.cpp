#include "Camera.h"
#include "imgui.h"

void Camera::DrawUI()
{
	glm::vec3 forward = transform.GetForward();
	glm::vec3 right = transform.GetRight();
	glm::vec3 up = transform.GetUp();

	ImGui::Begin("Camera Info");
	ImGui::Text("Pitch: %f", pitch);
	ImGui::Text("Yaw: %f", yaw);
	ImGui::Separator();
	ImGui::Text("Forward %f %f %f", forward.x, forward.y, forward.z);
	ImGui::Text("Right %f %f %f", right.x, right.y, right.z);
	ImGui::Text("Up %f %f %f", up.x, up.y, up.z);
	ImGui::Separator();
	ImGui::Text("Position: %f %f %f", transform.GetPosition().x, transform.GetPosition().y, transform.GetPosition().z);
	ImGui::End();
}

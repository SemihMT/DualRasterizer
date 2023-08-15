#pragma once
#include "Math.h"
class Camera
{
public:
	Camera(const dae::Vector3& _origin, float _fovAngle, float aspectRatio);
	~Camera();

	Camera(const Camera& other) = delete;
	Camera(Camera&& other) = delete;
	Camera& operator=(const Camera& other) = delete;
	Camera& operator=(Camera&& other) = delete;

	dae::Matrix GetViewMatrix() { return m_ViewMatrix; }
	dae::Matrix GetInvViewMatrix() { return m_InvViewMatrix; }
	dae::Matrix GetProjectionMatrix() { return m_ProjectionMatrix; }
	dae::Vector3 GetOrigin() const { return m_Origin; }
	
	void Update(const dae::Timer* pTimer);
private:
	dae::Matrix m_InvViewMatrix{};
	dae::Matrix m_ViewMatrix{};
	dae::Matrix m_ProjectionMatrix{};

	dae::Vector3 m_Origin{};
	dae::Vector3 m_Forward{ dae::Vector3::UnitZ };
	dae::Vector3 m_Up{ dae::Vector3::UnitY };
	dae::Vector3 m_Right{ dae::Vector3::UnitX };

	float m_FovAngle{};
	float m_Fov{ tanf((m_FovAngle * dae::TO_RADIANS) / 2.f) };
	float m_AspectRatio{ 1 };

	float m_TotalPitch{};
	float m_TotalYaw{};

	float m_Near{ 0.1f };
	float m_Far{ 100.f };


	void CalculateViewMatrix();
	void CalculateProjectionMatrix();
};


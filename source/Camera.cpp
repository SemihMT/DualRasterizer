#include "pch.h"
#include "Camera.h"


Camera::Camera(const dae::Vector3& _origin, float _fovAngle, float aspectRatio) :
	m_Origin{ _origin },
	m_FovAngle{ _fovAngle },
	m_AspectRatio{ aspectRatio }
{
	m_Fov = tanf((m_FovAngle * dae::TO_RADIANS) / 2.f);
}

Camera::~Camera()
{
}

void Camera::Update(const dae::Timer* pTimer)
{
	const float deltaTime = pTimer->GetElapsed();

	const float movementSpeed{ 25.f };
	const float rotationSpeed{ .1f };
	//Keyboard Input
	const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

	//Mouse Input
	int mouseX{}, mouseY{};
	const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

	if (pKeyboardState[SDL_SCANCODE_W])
	{
		m_Origin += m_Forward * movementSpeed * deltaTime;
	}
	if (pKeyboardState[SDL_SCANCODE_A])
	{
		m_Origin -= m_Right * movementSpeed * deltaTime;
	}
	if (pKeyboardState[SDL_SCANCODE_S])
	{
		m_Origin -= m_Forward * movementSpeed * deltaTime;
	}
	if (pKeyboardState[SDL_SCANCODE_D])
	{
		m_Origin += m_Right * movementSpeed * deltaTime;
	}
	if (mouseState == SDL_BUTTON(1))
	{
		m_Origin -= m_Forward * float(mouseY) * deltaTime;
		m_TotalYaw += float(mouseX) * rotationSpeed * deltaTime;
	}
	if (mouseState == SDL_BUTTON(3))
	{
		m_TotalYaw += float(mouseX) * rotationSpeed * deltaTime;
		m_TotalPitch -= float(mouseY) * rotationSpeed * deltaTime;
	}
	if (mouseState == (SDL_BUTTON(1) | SDL_BUTTON(3)))
	{
		m_Origin += m_Up * float(mouseY) * deltaTime;
	}

	const dae::Matrix finalRotation{ dae::Matrix::CreateRotation(m_TotalPitch,m_TotalYaw,0) };
	m_Forward = finalRotation.TransformVector(dae::Vector3::UnitZ).Normalized();
	m_Right = finalRotation.TransformVector(dae::Vector3::UnitX).Normalized();

	//Update Matrices
	CalculateViewMatrix();
	CalculateProjectionMatrix();
}
void Camera::CalculateViewMatrix()
{
	//m_Right = dae::Vector3::Cross(dae::Vector3::UnitY, m_Forward).Normalized();
	m_Up = dae::Vector3::Cross(m_Forward, m_Right).Normalized();

	m_InvViewMatrix =
	{
		m_Right,
		m_Up,
		m_Forward,
		m_Origin
	};

	m_ViewMatrix = m_InvViewMatrix.Inverse();
	//m_ViewMatrix = dae::Matrix::CreateLookAtLH()
	//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatlh
}
void Camera::CalculateProjectionMatrix()
{
	m_ProjectionMatrix = dae::Matrix::CreatePerspectiveFovLH(m_Fov, m_AspectRatio, m_Near, m_Far);
	//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectivefovlh
}
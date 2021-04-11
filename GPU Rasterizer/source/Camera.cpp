#include "pch.h"
#include "Camera.h"
#include "SDL.h"

using namespace Elite;

Camera::Camera(const FPoint3& pos, float angle, float aspectRatio)
	:m_Onb{ FMatrix4::Identity() }
	, m_Position{ pos }
	, m_WordUp{ 0.f,1.f,0.f }
	, m_RotationChange{}
	, m_RotationMatrix3x3{}
	, m_CameraChanged{ true }
	,m_UsingDirectX{true}
	, m_RotationCorrection{ 1 }
	, m_PositionChange{}
	, m_RotationSpeed{ 0.7f }
	, m_MovementSpeedMouse{ 0.7f }
	, m_MovementSpeedKeys{ 50.f }
	, m_ForwardV{ 0.f,0.f,1.f }
	, m_NearPlane{ 0.1f }
	, m_FarPlane{ 250.f }
	, m_AspectRatio{ aspectRatio }
	, m_Fov{ float(tan((angle / 180.f * M_PI) / 2.f)) }
{
	//to make sure its already called once before the update is called
	SDL_GetRelativeMouseState(NULL, NULL);
}

void Camera::UpdateTransformation()
{
	IPoint2 mouseChange{ };
	Uint32 button{ SDL_GetRelativeMouseState(&mouseChange.x, &mouseChange.y) };
	FPoint2 mouseChangeFloat{ float(mouseChange.x), float(mouseChange.y) };

	if (SDL_BUTTON(1) + SDL_BUTTON(3) == button) //left and right mouse
	{
		ChangePosition(FVector3{ mouseChangeFloat.x, -mouseChangeFloat.y ,0.f });
	}
	else if (SDL_BUTTON(1) == button) //left mouse button
	{
		ChangePosition(FVector3{ 0.f,0.f, mouseChangeFloat.y });
		ChangeRotation(-mouseChangeFloat.x, false);
	}
	else if (SDL_BUTTON(3) == button) //right mouse button
	{
		ChangeRotation(-mouseChangeFloat.x, false);
		ChangeRotation(mouseChangeFloat.y * m_RotationCorrection, true);
	}
}

void Camera::ToggleDirectX()
{
	m_UsingDirectX = !m_UsingDirectX;
	UpdateOnbMatrix();
}

void Camera::Update(float elepsedTime)
{
	//mouse transformation
	UpdateTransformation();

	//keyboard transformation

	if (m_CameraChanged)
	{
		UpdateOnbMatrix();
	}

	if (m_GoingDown)
	{
	UpdatePositionZ(elepsedTime, false);
	}
	if (m_GoingUp)
	{
	UpdatePositionZ(elepsedTime, true);
	}
	if (m_GoingForward)
	{
		UpdatePositionX(elepsedTime, true);
	}
	if (m_GoingBackwards)
	{
		UpdatePositionX(elepsedTime, false);
	}
}

void Camera::UpdatePositionZ(float elapsedSec, bool up)
{
	if (up)
	{
		ChangePosition(FVector3{ elapsedSec * m_MovementSpeedKeys,0.f,0.f });
	}
	else
	{
		ChangePosition(FVector3{ elapsedSec * -m_MovementSpeedKeys,0.f,0.f });
	}
}

void Camera::UpdatePositionX(float elapsedSec, bool forward)
{
	if (forward)
	{
		ChangePosition(FVector3{ 0.f,0.f,elapsedSec * -m_MovementSpeedKeys });
	}
	else
	{
		ChangePosition(FVector3{ 0.f,0.f,elapsedSec * m_MovementSpeedKeys });
	}
}



FMatrix4 Camera::GetProjectionMatrixLeftHanded() const
{
	FMatrix4 projectionMatrix{ FMatrix4::Identity() };

	projectionMatrix.data[0][0] = 1.f / (m_AspectRatio * m_Fov);
	projectionMatrix.data[1][1] = 1.f / m_Fov;
	projectionMatrix.data[2][2] = m_FarPlane / (m_FarPlane - m_NearPlane);
	projectionMatrix.data[3][2] = -(m_NearPlane * m_FarPlane) / (m_FarPlane - m_NearPlane);
	projectionMatrix.data[2][3] = 1.f;// copy z value into w, + because left handed system

	return projectionMatrix;
}

FMatrix4 Camera::GetProjectionMatrixRightHanded() const
{
	FMatrix4 projectionMatrix{ FMatrix4::Identity() };

	projectionMatrix.data[0][0] = 1.f / (m_AspectRatio * m_Fov);
	projectionMatrix.data[1][1] = 1.f / m_Fov;
	projectionMatrix.data[2][2] = m_FarPlane / (m_NearPlane - m_FarPlane);
	projectionMatrix.data[3][2] = (m_FarPlane * m_NearPlane) / (m_NearPlane - m_FarPlane);
	projectionMatrix.data[2][3] = -1.f;// copy z value into w, - because right handed system

	return projectionMatrix;
}

FMatrix4 Camera::GetVieuwMatrix() const
{
	return { Inverse(m_Onb) };
}

const FPoint3& Camera::GetPos() const
{
	return m_Position;
}

void Camera::SetGoingForward(bool value)
{
	m_GoingForward = value;
}

void Camera::SetGoingBackward(bool value)
{
	m_GoingBackwards = value;
}

void Camera::SetGoingUp(bool value)
{
	m_GoingUp = value;
}

void Camera::SetGoingDown(bool value)
{
	m_GoingDown = value;
}

void Camera::ChangeRotation(float degrees, bool aroundX)
{
	m_CameraChanged = true;

	if (aroundX)
	{
		m_RotationChange.x = float((degrees * m_RotationSpeed) / 180.f * M_PI);
	}
	else
	{
		m_RotationChange.y = float((degrees * m_RotationSpeed) / 180.f * M_PI);
	}
}

void Camera::ChangePosition(const FVector3& change)
{
	m_CameraChanged = true;

	m_PositionChange = m_PositionChange + change * m_MovementSpeedMouse;
}

void Camera::UpdateOnbMatrix()
{
	//rotation
	m_RotationMatrix3x3 = MakeRotationX(m_RotationChange.x);
	m_RotationMatrix3x3 *= MakeRotationY(m_RotationChange.y);

	m_ForwardV = m_RotationMatrix3x3 * m_ForwardV;
	Normalize(m_ForwardV);

	if (m_ForwardV.z > 0)  m_RotationCorrection = -1;
	else m_RotationCorrection = 1;

	//left handed axis system
	FVector3 forwardVLeft{ -m_ForwardV.x, -m_ForwardV.y, m_ForwardV.z };
	FVector3 rightVLeft{ Cross(m_WordUp,forwardVLeft) };
	Normalize(rightVLeft);
	FVector3 upVLeft{ Cross(forwardVLeft,rightVLeft) };
	Normalize(upVLeft);

	//right handed axis system
	FVector3 rightVRight{ Cross(m_WordUp,m_ForwardV) };
	Normalize(rightVRight);
	FVector3 upVRight{ Cross(m_ForwardV,rightVRight) };
	Normalize(upVRight);

	//update ovement
	m_Position += rightVRight * m_PositionChange.x;
	m_Position += upVRight * m_PositionChange.y;
	m_Position += m_ForwardV * m_PositionChange.z;

	if (m_UsingDirectX)
	{
	m_Onb = { FVector4{rightVLeft,0.f}, FVector4{upVLeft,0.f}, FVector4{forwardVLeft,0.f}, FVector4{m_Position.x,m_Position.y,-m_Position.z,1.f} };//- z value  for left handed system
	}
	else
	{
		m_Onb = { FVector4{rightVRight,0.f}, FVector4{upVRight,0.f}, FVector4{m_ForwardV,0.f}, FVector4{m_Position.x,m_Position.y,m_Position.z,1.f} };
	}

	m_CameraChanged = false;

	//reset change
	m_RotationChange = { 0.f,0.f };
	m_PositionChange = { 0.f,0.f,0.f };
}


#include "Camera.h"
#include "SDL.h"

using namespace Elite;

Camera::Camera(const FPoint3& pos, float angle)
	:m_Onb{ FMatrix4::Identity() }
	, m_Position{ pos }
	, m_Angle{ angle }
	, m_WordUp{ 0.f,1.f,0.f }
	, m_RotationChange{}
	, m_RotationMatrix3x3{}
	, m_CameraChanged{ true }
	, m_RotationCorrection{ 1 }
	, m_PositionChange{}
	, m_RotationSpeed{ 7.f }
	, m_ForwardV{ 0.f,0.f,1.f }
	, m_NearPlane{ 0.1f }
	, m_FarPlane{ 100.f }
{
	//to make sure its already called once before the update is called
	SDL_GetRelativeMouseState(NULL, NULL);
}

void Camera::UpdateTransformation(float elapsedTime)
{
	IPoint2 mouseChange{ };
	Uint32 button{ SDL_GetRelativeMouseState(&mouseChange.x, &mouseChange.y) };

	FPoint2 mouseChangeTimeDependent{ mouseChange.x * elapsedTime,	mouseChange.y * elapsedTime };

	if (SDL_BUTTON(1) + SDL_BUTTON(3) == button) //left and right mouse
	{
		ChangePosition(FPoint3{ mouseChangeTimeDependent.x, -mouseChangeTimeDependent.y ,0.f });
	}
	else if (SDL_BUTTON(1) == button) //left mouse button
	{
		ChangePosition(FPoint3{ 0.f,0.f, mouseChangeTimeDependent.y });
		ChangeRotation(-mouseChangeTimeDependent.x, false);
	}
	else if (SDL_BUTTON(3) == button) //right mouse button
	{
		ChangeRotation(-mouseChangeTimeDependent.x, false);
		ChangeRotation(mouseChangeTimeDependent.y * m_RotationCorrection, true);
	}
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

void Camera::ChangePosition(const FPoint3& change)
{
	m_CameraChanged = true;

	m_PositionChange.x = change.x;
	m_PositionChange.y = change.y;
	m_PositionChange.z = change.z;
}


const FPoint3& Camera::GetPos() const
{
	return m_Position;
}

float Camera::GetFOV() const
{
	return float(tan((m_Angle / 180.f * M_PI) / 2.f));
}

const FMatrix4& Camera::UpdateAndGetOnb()
{
	if (m_CameraChanged)
	{
		UpdateOnbMatrix();
	}
	return m_Onb;
}

const float Camera::GetNearPlane() const
{
	return m_NearPlane;
}

const float Camera::GetFarPlane() const
{
	return m_FarPlane;
}

void Camera::UpdateOnbMatrix()
{
	m_RotationMatrix3x3 = MakeRotationX(m_RotationChange.x);
	m_RotationMatrix3x3 *= MakeRotationY(m_RotationChange.y);

	m_ForwardV = m_RotationMatrix3x3 * m_ForwardV;
	Normalize(m_ForwardV);

	if (m_ForwardV.z > 0)  m_RotationCorrection = -1;
	else m_RotationCorrection = 1;

	FVector3 rightV{ Cross(m_WordUp,m_ForwardV) };
	Normalize(rightV);
	FVector3 upV{ Cross(m_ForwardV,rightV) };
	Normalize(upV);

	m_Position += m_ForwardV * m_PositionChange.z;
	m_Position += rightV * m_PositionChange.x;
	m_Position += upV * m_PositionChange.y;

	m_Onb = { FVector4{rightV,0.f}, FVector4{upV,0.f}, FVector4{m_ForwardV,0.f}, FVector4{FVector3{m_Position},1.f} };

	m_CameraChanged = false;

	//reset change
	m_RotationChange = { 0.f,0.f };
	m_PositionChange = { 0.f,0.f,0.f };
}

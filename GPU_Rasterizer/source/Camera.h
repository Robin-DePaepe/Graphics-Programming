#pragma once
#include "EMath.h"

using namespace Elite;

class Camera
{
public:
	Camera(const FPoint3& pos,float angle, float aspectRatio);

	//functions
	void ToggleDirectX();

	void Update(float elapsedTime);

	 FMatrix4 GetProjectionMatrixLeftHanded() const;
	 FMatrix4 GetProjectionMatrixRightHanded() const;
	 FMatrix4 GetVieuwMatrix() const;
	 const FPoint3& GetPos() const;

	 void SetGoingForward(bool value);
	 void SetGoingBackward(bool value);
	 void SetGoingUp(bool value);
	 void SetGoingDown(bool value);

private:
	//datamembers
	FMatrix4 m_Onb;
	FMatrix3 m_RotationMatrix3x3;
	FPoint3 m_Position;
	const FVector3 m_WordUp;
	 FVector3 m_ForwardV;
	FPoint3 m_PositionChange;
	FVector2 m_RotationChange;
	int m_RotationCorrection; //when the camera rotates 180 degree the rotation becomes the opposite
	const float m_Fov,m_AspectRatio;
	const float m_RotationSpeed,m_MovementSpeedMouse,m_MovementSpeedKeys;
	const float m_NearPlane,  m_FarPlane;
	bool m_CameraChanged;
	bool m_UsingDirectX;
	bool m_GoingUp, m_GoingDown, m_GoingForward, m_GoingBackwards;
	//functions
	void UpdateOnbMatrix();
	void UpdateTransformation();
	void ChangeRotation(float degrees, bool aroundX);
	void ChangePosition(const FVector3& change);

	void UpdatePositionZ(float elapsedSec, bool up);
	void UpdatePositionX(float elapsedSec, bool forward);
};


#pragma once

using namespace Elite;

class Camera
{
public:
	//rule of 5
	Camera(const FPoint3& pos);
	~Camera() = default;

	Camera(const Camera& other) = delete;
	Camera(Camera&& other) = delete;
	Camera& operator=(const Camera& rhs) = delete;
	Camera& operator=(Camera&& other) = delete;

	//functions
	void UpdateTransformation(float elapsedTime);
	const FPoint3& GetPos() const;
	float GetFOV() const;
	const FMatrix4& UpdateAndGetOnb(float x, float y);
private:
	//datamembers
	FMatrix4 m_Onb;
	FPoint3 m_Position;
	float m_Angle;
	const FVector3 m_WordUp;
	 FVector3 m_ForwardV;
	FPoint3 m_PositionChange;
	FVector2 m_RotationChange;
	int m_RotationCorrection; //when the camera rotates 180 degree the rotation becomes the opposite
	FMatrix3 m_RotationMatrix3x3;
	bool m_CameraChanged;
	const float m_RotationSpeed;
	//functions
	void UpdateOnbMatrix(float x, float y);

	void ChangeRotation(float degrees, bool aroundX);
	void ChangePosition(const FPoint3& change);
};


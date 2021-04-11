#pragma once
#include "Light.h"
#include "EMath.h"

class DirectionalLight : public Light
{
public:
	//rule of 5
	DirectionalLight(float intensity, const Elite::RGBColor& color, const Elite::FVector3 direction);
	~DirectionalLight() = default;

	DirectionalLight(const DirectionalLight& other) = delete;
	DirectionalLight(DirectionalLight&& other) = delete;
	DirectionalLight& operator=(const DirectionalLight& rhs) = delete;
	DirectionalLight& operator=(DirectionalLight&& other) = delete;

	//functions
	Elite::RGBColor GetBiradiance(const Elite::FPoint3& pos) const override;
	const Elite::FVector3 GetDirection(const Elite::FPoint3& point) const override; 
	float GetDistanceToPoint(const Elite::FPoint3& point) const override;
private:
	//datamembers
	const Elite::FVector3 m_Direction;
	const float m_ShadowDistance; //determines how far an object has to be away to block the direct lighting of other objects
};


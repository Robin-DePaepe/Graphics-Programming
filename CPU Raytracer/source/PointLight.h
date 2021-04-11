#pragma once
#include "Light.h"
#include "EMath.h"

class PointLight : public Light
{
public:
	//rule of 5
	PointLight(float intensity, const Elite::RGBColor& color, const Elite::FPoint3& pos);
	~PointLight() = default;

	PointLight(const PointLight& other) = delete;
	PointLight(PointLight&& other) = delete;
	PointLight& operator=(const PointLight& rhs) = delete;
	PointLight& operator=(PointLight&& other) = delete;

	//functions
	Elite::RGBColor GetBiradiance(const Elite::FPoint3& point) const override;
	 const Elite::FVector3 GetDirection(const Elite::FPoint3& point) const override;
	 float GetDistanceToPoint(const Elite::FPoint3& point) const override;

private:
	//datamembers
	const Elite::FPoint3 m_Pos; 
};


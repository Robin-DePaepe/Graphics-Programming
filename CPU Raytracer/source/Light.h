#pragma once
#include "ERGBColor.h"

class Light
{
public:
	//rule of 5
	Light(float intensity, const Elite::RGBColor& color, bool activated = true);
	virtual ~Light() = default;

	Light(const Light& other) = delete;
	Light(Light&& other) = delete;
	Light& operator=(const Light& rhs) = delete;
	Light& operator=(Light&& other) = delete;

	//functions
	virtual	Elite::RGBColor GetBiradiance(const Elite::FPoint3& pos) const = 0;
	virtual const Elite::FVector3 GetDirection(const Elite::FPoint3& point) const = 0;
	virtual float GetDistanceToPoint(const Elite::FPoint3& point) const = 0;
	void ToggleLight();

protected:
	//datamembers
	const float m_LightIntensity;
	const Elite::RGBColor m_Color;
	bool m_Activated;
};


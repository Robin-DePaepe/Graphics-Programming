#include "pch.h"
#include "PointLight.h"

using namespace Elite;
PointLight::PointLight(float intensity, const RGBColor& color, const FPoint3& pos)
	:m_Pos{ pos }
	, Light{ intensity,color }
{
}

RGBColor PointLight::GetBiradiance(const FPoint3& point) const
{
	if (!m_Activated) return RGBColor{};

	return m_Color * (m_LightIntensity / SqrMagnitude(m_Pos - point));
}

const FVector3 PointLight::GetDirection(const FPoint3& point) const
{
	return FVector3{ GetNormalized(m_Pos - point) };
}

float PointLight::GetDistanceToPoint(const FPoint3& point) const
{
	return Distance(m_Pos, point);
}

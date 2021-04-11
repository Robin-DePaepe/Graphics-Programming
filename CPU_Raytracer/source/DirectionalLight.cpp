#include "pch.h"
#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(float intensity, const Elite::RGBColor& color, const Elite::FVector3 direction)
	:Light{intensity,color}
	, m_Direction{Elite::GetNormalized(direction)}
	,m_ShadowDistance{5.f}
{
}

Elite::RGBColor DirectionalLight::GetBiradiance(const Elite::FPoint3& pos) const
{
	if (!m_Activated)
	{
		return Elite::RGBColor{};
	}
	return m_Color * m_LightIntensity;
}

const Elite::FVector3 DirectionalLight::GetDirection(const Elite::FPoint3& point) const
{
	return m_Direction;
}

float DirectionalLight::GetDistanceToPoint(const Elite::FPoint3& point) const
{
	return m_ShadowDistance;
}

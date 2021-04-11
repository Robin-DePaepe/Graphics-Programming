#include "pch.h"
#include "Light.h"

Light::Light(float intensity, const Elite::RGBColor& color, bool activated)
	:m_Color{ color }
	, m_LightIntensity{ intensity }
	, m_Activated{ activated }
{
}

void Light::ToggleLight()
{
	m_Activated = !m_Activated;;
}






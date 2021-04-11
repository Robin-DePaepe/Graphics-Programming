#include "pch.h"
#include "LightManager.h"

std::vector< Light*> LightManager::m_pLights{};

LightManager::~LightManager()
{
	for (size_t i = 0; i < m_pLights.size(); i++)
	{
		delete m_pLights[i];
	}
	m_pLights.clear();
}

const std::vector< Light*>& LightManager::GetLights()
{
	return m_pLights;
}

void LightManager::PushLight(Light* pLight)
{
	m_pLights.push_back(pLight);
}


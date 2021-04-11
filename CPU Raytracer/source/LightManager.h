#pragma once
#include <vector>
#include "Light.h"

class LightManager
{
public:
	//rule of 5
	LightManager() = default;
	~LightManager();

	LightManager(const LightManager& other) = delete;
	LightManager(LightManager&& other) = delete;
	LightManager& operator=(const LightManager& rhs) = delete;
	LightManager& operator=(LightManager&& other) = delete;
	//functions
	static const std::vector< Light*>& GetLights();
	static void PushLight(Light* pLight);

private:
	//datamembers
	static std::vector< Light*> m_pLights;
};


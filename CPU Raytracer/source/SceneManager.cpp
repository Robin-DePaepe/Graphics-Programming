#include "pch.h"
#include "SDL.h"
#include "SceneManager.h"
#include "Scene.h"
#include "LightManager.h"
#include "Camera.h"
#include "Structs.h"
#include "Material.h"

//initializations
std::vector< Scene*> SceneManager::m_pScenes{};
const LightManager* SceneManager::m_pLightManager{ new LightManager{} };
int SceneManager::m_CurrentSceneIndex{ 0 };
bool SceneManager::m_RenderShadows{ true };
SceneManager::RenderState SceneManager::m_RenderState{ SceneManager::RenderState::allActive };

SceneManager::~SceneManager()
{
	for (size_t i = 0; i < m_pScenes.size(); i++)
	{
		delete m_pScenes.at(i);
	}
	m_pScenes.clear();
	delete m_pLightManager;
}

void SceneManager::SwitchToNextScene()
{
	++m_CurrentSceneIndex;

	if (m_CurrentSceneIndex >= m_pScenes.size())
	{
		m_CurrentSceneIndex = 0;
	}
}

void SceneManager::Update(float elapsedTime)
{
	float rotationSpeed{ 0.175f };

	if (m_pScenes.size() > 0)
	{
		m_pScenes[m_CurrentSceneIndex]->UpdateCamera(elapsedTime);
		m_pScenes[m_CurrentSceneIndex]->updateTrianglesRotation(rotationSpeed, elapsedTime);
	}
}

const RGBColor SceneManager::RenderScene(float xSS, float ySS)
{
	Camera* pCamera = GetCamera();
	RGBColor color{ 0.f,0.f,0.f };

	if (pCamera != nullptr && m_pScenes.size() > 0)
	{
		HitRecord hitRecord{};

		//define ray
		FPoint3 sample{ xSS,ySS, 1 };

		MutiplyWithOnb(sample);
		FVector3 dir = sample - pCamera->GetPos();
		Normalize(dir);

		Ray pixelRay{ pCamera->GetPos(), dir };

		//check if the ray hits an object
		if (m_pScenes.at(m_CurrentSceneIndex)->CheckObjectHit(pixelRay, hitRecord))
		{
			//create shadow ray
			HitRecord hitRecordShadow{};
			hitRecordShadow.IsShadowCast = true;

			for (Light* pLight : LightManager::GetLights())
			{
				Ray directLightRay{ hitRecord.intersection, pLight->GetDirection(hitRecord.intersection) };
				directLightRay.tMax = pLight->GetDistanceToPoint(hitRecord.intersection);

				//checks if the light directly hits the intersection
				//order of if statement if importing! checking a simple bool first can save us the calculations of the objecthit function but the last check should always be after the objecthit or it won't work
				if (!m_RenderShadows || m_pScenes.at(m_CurrentSceneIndex)->CheckObjectHit(directLightRay, hitRecordShadow) == false || hitRecordShadow.IsPlane)  //planes don't cast shadows for beauty purpose
				{
					float cosineLaw{ Dot(hitRecord.normal, pLight->GetDirection(hitRecord.intersection)) };

					if (cosineLaw > 0)
					{
						//calculating the color
						switch (m_RenderState)
						{
						case SceneManager::RenderState::allActive:
							color += pLight->GetBiradiance(hitRecord.intersection) * hitRecord.pMat->Shade(hitRecord, directLightRay.direction, pixelRay.direction) * cosineLaw;
							break;
						case SceneManager::RenderState::biradianceOnly:
							color += pLight->GetBiradiance(hitRecord.intersection) * cosineLaw;
							break;
						case SceneManager::RenderState::brdfOnly:
							color += hitRecord.pMat->Shade(hitRecord, directLightRay.direction, pixelRay.direction) * cosineLaw;
							break;
						default:
							break;
						}
					}
				}
			}
			color.MaxToOne();
		}
	}
	return color;
}

void SceneManager::CheckAndHandleInput()
{
	const uint8_t* state{ SDL_GetKeyboardState(NULL) };

	if (state[SDL_SCANCODE_J])
	{
		//Enable/disable directional light
		LightManager::GetLights().at(2)->ToggleLight();
	}
	if (state[SDL_SCANCODE_K])
	{
		//Enable/disable back point light
		LightManager::GetLights().at(0)->ToggleLight();
	}
	if (state[SDL_SCANCODE_L])
	{
		//Enable/disable front point light
		LightManager::GetLights().at(1)->ToggleLight();
	}
	if (state[SDL_SCANCODE_Z])
	{
		//Enable/disable shadows
		SceneManager::ToggleShadows();
	}
	if (state[SDL_SCANCODE_T])
	{
		//toggle render state
		SceneManager::ToggleRenderState();
	}
	if (state[SDL_SCANCODE_S])
	{
		//switch render scene
		SceneManager::SwitchToNextScene();
	}
}

void SceneManager::AddScene(Scene* scene)
{
	m_pScenes.push_back(scene);
}

void SceneManager::ToggleShadows()
{
	m_RenderShadows = !m_RenderShadows;
}

void SceneManager::ToggleRenderState()
{
	switch (m_RenderState)
	{
	case SceneManager::RenderState::allActive:
		m_RenderState = RenderState::biradianceOnly;
		break;

	case SceneManager::RenderState::biradianceOnly:
		m_RenderState = RenderState::brdfOnly;
		break;

	case SceneManager::RenderState::brdfOnly:
		m_RenderState = RenderState::allActive;
		break;
	}
}

float SceneManager::GetCameraFov()
{
	Camera* pCam{ GetCamera() };

	if (pCam != nullptr)
	{
		return GetCamera()->GetFOV();
	}
	return 0.f;
}

Camera* SceneManager::GetCamera()
{
	if (m_pScenes.size() > 0)
	{
		return m_pScenes[m_CurrentSceneIndex]->GetCamera();
	}
	return nullptr;
}

void SceneManager::MutiplyWithOnb(FPoint3& point)
{
	FPoint4 temp{ point.x,point.y,point.z,1 };
	Camera* pCamera = GetCamera();

	if (pCamera != nullptr)
	{
		temp = pCamera->UpdateAndGetOnb(point.x, point.y) * temp;
	}

	point.x = temp.x;
	point.y = temp.y;
	point.z = temp.z;
}



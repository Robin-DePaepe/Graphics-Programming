#pragma once
#include <vector>

class LightManager;
class Scene;
class Camera;

using namespace Elite;

class SceneManager
{
public:
	//enum
	enum class RenderState
	{
		allActive, biradianceOnly, brdfOnly
	};
	//rule of 5
	SceneManager() = default;
	~SceneManager();

	SceneManager(const SceneManager& other) = delete;
	SceneManager(SceneManager&& other) = delete;
	SceneManager& operator=(const SceneManager& rhs) = delete;
	SceneManager& operator=(SceneManager&& other) = delete;

	//functions
	static void Update(float elapsedTime);
	static const RGBColor RenderScene(float xSS, float ySS);
	static void CheckAndHandleInput();

	static void AddScene(Scene* pScene);
	static void SwitchToNextScene();

	static void ToggleShadows();
	static void ToggleRenderState();
	static float GetCameraFov();
private:
	//datamembers
	static std::vector<Scene*> m_pScenes;
	static const LightManager* m_pLightManager;
	static int m_CurrentSceneIndex;
	static RenderState m_RenderState;
	static bool m_RenderShadows;
	//functions
	static void MutiplyWithOnb(FPoint3& point);
	static Camera* GetCamera();
};


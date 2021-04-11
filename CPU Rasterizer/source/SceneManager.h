#pragma once
#include "EMath.h"
#include "ERGBColor.h"
#include <vector>
#include "Scene.h"
#include "Camera.h"

class Scene;
class Camera;

using namespace Elite;

class SceneManager
{
public:
	//rule of 5
	SceneManager() = default;
	~SceneManager();

	SceneManager(const SceneManager& other) = delete;
	SceneManager(SceneManager&& other) = delete;
	SceneManager& operator=(const SceneManager& rhs) = delete;
	SceneManager& operator=(SceneManager&& other) = delete;

	//functions
	static void Update(float elapsedTime);
	static const void RenderScene();

	static void AddScene(Scene* pScene);
	static void SwitchToNextScene();
private:
	//datamembers
	static std::vector<Scene*> m_pScenes;
	static int m_CurrentSceneIndex;
};


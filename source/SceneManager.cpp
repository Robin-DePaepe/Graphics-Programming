#include "SDL.h"
#include "SceneManager.h"
#include "Structs.h"
#include "ERGBColor.h"

//initializations
std::vector< Scene*> SceneManager::m_pScenes{};
int SceneManager::m_CurrentSceneIndex{ 0 };

void SceneManager::SwitchToNextScene()
{
	++m_CurrentSceneIndex;

	if (m_CurrentSceneIndex >= m_pScenes.size())
	{
		m_CurrentSceneIndex = 0;
	}
}

SceneManager::~SceneManager()
{
	for (size_t i = 0; i < m_pScenes.size(); i++)
	{
		delete m_pScenes.at(i);
	}
}

void SceneManager::Update(float elapsedTime)
{
	if (m_pScenes.size() > 0)
	{
		m_pScenes[m_CurrentSceneIndex]->Update(elapsedTime);
	}
}

const void SceneManager::RenderScene()
{
	if (m_pScenes.size() > 0)
	{
		m_pScenes[m_CurrentSceneIndex]->RenderScene();
	}
}
void SceneManager::AddScene( Scene* pScene)
{
	m_pScenes.push_back(pScene);
}






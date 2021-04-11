#include "pch.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "Camera.h"


Scene::Scene(const FPoint3& cameraPos)
	:m_pCamera{ new Camera{cameraPos} }
	, m_pObjectManager{ new ObjectManager{} }
{
}

Scene::~Scene()
{
	delete m_pCamera;
	delete m_pObjectManager;
}

void Scene::AddObject(Object* pObject)
{
	m_pObjectManager->PushObject(pObject);
}

void Scene::UpdateCamera(float elapsedTime)
{
	m_pCamera->UpdateTransformation( elapsedTime);
}

void Scene::updateTrianglesRotation(float speed, float elapsedTime)
{
	m_pObjectManager->RotateTriangles(speed, elapsedTime);
}

Camera* Scene::GetCamera() const
{
	return m_pCamera;
}

bool Scene::CheckObjectHit(const Ray& ray, HitRecord& hitRecord)
{
	return m_pObjectManager->CheckObjectHit(ray, hitRecord);
}

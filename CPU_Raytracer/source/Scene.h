#pragma once
#include "EMath.h"
#include "Structs.h"

class Camera;
class ObjectManager;
class Object;

using namespace Elite;

class Scene
{
public:
	//rule of 5
	Scene(const FPoint3& cameraPos);
	~Scene();

	Scene(const Scene& other) = delete;
	Scene(Scene&& other) = delete;
	Scene& operator=(const Scene& rhs) = delete;
	Scene& operator=(Scene&& other) = delete;
	//functions
	void UpdateCamera(float elapsedTime);
	void updateTrianglesRotation(float speed, float elapsedTime);

	Camera* GetCamera() const;
	void AddObject(Object* pObject);
	bool CheckObjectHit(const Ray& ray, HitRecord& hitRecord);

private:
	//datamembers
	Camera* m_pCamera;
	ObjectManager* m_pObjectManager;
};


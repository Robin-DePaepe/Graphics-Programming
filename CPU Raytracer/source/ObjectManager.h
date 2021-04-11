#pragma once
#include <vector>
#include "Object.h"

class ObjectManager
{
public:
	//rule of 5
	ObjectManager();
	~ObjectManager();

	ObjectManager(const ObjectManager& other) = delete;
	ObjectManager(ObjectManager&& other) = delete;
	ObjectManager& operator=(const ObjectManager& rhs) = delete;
	ObjectManager& operator=(ObjectManager&& other) = delete;

	//functions
	bool CheckObjectHit(const Ray& ray, HitRecord& hitRecord);

	void RotateTriangles(float speed, float elapsedTime);
	void PushObject(Object* pObject);

private:
	//datamembers
	std::vector< Object*>* m_pObjects;
};




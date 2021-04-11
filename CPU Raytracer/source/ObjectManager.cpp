#include "pch.h"
#include "ObjectManager.h"
#include "Structs.h"
#include <typeinfo>


ObjectManager::ObjectManager()
	:m_pObjects{ new std::vector< Object*>{} }
{
}

ObjectManager::~ObjectManager()
{
	for (size_t i = 0; i < m_pObjects->size(); i++)
	{
		delete m_pObjects->at(i);
	}
	m_pObjects->clear();
	delete m_pObjects;
}

bool ObjectManager::CheckObjectHit(const Ray& ray, HitRecord& hitRecord)
{
	bool objecthit{ false };
	HitRecord temp{};
	temp.t = ray.tMax;

	for (size_t i = 0; i < m_pObjects->size(); i++)
	{
		if (m_pObjects->at(i)->Hit(ray, hitRecord))
		{
			if (hitRecord.t < temp.t)
			{
				temp = hitRecord;
			}
			objecthit = true;
		}
	}
	hitRecord = temp;
	return objecthit;
}

void ObjectManager::RotateTriangles(float speed, float elapsedTime)
{
	for (size_t i = 0; i < m_pObjects->size(); i++)
	{
		if (typeid(*m_pObjects->at(i)) == typeid(Triangle))
		{
			Triangle* pTiangle{ static_cast<Triangle*>(m_pObjects->at(i)) };
			pTiangle->RotateAroundY(speed, elapsedTime);
		}
	}
}

void ObjectManager::PushObject(Object* pObject)
{
	m_pObjects->push_back(pObject);
}


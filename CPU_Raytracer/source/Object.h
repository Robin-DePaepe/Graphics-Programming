#pragma once
#include "EMath.h"

struct  Ray;
struct HitRecord;
class Material;

class Object
{
public:
	//rule of 5
	Object(Material* pMat, const Elite::FPoint3& pos, bool partOfMesh);
	virtual ~Object();

	Object(const Object& other) = delete;
	Object(Object&& other) = delete;
	Object& operator=(const Object& rhs) = delete;
	Object& operator=(Object&& other) = delete;
	//functions
	virtual bool Hit(const Ray& ray, HitRecord& hitRecord) const = 0;

protected:
	//datamembers
	 Material* m_pMaterial;
	const Elite::FPoint3 m_Position;
	const bool m_PartOfMesh;
};


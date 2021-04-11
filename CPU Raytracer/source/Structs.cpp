#include "pch.h"
#include "Structs.h"
#include "Material.h"

using namespace Elite;

Ray::Ray(const FPoint3& origin, const FVector3 direction)
	:origin{ origin }
	, direction{ direction }
{}

Sphere::Sphere(const FPoint3& pos, float rad, Material* pMat, bool partOfMesh)
	:Object{pMat,pos,partOfMesh}
	, radius{ rad }
{}

bool Sphere::Hit(const Ray& ray, HitRecord& hitRecord) const
{
	const float a{ Dot(ray.direction, ray.direction) };
	const float b{ 2.f * Dot(ray.direction,(ray.origin - m_Position)) };
	const float c{ Dot((ray.origin - m_Position),(ray.origin - m_Position)) - radius * radius };

	const float discriminant{ b * b - 4 * a * c };

	if (discriminant > 0)
	{
		if (discriminant > ray.tMax)
		{
			return false;
		}
		const float sqrtDiscriminant{ float(sqrt(discriminant)) };
		const float t{ (-b - sqrtDiscriminant) / 2.f * a };

		if (t < ray.tMin || t > ray.tMax)
		{
			return false;
		}

		hitRecord.intersection = ray.origin + t * ray.direction;
		hitRecord.t = t;
		hitRecord.normal = hitRecord.intersection - m_Position;
		hitRecord.pMat = m_pMaterial;
		Normalize(hitRecord.normal);

		return true;
	}
	return false;
}


Plane::Plane(const FPoint3& pos, const FVector3& norm, Material* pMat, bool partOfMesh)
	:Object{pMat,pos,partOfMesh}
	, normal{ norm }
{}


bool Plane::Hit(const Ray& ray, HitRecord& hitRecord) const
{
	const float denominator{ Dot(ray.direction,normal) };

	if (std::abs(denominator) <= 0.0001f)
	{
		return false;
	}

	const float nominator{ Dot(FVector3{m_Position - ray.origin}, normal) };
	const float t = nominator / denominator;

	if (t > ray.tMin && t < ray.tMax)
	{
		hitRecord.intersection = { ray.origin + t * ray.direction };
		hitRecord.t = t;
		hitRecord.normal = normal;
		hitRecord.pMat = m_pMaterial;
		hitRecord.IsPlane = true;

		return true;
	}
	return false;
}

HitRecord::HitRecord()
	: intersection{}
	, t{}
	, normal{}
	, pMat{ nullptr }
	, IsPlane{ false }
	, IsShadowCast{ false }
{
}

Triangle::Triangle(const FPoint3& v0, const FPoint3& v1, const FPoint3& v2, const FPoint3& location,
	Material* pMat, bool partOfMesh, CullModes cullingMode)
	:Object{pMat,location,partOfMesh}
	,v0{ v0 }
	, v1{ v1 }
	, v2{ v2 }
	, cullMode{ cullingMode }
	, time{ 0.f }
{
}

Triangle::Triangle(Triangle&& other)
	:Object{other.m_pMaterial,other.m_Position,other.m_PartOfMesh}
	, time{ 0.f }
	, cullMode{ other.cullMode }
	, v0{ other.v0 }
	, v1{ other.v1 }
	, v2{ other.v2 }
{
	other.m_pMaterial = nullptr;
}


void Triangle::RotateAroundY(float speed, float elapsedTime)
{
	time += elapsedTime;

	if (time >= 1.f)
	{
		FMatrix3 rotation{ MakeRotationY(speed) };

		FPoint3 center{ (v0.x + v1.x + v2.x) / 3.f,0.f ,(v0.z + v1.z + v2.z) / 3.f };//x and y caus y won't change anyway

		v0 = FPoint3(rotation * (v0 - center));
		v1 = FPoint3(rotation * (v1 - center));
		v2 = FPoint3(rotation * (v2 - center));

		v0.x += center.x;
		v1.x += center.x;
		v2.x += center.x;

		v0.z += center.z;
		v1.z += center.z;
		v2.z += center.z;

		time -= 1.f;
	}
}

bool Triangle::Hit(const Ray& ray, HitRecord& hitRecord) const
{
	const FPoint3 point1{ m_Position.x + v0.x , m_Position.y + v0.y , m_Position.z + v0.z };
	const FPoint3 point2{ m_Position.x + v1.x , m_Position.y + v1.y , m_Position.z + v1.z };
	const FPoint3 point3{ m_Position.x + v2.x , m_Position.y + v2.y , m_Position.z + v2.z };

	const FVector3 EdgeA{ point2 - point1 }, EdgeB{ point3 - point1 };

	hitRecord.normal = GetNormalized(Cross(EdgeA, EdgeB));

	const float dotDirectionAndNormal{ Dot(ray.direction, hitRecord.normal) };

	if (!dotDirectionAndNormal)
	{
		return false;
	}

	switch (cullMode)
	{
	case Triangle::frontFaceCulling:

		if (hitRecord.IsShadowCast) //opposiste culling for shadows
		{
			if (Dot(hitRecord.normal, ray.direction) > 0)
			{
				return false;
			}
		}
		else
		{
			if (Dot(hitRecord.normal, ray.direction) < 0)
			{
				return false;
			}
		}
		break;
	case Triangle::backFaceCulling:

		if (hitRecord.IsShadowCast)
		{
			if (Dot(hitRecord.normal, ray.direction) < 0) //opposiste culling for shadows
			{
				return false;
			}
		}
		else
		{
			if (Dot(hitRecord.normal, ray.direction) > 0)
			{
				return false;
			}
		}
		break;
	}

	const FPoint3 center{ (point1.x + point2.x + point3.x) / 3.f ,(point1.y + point2.y + point3.y) / 3.f,(point1.z + point2.z + point3.z) / 3.f };
	const FVector3 l{ center - ray.origin };

	hitRecord.t = { Dot(l,hitRecord.normal) / dotDirectionAndNormal };

	if (hitRecord.t < ray.tMin || hitRecord.t > ray.tMax)
	{
		return false;
	}

	hitRecord.intersection = { ray.origin + hitRecord.t * ray.direction };
	const FVector3 sideToPoint{ hitRecord.intersection - point1 };

	if (Dot(hitRecord.normal, Cross(EdgeA, sideToPoint)) < 0)
	{
		return false;
	}

	//Barycentric Technique

	const float dotEdgeA{ Dot(EdgeA,EdgeA) };
	const float dotEdgeAB{ Dot(EdgeA,EdgeB) };
	const float dotEdgeASideToPoint{ Dot(EdgeA,sideToPoint) };
	const float dotEdgeB{ Dot(EdgeB,EdgeB) };
	const float dotEdgeBSideToPoint{ Dot(EdgeB,sideToPoint) };

	const float invDenom = 1.f / (dotEdgeA * dotEdgeB - dotEdgeAB * dotEdgeAB);
	const float u = (dotEdgeB * dotEdgeASideToPoint - dotEdgeAB * dotEdgeBSideToPoint) * invDenom;
	const float v = (dotEdgeA * dotEdgeBSideToPoint - dotEdgeAB * dotEdgeASideToPoint) * invDenom;

	hitRecord.pMat = m_pMaterial;

	return (u >= 0) && (v >= 0) && (u + v < 1);
}

TriangleMesh::TriangleMesh(std::vector<FPoint3>* pVertexBuffer, std::vector<IPoint3>* pIndexBuffer, Material* pMat, const FPoint3& pos, bool partOfMesh)
	:Object{pMat,pos,partOfMesh}
	, m_pIndexBuffer{ pIndexBuffer }
	, m_pTriangles{ new std::vector<Triangle> }
	, m_pVertexBuffer{ pVertexBuffer }
{
	CreateTriangles();
}

TriangleMesh::~TriangleMesh()
{
	delete m_pTriangles;
	delete m_pIndexBuffer;
	delete m_pVertexBuffer;
}

bool TriangleMesh::Hit(const Ray& ray, HitRecord& hitRecord) const
{
	bool objecthit{ false };
	HitRecord temp{};

	temp.t = ray.tMax;

	for (size_t i = 0; i < m_pTriangles->size(); i++)
	{
		if (m_pTriangles->at(i).Hit(ray, hitRecord))
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


void TriangleMesh::CreateTriangles()
{
	for (size_t i = 0; i < m_pIndexBuffer->size(); i++)
	{
		FPoint3 v0{ m_pVertexBuffer->at(m_pIndexBuffer->at(i).x - 1) };//index buffer starts at 1 not 0
		FPoint3 v1{ m_pVertexBuffer->at(m_pIndexBuffer->at(i).y - 1) };
		FPoint3 v2{ m_pVertexBuffer->at(m_pIndexBuffer->at(i).z - 1) };

		m_pTriangles->push_back({ AddPoints(v0, m_Position), AddPoints(v1, m_Position), AddPoints(v2, m_Position), FPoint3{}, m_pMaterial, true });
	}
}

FPoint3 TriangleMesh::AddPoints(const FPoint3& a, const FPoint3& b) const
{
	return FPoint3(a.x + b.x, a.y + b.y, a.z + b.z);
}


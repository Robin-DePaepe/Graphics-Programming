#pragma once
#include "Object.h"
#include <unordered_map>
#include "EMath.h"

class Material;
using namespace Elite;

struct Ray
{
	//constructor
	Ray(const FPoint3& origin, const FVector3 direction);

	//datamembers
	float tMin = { 0.0001f };
	float tMax = FLT_MAX;
	FPoint3	 origin{};
	FVector3 direction{};
};

struct Sphere : Object
{
	//rule of 5
	Sphere(const FPoint3& pos, float rad, Material* pMat, bool partOfMesh = false);
	~Sphere() = default;

	Sphere(const Sphere& other) = delete;
	Sphere(Sphere&& other) = delete;
	Sphere& operator=(const Sphere& rhs) = delete;
	Sphere& operator=(Sphere&& other) = delete;

	//functions
	bool Hit(const Ray& ray, HitRecord& hitRecord) const override;

	//datamembers
	const float radius;
};

struct Plane : Object
{
	//rule of 5
	Plane(const FPoint3& pos, const FVector3& norm, Material* pMat, bool partOfMesh = false);
	~Plane() = default;

	Plane(const Plane& other) = delete;
	Plane(Plane&& other) = delete;
	Plane& operator=(const Plane& rhs) = delete;
	Plane& operator=(Plane&& other) = delete;

	//functions
	bool Hit(const Ray& ray, HitRecord& hitRecord) const override;

	//datamembers
	const FVector3 normal;
};

struct Triangle : Object
{
	//enum
	enum CullModes
	{
		noCulling, frontFaceCulling, backFaceCulling
	};
	//rule of 5
	Triangle(const FPoint3& v0, const FPoint3& v1, const FPoint3& v2, const FPoint3& location, Material* pMat, bool partOfMesh,
		CullModes cullingMode = CullModes::noCulling);
	~Triangle() = default;

	Triangle(const Triangle& other) = delete;
	Triangle(Triangle&& other);
	Triangle& operator=(const Triangle& rhs) = delete;
	Triangle& operator=(Triangle&& other) = delete;

	//functions
	void RotateAroundY(float speed, float elapsedTime);
	bool Hit(const Ray& ray, HitRecord& hitRecord) const override;

	//datamembers
	FPoint3 v0, v1, v2;
	const CullModes cullMode;
	float time;
};

struct HitRecord
{
	HitRecord();

	float t;
	FPoint3 intersection;
	FVector3 normal;
	Material* pMat;
	bool IsPlane;
	bool IsShadowCast;
};

struct ObjData
{
	ObjData() = default;

	std::vector<FPoint3>m_Vertexes;
	std::vector<IPoint3>m_Faces;
};

struct TriangleMesh : Object
{
public:
	//rule of 5
	TriangleMesh(std::vector<FPoint3>* pVertexBuffer, std::vector<IPoint3>* pIndexBuffer, Material* pMat, const FPoint3& pos, bool partOfMesh = false);
	~TriangleMesh();

	TriangleMesh(const TriangleMesh& other) = delete;
	TriangleMesh(TriangleMesh&& other) = delete;
	TriangleMesh& operator=(const TriangleMesh& rhs) = delete;
	TriangleMesh& operator=(TriangleMesh&& other) = delete;

	//functions
	bool Hit(const Ray& ray, HitRecord& hitRecord) const override;

private:
	//datamembers
	std::vector<FPoint3>* m_pVertexBuffer;
	std::vector<IPoint3>* m_pIndexBuffer;
	std::vector<Triangle>* m_pTriangles;
	//functions
	void CreateTriangles();
	FPoint3 AddPoints(const FPoint3& a, const FPoint3& b) const; 
};
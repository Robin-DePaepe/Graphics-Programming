#pragma once
#include <vector>
#include "Structs.h"
#include "EMath.h"

using namespace Elite;
using namespace std;

class Mesh
{
public:
	//enum
	enum class PrimitiveTopology
	{
		triangleList, triangleStrip
	};
	//rule of 5
	Mesh(const string& diffusePath, const string& normalMapPath, const string& specularMapPath, const string& glossyMapPath, const PrimitiveTopology& topology, bool usePhong, float shininess = 0.f);
	~Mesh();

	Mesh(const Mesh& other) = delete;
	Mesh(Mesh&& other) = delete;
	Mesh& operator=(const Mesh& rhs) = delete;
	Mesh& operator=(Mesh&& other) = delete;

	//functions
	std::vector<TriangleInput> GetTriangles() const;
	const TextureManager* GetTextures() const;

	float GetRotationY()const;

	size_t AddPoint(const InputVertex point);
	void AddTriangeIndices(const IPoint3 indices);
	void AddTriangeIndices(const size_t indices);

	std::pair<bool, int> CheckForDublicate(const InputVertex& vertex);
	void Update(float elapsedTime);
	void CalculateAndInitializeVertexTangent();

private:
	//datamembers
	std::vector<InputVertex>  m_Verteces;
	std::vector<IPoint3>  m_TriangleIndices;
	std::vector<size_t>  m_TriangleIndicesStrip;
	const PrimitiveTopology m_Topology;
	const TextureManager* m_pTextures;
	const float m_RotationSpeed;
	float m_RotationY;
	const bool m_UsePhong;
};
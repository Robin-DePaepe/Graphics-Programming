#include "Mesh.h"

Mesh::Mesh(const string& diffusePath, const string& normalMapPath, const string& specularMapPath, const string& glossyMapPath, const PrimitiveTopology& topology, bool usePhong, float shininess)
	:m_TriangleIndices{ }
	, m_TriangleIndicesStrip{}
	, m_Verteces{ }
	, m_Topology{ topology }
	, m_pTextures{ new TextureManager{ diffusePath,normalMapPath,specularMapPath,glossyMapPath ,usePhong,shininess} }
	, m_RotationSpeed{ 15.f }
	, m_RotationY{}
	, m_UsePhong{ usePhong }
{
}

Mesh::~Mesh()
{
	delete m_pTextures;
}

vector<TriangleInput> Mesh::GetTriangles() const
{
	vector<TriangleInput> triangles;

	switch (m_Topology)
	{
	case Mesh::PrimitiveTopology::triangleList:
		for (size_t i = 0; i < m_TriangleIndices.size(); i++)
		{
			const IPoint3 indices{ m_TriangleIndices.at(i) };

			triangles.push_back(TriangleInput{ m_Verteces.at(indices.x),  m_Verteces.at(indices.y), m_Verteces.at(indices.z) });
		}
		break;
	case Mesh::PrimitiveTopology::triangleStrip:

		bool even = true; //use of boolean for performance and simplicity

		for (size_t i = 0; i < m_TriangleIndicesStrip.size() - 2; i++)
		{
			//check for degenerate triangle
			if (m_TriangleIndicesStrip.at(i + 1) != m_TriangleIndicesStrip.at(i + 2))
			{
				if (even)
				{
					triangles.push_back(TriangleInput{ m_Verteces.at(m_TriangleIndicesStrip.at(i)),  m_Verteces.at(m_TriangleIndicesStrip.at(i + 1)),	m_Verteces.at(m_TriangleIndicesStrip.at(i + 2)) });
				}
				else
				{
					triangles.push_back(TriangleInput{ m_Verteces.at(m_TriangleIndicesStrip.at(i)),  m_Verteces.at(m_TriangleIndicesStrip.at(i + 2)),	m_Verteces.at(m_TriangleIndicesStrip.at(i + 1)) });
				}
				even = !even;
			}
			else
			{
				i += 3;
			}
		}
		break;
	}
	return triangles;
}

const TextureManager* Mesh::GetTextures() const
{
	return m_pTextures;
}

float Mesh::GetRotationY() const
{
	return m_RotationY;
}

size_t Mesh::AddPoint(const InputVertex point)
{
	m_Verteces.push_back(point);
	return m_Verteces.size() - 1;  //return the index of the point
}

void Mesh::AddTriangeIndices(const IPoint3 indices)
{
	m_TriangleIndices.push_back(indices);
}

void Mesh::AddTriangeIndices(const size_t indices)
{
	m_TriangleIndicesStrip.push_back(indices);
}

pair<bool, int> Mesh::CheckForDublicate(const InputVertex& vertex)
{
	for (size_t i = 0; i < m_Verteces.size(); i++)
	{
		if (m_Verteces.at(i) == vertex)
		{
			return { true,int(i) };
		}
	}
	return { false,0 };
}

void Mesh::Update(float elapsedTime)
{
	m_RotationY += elapsedTime * m_RotationSpeed;

	if (m_RotationY > 360.f) m_RotationY -= 360.f; //prevents the float from reaching its limit
}

void Mesh::CalculateAndInitializeVertexTangent()
{
	if (!m_pTextures->normalMap.IsValid()) return; //tangens won't be used if the normalmap is invalid

	switch (m_Topology)
	{
	case Mesh::PrimitiveTopology::triangleList:
		for (size_t i = 0; i < m_TriangleIndices.size(); i++)
		{
			const IPoint3 indices{ m_TriangleIndices.at(i) };

			const FPoint3& p0 = m_Verteces.at(indices.x).point;
			const FPoint3& p1 = m_Verteces.at(indices.y).point;
			const FPoint3& p2 = m_Verteces.at(indices.z).point;
			const FVector2& uv0 = m_Verteces.at(indices.x).uv;
			const FVector2& uv1 = m_Verteces.at(indices.y).uv;
			const FVector2& uv2 = m_Verteces.at(indices.z).uv;

			const FVector3 edge0 = p1 - p0;
			const FVector3 edge1 = p2 - p0;
			const FVector2 diffX = FVector2(uv1.x - uv0.x, uv2.x - uv0.x);
			const FVector2 diffY = FVector2(uv1.y - uv0.y, uv2.y - uv0.y);
			const float r = 1.f / Cross(diffX, diffY);

			FVector3 vertexTangent{ (edge0 * diffY.y - edge1 * diffY.x) * r };
			m_Verteces.at(indices.x).tangent += vertexTangent;
			m_Verteces.at(indices.y).tangent += vertexTangent;
			m_Verteces.at(indices.z).tangent += vertexTangent;
		}
		for (size_t i = 0; i < m_Verteces.size(); i++)
		{
			m_Verteces.at(i).tangent = GetNormalized(Reject(m_Verteces.at(i).tangent, m_Verteces.at(i).normal));
		}
		break;
	}
	//open for more topology implementations
}

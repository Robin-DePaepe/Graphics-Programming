#include "Scene.h"
#include "Camera.h"
#include "Mesh.h"

Scene::Scene(const FPoint3& cameraPos, float fovAngle, std::shared_ptr<Renderer> spRenderer)
	:m_pCamera{ new Camera{cameraPos,fovAngle} }
	, m_pMeshes{ }
	, m_spRenderer{ spRenderer }
	, m_ProjectionMatrix{}
	, m_WorldVieuwProjectionMatrix{}
	, m_WorldMatrix{  }
{
	m_ProjectionMatrix.data[2][3] = -1.f;// copy z value into w, - because right handed system
}

Scene::~Scene()
{
	delete m_pCamera;

	for (size_t i = 0; i < m_pMeshes.size(); i++)
	{
		delete m_pMeshes[i];
	}
}

void Scene::AddMesh(Mesh* pMesh)
{
	m_pMeshes.push_back(pMesh);
}

void Scene::Update(float elapsedTime)
{
	for (size_t i = 0; i < m_pMeshes.size(); i++)
	{
		m_pMeshes.at(i)->Update(elapsedTime);
	}
	m_pCamera->UpdateTransformation(elapsedTime);
}

void Scene::RenderScene()
{
	m_spRenderer->StartRender();

	const FMatrix4 inverseOnb{ Inverse(m_pCamera->UpdateAndGetOnb()) };

	//render meshes
	for (size_t i = 0; i < m_pMeshes.size(); i++)
	{
		std::vector<TriangleInput> meshTriangles = m_pMeshes.at(i)->GetTriangles();

		FMatrix4 rotationMatrix{ Elite::MakeRotationY(Elite::ToRadians(m_pMeshes.at(i)->GetRotationY())),FVector3{} };
		m_WorldMatrix = FMatrix4::Identity() * rotationMatrix;

		RenderTriangles(meshTriangles, inverseOnb, m_pMeshes.at(i)->GetTextures());
	}
	m_spRenderer->EndRender();
}

void Scene::PerspectiveDivide(FPoint4& vieuwPoint) const
{
	vieuwPoint.xyz /= vieuwPoint.w;
}


void Scene::UpdateProjectionMatrix()
{
	m_ProjectionMatrix.data[0][0] = 1.f / (m_spRenderer->GetAspectRatio() * m_pCamera->GetFOV());
	m_ProjectionMatrix.data[1][1] = 1.f / m_pCamera->GetFOV();
	m_ProjectionMatrix.data[2][2] = m_pCamera->GetFarPlane() / (m_pCamera->GetNearPlane() - m_pCamera->GetFarPlane());
	m_ProjectionMatrix.data[3][2] = (m_pCamera->GetFarPlane() * m_pCamera->GetNearPlane()) / (m_pCamera->GetNearPlane() - m_pCamera->GetFarPlane());
}

void Scene::UpdateWorldVieuwProjectionMatrix(const FMatrix4& inverseOnb)
{
	UpdateProjectionMatrix();
	m_WorldVieuwProjectionMatrix = m_ProjectionMatrix * inverseOnb * m_WorldMatrix;
}

void Scene::RenderTriangles(const std::vector<TriangleInput>& triangles, const FMatrix4& inverseOnb, const TextureManager* pTextures)
{
	UpdateWorldVieuwProjectionMatrix(inverseOnb); //happens for each object

	for (size_t i = 0; i < triangles.size(); i++)
	{
		TriangleInput currentTriangle{ triangles.at(i) };

		TriangleOutput ndcTriangle{ TransformToOutput(currentTriangle.v0,pTextures->usePhong),TransformToOutput(currentTriangle.v1,pTextures->usePhong),TransformToOutput(currentTriangle.v2,pTextures->usePhong) };

		PerspectiveDivide(ndcTriangle.v0.point);
		PerspectiveDivide(ndcTriangle.v1.point);
		PerspectiveDivide(ndcTriangle.v2.point);

		//check culling
		if (IsInRange(ndcTriangle.v0.point.x, -1.f, 1.f) && IsInRange(ndcTriangle.v1.point.x, -1.f, 1.f) && IsInRange(ndcTriangle.v2.point.x, -1.f, 1.f) && //check x culling
			IsInRange(ndcTriangle.v0.point.y, -1.f, 1.f) && IsInRange(ndcTriangle.v1.point.y, -1.f, 1.f) && IsInRange(ndcTriangle.v2.point.y, -1.f, 1.f))		//check y culling
		{
			m_spRenderer->RenderTriangle(ndcTriangle, pTextures);
		}
	}
}

bool Scene::IsInRange(float value, float min, float max) const
{
	return (value > min && value < max);
}

OutputVertex Scene::TransformToOutput(InputVertex vertex, bool calculateVieuwDir)
{
	FVector3 vieuwDir{};

	if (calculateVieuwDir)
	{
		FPoint3 posInWorld{ (m_WorldMatrix * FPoint4{ vertex.point}).xyz };
		vieuwDir = GetNormalized(FVector3{ posInWorld - m_pCamera->GetPos() });
	}
	//apply the woldvieuwprojection matrix to the points and the world matrix to the normals
	return	OutputVertex{ m_WorldVieuwProjectionMatrix * FPoint4{ vertex.point } , vertex.uv,ApplyWorldToVector3(vertex.normal),ApplyWorldToVector3(vertex.tangent), vieuwDir,vertex.color };
}

FVector3 Scene::ApplyWorldToVector3(const FVector3& vector)
{
	FVector4 vector4D{ vector,1.f };

	vector4D = m_WorldMatrix * vector4D;

	return  vector4D.xyz;
}



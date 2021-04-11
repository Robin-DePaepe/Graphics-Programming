#pragma once
#include "EMath.h"
#include "Structs.h"
#include <vector>
#include "ERenderer.h"
#include <memory>
#include "Camera.h"
#include "mesh.h"

class Camera;
class Mesh;

using namespace Elite;

class Scene
{
public:
	//rule of 5
	Scene(const FPoint3& cameraPos, float fovAngle, std::shared_ptr<Renderer> spRenderer);
	~Scene();

	Scene(const Scene& other) = delete;
	Scene(Scene&& other) = delete;
	Scene& operator=(const Scene& rhs) = delete;
	Scene& operator=(Scene&& other) = delete;

	//functions
	void AddMesh( Mesh* pMesh);
	void Update(float elapsedTime);
	void RenderScene();
private:
	//datamembers
	Camera * m_pCamera;
	std::vector<Mesh*> m_pMeshes;
	std::shared_ptr<Renderer>  m_spRenderer;
	FMatrix4 m_ProjectionMatrix,m_WorldMatrix, m_WorldVieuwProjectionMatrix;
	//functions
	 void PerspectiveDivide( FPoint4& vieuwPoint) const;

	void UpdateProjectionMatrix();
	void UpdateWorldVieuwProjectionMatrix(const FMatrix4& inverseOnb);
	void RenderTriangles(const std::vector<TriangleInput>& triangles, const FMatrix4& inverseOnb,  const TextureManager* pTextures);
	bool IsInRange(float value, float min, float max) const;
	OutputVertex TransformToOutput(InputVertex vertex, bool calculateVieuwDir);
	FVector3 ApplyWorldToVector3(const FVector3& vector);
};


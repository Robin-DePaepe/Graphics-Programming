#pragma once
#include <vector>
#include "structs.h"
#include "Texture.h"

class Effect;
struct  ID3D11Device;
struct  ID3D11DeviceContext;
struct ID3D11InputLayout;
struct ID3D11Buffer;
struct ID3DX11EffectTechnique;

class Camera;
using namespace std;

	//enum
	enum TextureFiltering
	{
	point,	linear, anisotropic, end
	};
	void operator++(TextureFiltering& filter);
class Mesh
{
public:

	//rule of 5
	Mesh(ID3D11Device* pDeviceconst ,	const char* imagePath, const char* normalPath, const char* specularPath, const char* glossinessPath,  Effect* pEffect,bool hasTransparency, float shininess = 0.f);
	~Mesh();

	//functions
	void RenderInDirectX(ID3D11DeviceContext* pDeviceContext, const FMatrix4& vieuwMatrix);
	void Update(float elapsedTime);
	void SwitchTextureFilter();

	pair<bool, int> CheckForDublicate(const inputVertex& vertex);
	void CreateBuffersForGpu();

	void CalculateAndInitializeVertexTangent();
	void InvertZComponentsVertices();

	size_t AddVertex(const inputVertex& vertex);
	void AddIndices(const IPoint3& indices);

	 std::vector<TriangleOutput> GetTriangles(const Camera* pCamera) ;
	const TextureManager& GetTextureManager() const;
	bool UsesTransparency() const;
private:
	//datamembers
	Effect* m_pEffect;
	size_t m_AmountIndices;

	TextureManager m_TextureManager;
	std::vector<inputVertex> m_Vertices;
	std::vector<uint32_t> m_Indices;

	ID3D11InputLayout* m_pVertexLayout;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	ID3D11Device* m_pDevice;
	ID3DX11EffectTechnique* m_pTechnique;

	TextureFiltering m_TextureFiltering;
	const float m_RotationSpeed;
	float m_RotationY;
	FMatrix4 m_WorldMatrix,m_WorldVieuwProj;
	bool m_HasTransparency;

	//functions
	OutputVertex TransformVertex(const inputVertex& input, const FPoint3& cameraPos) const;
	FVector3 ApplyWorldtoVector(const FVector3& vector) const;
	void UpdateWorldVieuwProjectionMatrix(const Camera* pCamera);
	void PerspectiveDivide(FPoint4& vieuwPoint) const;
	bool IsInRange(float value, float min, float max) const;
};


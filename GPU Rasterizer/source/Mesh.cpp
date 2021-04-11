#include "pch.h"
#include "Mesh.h"
#include "PhongEffect.h"
#include "Effect.h"
#include "Camera.h"


Mesh::Mesh(ID3D11Device* pDevice, const char* imagePath, const char* normalPath, const char* specularPath, const char* glossinessPath, Effect* pEffect, bool hasTransparency, float shininess)
	:m_pEffect{ pEffect }
	, m_pTechnique{ m_pEffect->GetTechnique() }
	, m_pDevice{ pDevice }
	, m_TextureManager{ imagePath,normalPath,specularPath,glossinessPath,pDevice,25.f }
	, m_TextureFiltering{ TextureFiltering::point }
	, m_RotationY{}
	, m_RotationSpeed{ 15.f }
	, m_HasTransparency{ hasTransparency }
{
	//Create vertex layout

	HRESULT result = S_OK;
	static const uint32_t numElements{ 4 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "NORMAL";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "TANGENT";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 24;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "TEXCOORD";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 36;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//create the input layout
	if (m_pTechnique->IsValid())
	{
		D3DX11_PASS_DESC passDesc;
		m_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);
		result = pDevice->CreateInputLayout(vertexDesc, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pVertexLayout);
		if (FAILED(result)) return;
	}
}

Mesh::~Mesh()
{
	delete m_pEffect;

	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
	}
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
	}
	if (m_pVertexLayout)
	{
		m_pVertexLayout->Release();
	}
}


void Mesh::RenderInDirectX(ID3D11DeviceContext* pDeviceContext, const FMatrix4& vieuwMatrix)
{
	FMatrix4 rotationMatrix{ Elite::MakeRotationY(Elite::ToRadians(m_RotationY)),FVector3{} };

	m_pEffect->UpdateMatrices(vieuwMatrix, FMatrix4::Identity() * rotationMatrix);

	if (m_TextureManager.diffuseTexture.IsValid())	m_pEffect->SetDiffuseMap(m_TextureManager.diffuseTexture.GetShaderResourceVieuw());
	if (m_TextureManager.glossMap.IsValid())	m_pEffect->SetGlossinessMap(m_TextureManager.glossMap.GetShaderResourceVieuw());
	if (m_TextureManager.normalMap.IsValid())	m_pEffect->SetNormalMap(m_TextureManager.normalMap.GetShaderResourceVieuw());
	if (m_TextureManager.specularMap.IsValid())	m_pEffect->SetSpecularMap(m_TextureManager.specularMap.GetShaderResourceVieuw());

	//set vertex buffer
	UINT stride = sizeof(inputVertex);
	UINT offset = 0;

	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//set index buffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//set the input layout
	pDeviceContext->IASetInputLayout(m_pVertexLayout);

	//set primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Render a triangle
	if (m_pTechnique->IsValid())
	{
		D3DX11_TECHNIQUE_DESC techDesc;
		m_pTechnique->GetDesc(&techDesc);

		m_pTechnique->GetPassByIndex(static_cast<int>(m_TextureFiltering))->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(UINT(m_AmountIndices), 0, 0);
	}
}

void Mesh::Update(float elapsedTime)
{
	m_RotationY += elapsedTime * m_RotationSpeed;

	if (m_RotationY > 360.f) m_RotationY -= 360.f; //prevents the float from reaching its limit
	if (m_RotationY < -360.f) m_RotationY += 360.f; //prevents the float from reaching its limit

	m_WorldMatrix = { MakeRotationY(ToRadians(-m_RotationY)), FVector3{} };
}

void Mesh::SwitchTextureFilter()
{
	++m_TextureFiltering;

	switch (m_TextureFiltering)
	{
	case TextureFiltering::point:
		cout << "Point filtering activated. \n";
		break;
	case TextureFiltering::linear:
		cout << "Linear filtering activated. \n";
		break;
	case TextureFiltering::anisotropic:
		cout << "Anisotropic filtering activated. \n";
		break;
	default:
		cout << "No info given for this mode. \n";
		break;
	}
}

size_t Mesh::AddVertex(const inputVertex& vertex)
{
	m_Vertices.push_back(vertex);
	return m_Vertices.size() - 1;
}

void Mesh::AddIndices(const IPoint3& indices)
{
	m_Indices.push_back(indices.x);
	m_Indices.push_back(indices.y);
	m_Indices.push_back(indices.z);
}


pair<bool, int> Mesh::CheckForDublicate(const inputVertex& vertex)
{
	for (size_t i = 0; i < m_Vertices.size(); i++)
	{
		if (m_Vertices.at(i) == vertex)
		{
			return { true,int(i) };
		}
	}
	return { false,0 };
}

void Mesh::CreateBuffersForGpu()
{
	HRESULT result = S_OK;

	//create vertex buffer
	CD3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(inputVertex) * (uint32_t)m_Vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = m_Vertices.data();
	result = m_pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result)) return;

	//create index buffer
	m_AmountIndices = (uint32_t)m_Indices.size();
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = UINT(sizeof(uint32_t) * m_AmountIndices);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = m_Indices.data();
	result = m_pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(result)) return;
}

void Mesh::CalculateAndInitializeVertexTangent()
{
	if (true)//check if normal map is valid
	{
		for (size_t i = 0; i < m_Indices.size(); i += 3)
		{
			const IPoint3 indices{ int(m_Indices.at(i)), int(m_Indices.at(i + 1)), int(m_Indices.at(i + 2)) };

			const FPoint3& p0 = m_Vertices.at(indices.x).position;
			const FPoint3& p1 = m_Vertices.at(indices.y).position;
			const FPoint3& p2 = m_Vertices.at(indices.z).position;
			const FVector2& uv0 = m_Vertices.at(indices.x).uv;
			const FVector2& uv1 = m_Vertices.at(indices.y).uv;
			const FVector2& uv2 = m_Vertices.at(indices.z).uv;

			const FVector3 edge0 = p1 - p0;
			const FVector3 edge1 = p2 - p0;
			const FVector2 diffX = FVector2(uv1.x - uv0.x, uv2.x - uv0.x);
			const FVector2 diffY = FVector2(uv1.y - uv0.y, uv2.y - uv0.y);
			const float r = 1.f / Cross(diffX, diffY);

			FVector3 vertexTangent{ (edge0 * diffY.y - edge1 * diffY.x) * r };
			m_Vertices.at(indices.x).tangent += vertexTangent;
			m_Vertices.at(indices.y).tangent += vertexTangent;
			m_Vertices.at(indices.z).tangent += vertexTangent;
		}
		for (size_t i = 0; i < m_Vertices.size(); i++)
		{
			m_Vertices.at(i).tangent = GetNormalized(Reject(m_Vertices.at(i).tangent, m_Vertices.at(i).normal));
		}
	}
}

void Mesh::InvertZComponentsVertices()
{
	for (inputVertex& vertex : m_Vertices)
	{
		vertex.normal.z *= -1.f;
		vertex.tangent.z *= -1.f;
		vertex.position.z *= -1.f;
	}
}


std::vector<TriangleOutput> Mesh::GetTriangles(const Camera* pCamera)
{
	UpdateWorldVieuwProjectionMatrix(pCamera);

	std::vector<TriangleOutput> triangles;

	for (size_t i = 0; i < m_Indices.size(); i += 3)
	{
		TriangleOutput ndcTriangle{ TransformVertex(m_Vertices.at(m_Indices.at(i)),pCamera->GetPos()),TransformVertex(m_Vertices.at(m_Indices.at(i + 2)),
			pCamera->GetPos()),TransformVertex(m_Vertices.at(m_Indices.at(i + 1)),pCamera->GetPos()) };

		PerspectiveDivide(ndcTriangle.v0.point);
		PerspectiveDivide(ndcTriangle.v1.point);
		PerspectiveDivide(ndcTriangle.v2.point);

		//check culling
		if (IsInRange(ndcTriangle.v0.point.x, -1.f, 1.f) && IsInRange(ndcTriangle.v1.point.x, -1.f, 1.f) && IsInRange(ndcTriangle.v2.point.x, -1.f, 1.f) && //check x culling
			IsInRange(ndcTriangle.v0.point.y, -1.f, 1.f) && IsInRange(ndcTriangle.v1.point.y, -1.f, 1.f) && IsInRange(ndcTriangle.v2.point.y, -1.f, 1.f))		//check y culling
		{
			triangles.push_back(ndcTriangle);
		}
	}
	return triangles;
}

const TextureManager& Mesh::GetTextureManager() const
{
	return m_TextureManager;
}

bool Mesh::UsesTransparency() const
{
	return m_HasTransparency;
}

OutputVertex Mesh::TransformVertex(const inputVertex& input, const FPoint3& cameraPos) const
{
	FPoint3 posInWorld{ (m_WorldMatrix * FPoint4{ input.position}).xyz };
	FVector3 	vieuwDir{ GetNormalized(FVector3{ posInWorld - cameraPos}) };

	return	OutputVertex{ m_WorldVieuwProj * FPoint4{ input.position } , input.uv,ApplyWorldtoVector(input.normal),ApplyWorldtoVector(input.tangent), vieuwDir };
}

FVector3 Mesh::ApplyWorldtoVector(const FVector3& vector) const
{
	FVector4 vector4D{ vector,1.f };

	vector4D = m_WorldMatrix * vector4D;

	return  vector4D.xyz;
}

void Mesh::UpdateWorldVieuwProjectionMatrix(const Camera* pCamera)
{
	m_WorldVieuwProj = pCamera->GetProjectionMatrixRightHanded() * pCamera->GetVieuwMatrix() * m_WorldMatrix;
}

void Mesh::PerspectiveDivide(FPoint4& vieuwPoint) const
{
	vieuwPoint.xyz /= vieuwPoint.w;
}

bool Mesh::IsInRange(float value, float min, float max) const
{
	return (value > min && value < max);
}

void operator++(TextureFiltering& filter)
{
	//increment to next next spot in the enum
	int newValue{ static_cast<int>(filter) + 1 };
	//reset the enum when we hit the end
	if (static_cast<TextureFiltering>(newValue) == TextureFiltering::end) newValue = 0;

	filter = static_cast<TextureFiltering>(newValue);
}

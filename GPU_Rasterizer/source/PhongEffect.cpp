#include "pch.h"
#include "PhongEffect.h"

PhongEffect::PhongEffect(ID3D11Device* pDevice, const FMatrix4& projectionMatrix)
	:Effect{pDevice,projectionMatrix,L"Resources/PosCol3D.fx"}
{
	m_pWorldMatrix = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pWorldMatrix->IsValid()) std::wcout << L"world  matrix is not valid. \n";

	m_pInverseVieuwMatrix = m_pEffect->GetVariableByName("gVieuwInverseMatrix")->AsMatrix();
	if (!m_pInverseVieuwMatrix->IsValid()) std::wcout << L"vieuw inverse matrixis not valid. \n";

	m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
	if (!m_pSpecularMapVariable->IsValid()) std::wcout << L"Variable gSpecularMap not valid. \n";

	m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
	if (!m_pNormalMapVariable->IsValid()) std::wcout << L"Variable gNormalMap not valid. \n";

	m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
	if (!m_pGlossinessMapVariable->IsValid()) std::wcout << L"Variable gGlossinessMap not valid. \n";
}
PhongEffect::~PhongEffect()
{
	if (m_pGlossinessMapVariable->IsValid())
	{
		m_pGlossinessMapVariable->Release();
	}
	if (m_pNormalMapVariable->IsValid())
	{
		m_pNormalMapVariable->Release();
	}
	if (m_pSpecularMapVariable->IsValid())
	{
		m_pSpecularMapVariable->Release();
	}
	if (m_pInverseVieuwMatrix->IsValid())
	{
		m_pInverseVieuwMatrix->Release();
	}
	if (m_pWorldMatrix->IsValid())
	{
		m_pWorldMatrix->Release();
	}
}

void PhongEffect::UpdateMatrices(const FMatrix4& vieuwMatrix, const FMatrix4& worldMatrix)
{
	Effect::UpdateMatrices(vieuwMatrix, worldMatrix);

	const FMatrix4 inverseVieuwMatrix{ Inverse(vieuwMatrix) };

	m_pWorldMatrix->SetMatrix(reinterpret_cast<const float*>(&worldMatrix));
	m_pInverseVieuwMatrix->SetMatrix(reinterpret_cast<const float*>(&inverseVieuwMatrix));
}

void PhongEffect::SetNormalMap(ID3D11ShaderResourceView* pResourceVieuw)
{
	if (m_pNormalMapVariable->IsValid()) m_pNormalMapVariable->SetResource(pResourceVieuw);
}

void PhongEffect::SetSpecularMap(ID3D11ShaderResourceView* pResourceVieuw)
{
	if (m_pSpecularMapVariable->IsValid()) m_pSpecularMapVariable->SetResource(pResourceVieuw);
}

void PhongEffect::SetGlossinessMap(ID3D11ShaderResourceView* pResourceVieuw)
{
	if (m_pGlossinessMapVariable->IsValid()) m_pGlossinessMapVariable->SetResource(pResourceVieuw);
}

#include "pch.h"
#include "Effect.h"
#include <iostream>
#include <sstream>

Effect::Effect(ID3D11Device* pDevice, const FMatrix4& projectionMatrix, const std::wstring& assetFile)
	:m_pEffect{ LoadEffect(pDevice,assetFile) }
	, m_ProjectionMatrix{ projectionMatrix }
{
	if (m_pEffect != nullptr) m_pTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");
	if (!m_pTechnique->IsValid())
	{
		std::cout << "Technique is not valid. \n";
	}
	m_pWorldVieuwProjMatrix = m_pEffect->GetVariableByName("gWorldVieuwProj")->AsMatrix();
	if (!m_pWorldVieuwProjMatrix->IsValid()) std::wcout << L"world vieuw matrix is not valid. \n";
	
	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid()) std::wcout << L"Variable gDiffuseMap not valid. \n";

}

Effect::~Effect()
{
	if (m_pDiffuseMapVariable->IsValid())
	{
		m_pDiffuseMapVariable->Release();
	}
	if (m_pWorldVieuwProjMatrix->IsValid())
	{
		m_pWorldVieuwProjMatrix->Release();
	}
	if (m_pTechnique->IsValid())
	{
		m_pTechnique->Release();
	}
	if (m_pEffect->IsValid())
	{
		m_pEffect->Release();
	}
}

void Effect::UpdateMatrices(const FMatrix4& vieuwMatrix, const FMatrix4& worldMatrix)
{
	const FMatrix4 wvpMatrix{ m_ProjectionMatrix * vieuwMatrix  * worldMatrix};

	m_pWorldVieuwProjMatrix->SetMatrix(reinterpret_cast<const float*>(&wvpMatrix));
}

ID3DX11Effect* Effect::GetEffect() const
{
	return m_pEffect;
}

ID3DX11EffectTechnique* Effect::GetTechnique() const
{
	return m_pTechnique;
}

void Effect::SetDiffuseMap(ID3D11ShaderResourceView* pResourceVieuw)
{
	if (m_pDiffuseMapVariable->IsValid()) m_pDiffuseMapVariable->SetResource(pResourceVieuw);
}

void Effect::SetNormalMap(ID3D11ShaderResourceView* pResourceVieuw)
{
}

void Effect::SetSpecularMap(ID3D11ShaderResourceView* pResourceVieuw)
{
}

void Effect::SetGlossinessMap(ID3D11ShaderResourceView* pResourceVieuw)
{
}


ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	HRESULT result = S_OK;
	ID3D10Blob* pErrorBlob = nullptr;
	ID3DX11Effect* pEffect;

	DWORD shaderFlags = 0;

#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile(assetFile.c_str(), nullptr, nullptr, shaderFlags, 0, pDevice, &pEffect, &pErrorBlob);

	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			char* pErrors = (char*)pErrorBlob->GetBufferPointer();
			std::wstringstream ss{};

			for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); i++) ss << pErrors[i];

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << std::endl;
		}
		else
		{
			std::cout << "EffectLoader: Failed to create effect from file! \n Path:  " << assetFile.c_str() << std::endl;
			return nullptr;
		}
	}
	return pEffect;
}

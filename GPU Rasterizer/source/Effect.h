#pragma once
#include <string>
#include "EMath.h"

using namespace Elite;

struct ID3DX11Effect;
struct  ID3D11Device;
struct  ID3DX11EffectTechnique;
struct  ID3DX11EffectMatrixVariable;
struct  ID3DX11EffectShaderResourceVariable;
struct ID3D11ShaderResourceView;

class Effect
{
public:
	//rule of 5
	Effect(ID3D11Device* pDevice, const FMatrix4& projectionMatrix,const std::wstring& assetFile = L"Resources/FlatShader.fx");
	~Effect();	

	Effect(const Effect&) = delete;
	Effect(Effect&&) noexcept = delete;
	Effect& operator=(const Effect&) = delete;
	Effect& operator=(Effect&&) noexcept = delete;

	//functions
	virtual void UpdateMatrices(const FMatrix4& vieuwMatrix, const FMatrix4& worldMatrix);

	virtual ID3DX11Effect* GetEffect() const;
	virtual ID3DX11EffectTechnique* GetTechnique() const;

	virtual void SetDiffuseMap(ID3D11ShaderResourceView* pResourceVieuw);
	virtual void SetNormalMap(ID3D11ShaderResourceView* pResourceVieuw);
	virtual void SetSpecularMap(ID3D11ShaderResourceView* pResourceVieuw);
	virtual void SetGlossinessMap(ID3D11ShaderResourceView* pResourceVieuw);
protected:
	//datamembers
	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechnique;
	ID3DX11EffectMatrixVariable* m_pWorldVieuwProjMatrix;
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;
	const FMatrix4 m_ProjectionMatrix;
	//functions
	ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
};


#pragma once
#include "Effect.h"

struct  ID3DX11EffectMatrixVariable;
struct  ID3DX11EffectShaderResourceVariable;

class PhongEffect final : public Effect
{
public:
	//rule of 5
	PhongEffect(ID3D11Device* pDevice, const FMatrix4& projectionMatrix);
	~PhongEffect();

	PhongEffect(const PhongEffect&) = delete;
	PhongEffect(PhongEffect&&) noexcept = delete;
	PhongEffect& operator=(const PhongEffect&) = delete;
	PhongEffect& operator=(PhongEffect&&) noexcept = delete;

	//functions
	void UpdateMatrices(const FMatrix4& vieuwMatrix, const FMatrix4& worldMatrix);

	void SetNormalMap(ID3D11ShaderResourceView* pResourceVieuw);
	void SetSpecularMap(ID3D11ShaderResourceView* pResourceVieuw);
	void SetGlossinessMap(ID3D11ShaderResourceView* pResourceVieuw);
private:
	//datamembers
	ID3DX11EffectMatrixVariable * m_pWorldMatrix, * m_pInverseVieuwMatrix;
	ID3DX11EffectShaderResourceVariable*  m_pNormalMapVariable, * m_pSpecularMapVariable, * m_pGlossinessMapVariable;
};


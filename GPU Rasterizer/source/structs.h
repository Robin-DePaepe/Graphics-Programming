#pragma once
#include "EMath.h"
#include "ERGBColor.h"
#include "Texture.h"

using namespace Elite;
struct ID3D11Device;

struct DirectionalLight
{
	DirectionalLight(const FVector3& direction, float intensity, const RGBColor& color = { 1.f,1.f,1.f });

	const float intensity;
	const RGBColor lightColor;
	const FVector3 lightDirection;
};


//input output sturcts
struct inputVertex
{
	inputVertex(const FPoint3& pos, const FVector3& normal, const FVector2& uv);
	bool operator==(const inputVertex& other);

	FPoint3 position = {};
	FVector3 normal = {};
	FVector3 tangent = {};
	FVector2 uv = {};
};

struct OutputVertex
{
	OutputVertex(const FPoint4& point, const FVector2& uv, const FVector3& normal, const FVector3& tangent, const FVector3& vieuwDir, const RGBColor& color = { 1.f,1.f,1.f });

	FPoint4 point = {};
	RGBColor color = {};
	FVector2 uv = {};
	FVector3 normal = {};
	FVector3 tangent = {};
	FVector3 vieuwDirection = {};
};

struct TextureManager
{
	TextureManager(const std::string& diffusePath, const std::string& normalMapPath, const std::string& specularMapPath, const std::string& glossyMapPath, ID3D11Device* pDevice, float shininess = 20.f);

	//datamembers
	const Texture diffuseTexture;
	const Texture glossMap;
	const Texture normalMap;
	const Texture specularMap;
	const float shininess;
};


struct TriangleOutput
{
	OutputVertex v0, v1, v2;

	bool Hit(const OutputVertex& pixel) const;
	void InterpolatePixelValues(OutputVertex& pixel) const;
};
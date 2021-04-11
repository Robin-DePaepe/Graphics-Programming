#pragma once
#include <unordered_map>
#include "EMath.h"
#include "ERGBColor.h"
#include "Texture.h"

using namespace Elite;

struct InputVertex
{
	InputVertex(const FPoint3& point, const FVector2& uv, const FVector3& normal, const RGBColor& color = { 1.f,1.f,1.f });
	bool operator==(const InputVertex& other);

	FPoint3 point = {};
	RGBColor color = {};
	FVector2 uv = {};
	FVector3 normal = {};
	FVector3 tangent = {};
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

struct TriangleInput
{
	InputVertex v0, v1, v2;
};

struct TriangleOutput
{
	OutputVertex v0, v1, v2;

	bool Hit(const OutputVertex& pixel) const;
	void InterpolatePixelValues(OutputVertex& pixel) const;
};

struct DirectionalLight
{
	DirectionalLight(const FVector3& direction, float intensity, const RGBColor& color = { 1.f,1.f,1.f });

	const float intensity;
	const RGBColor lightColor;
	const FVector3 lightDirection;
};

struct TextureManager
{
	TextureManager(const std::string& diffusePath, const std::string& normalMapPath, const std::string& specularMapPath, const std::string& glossyMapPath, bool usePhong, float shininess);
	//datamembers
	const Texture diffuseTexture;
	const Texture glossMap;
	const Texture normalMap;
	const Texture specularMap;
	const bool usePhong;
	const float shininess;
};
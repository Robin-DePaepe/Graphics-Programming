#include "Structs.h"
#include "EMath.h"

using namespace Elite;


bool TriangleOutput::Hit(const OutputVertex& pixel) const
{
	const FPoint3 point0{ v0.point.x , v0.point.y, v0.point.z };

	const FVector3 EdgeA{ FPoint3{ v1.point.x , v1.point.y, v1.point.z } -point0 }, EdgeB{ FPoint3{ v2.point.x , v2.point.y, v2.point.z } -point0 };

	const FVector3 sideToPoint{ FPoint3{ pixel.point.x , pixel.point.y, pixel.point.z } -point0 };

	//Barycentric Technique

	const float dotEdgeA{ Dot(EdgeA,EdgeA) };
	const float dotEdgeAB{ Dot(EdgeA,EdgeB) };
	const float dotEdgeASideToPoint{ Dot(EdgeA,sideToPoint) };
	const float dotEdgeB{ Dot(EdgeB,EdgeB) };
	const float dotEdgeBSideToPoint{ Dot(EdgeB,sideToPoint) };

	const float invDenom = 1.f / (dotEdgeA * dotEdgeB - dotEdgeAB * dotEdgeAB);
	const float u = (dotEdgeB * dotEdgeASideToPoint - dotEdgeAB * dotEdgeBSideToPoint) * invDenom;
	const float v = (dotEdgeA * dotEdgeBSideToPoint - dotEdgeAB * dotEdgeASideToPoint) * invDenom;

	//check if point is in triangle
	return ((u >= 0) && (v >= 0) && (u + v < 1));
}

void TriangleOutput::InterpolatePixelValues(OutputVertex& pixel) const
{
	const FPoint2 point2D0{ v0.point.x , v0.point.y };
	const FPoint2 point2D1{ v1.point.x , v1.point.y };
	const FPoint2 point2D2{ v2.point.x , v2.point.y };
	const FPoint2 pixel2D{ pixel.point.x,pixel.point.y };

	//Determine weights
	const float areaTriangle{ Elite::Cross((point2D0 - point2D1),(point2D0 - point2D2)) };

	const float weight0{ std::abs(Elite::Cross((pixel2D - point2D1),(point2D2 - point2D1)) / areaTriangle) };
	const float weight1{ std::abs(Elite::Cross((pixel2D - point2D2),(point2D0 - point2D2)) / areaTriangle) };
	const float weight2{ std::abs(Elite::Cross((pixel2D - point2D0),(point2D1 - point2D0)) / areaTriangle) };

	//	if false something went wrong with the calculations
	if (std::abs(weight0 + weight1 + weight2 - 1.f) < 0.1f)
	{
		//Determine depth
		pixel.point.z = 1.f / (1.f / v0.point.z * weight0 + 1.f / v1.point.z * weight1 + 1.f / v2.point.z * weight2);
		pixel.point.w = 1.f / (1.f / v0.point.w * weight0 + 1.f / v1.point.w * weight1 + 1.f / v2.point.w * weight2);

		//Calc interpolated values
		pixel.uv = { (v0.uv / v0.point.w * weight0 + v1.uv / v1.point.w * weight1 + v2.uv / v2.point.w * weight2) * pixel.point.w };
		pixel.normal = { (v0.normal / v0.point.w * weight0 + v1.normal / v1.point.w * weight1 + v2.normal / v2.point.w * weight2) * pixel.point.w };
		pixel.tangent = { (v0.tangent / v0.point.w * weight0 + v1.tangent / v1.point.w * weight1 + v2.tangent / v2.point.w * weight2) * pixel.point.w };
		pixel.vieuwDirection = { (v0.vieuwDirection / v0.point.w * weight0 + v1.vieuwDirection / v1.point.w * weight1 + v2.vieuwDirection / v2.point.w * weight2) * pixel.point.w };

		//interpolation does not preserve normalization
		Normalize(pixel.normal);
	}
}

InputVertex::InputVertex(const FPoint3& point, const FVector2& uv, const FVector3& normal, const RGBColor& color)
	:point{ point }
	, color{ color }
	, uv{ uv }
	, normal{ normal }
{
}

bool InputVertex::operator==(const InputVertex& other)
{
	return (this->uv == other.uv && this->normal == other.normal && this->point == other.point);
}

OutputVertex::OutputVertex(const FPoint4& point, const FVector2& uv, const FVector3& normal, const FVector3& tangent, const FVector3& vieuwDir, const RGBColor& color)
	:point{ point }
	, color{ color }
	, uv{ uv }
	, normal{ normal }
	, tangent{ tangent }
	, vieuwDirection{ vieuwDir }
{
}

DirectionalLight::DirectionalLight(const FVector3& direction, float intensity, const RGBColor& color)
	:lightColor{ color }
	, lightDirection{ direction }
	, intensity{ intensity }
{
}

TextureManager::TextureManager(const std::string& diffusePath, const std::string& normalMapPath, const std::string& specularMapPath, const std::string& glossyMapPath, bool usePhong, float shininess)
	:diffuseTexture{ diffusePath.c_str() }
	, normalMap{ normalMapPath.c_str() }
	, specularMap{ specularMapPath.c_str() }
	, glossMap{ glossyMapPath.c_str() }
	, usePhong{ usePhong }
	, shininess{ shininess }
{
}


#include "pch.h"
#include "BRDF.h"

const Elite::RGBColor BRDF::Lambert(float diffuseReflactance, const Elite::RGBColor& color)
{
	return Lambert(Elite::RGBColor{diffuseReflactance,diffuseReflactance,diffuseReflactance}, color);
}

const Elite::RGBColor BRDF::Lambert(const Elite::RGBColor& diffuseReflactance, const Elite::RGBColor& color)
{
	return (color * diffuseReflactance) / float(E_PI);
}

const Elite::RGBColor BRDF::Phong(float specularReflactance, float phongExponent, const Elite::FVector3& l, const Elite::FVector3& v, const Elite::FVector3& normal)
{
	Elite::FVector3 reflect{ Elite::GetNormalized(-l + 2 * (Elite::Dot(normal,l) * normal)) };

	const float cosine{ Elite::Dot(reflect,v) };
	const float result{ specularReflactance * powf(cosine, phongExponent) };

	return Elite::RGBColor{ result, result , result };
}


const Elite::RGBColor BRDF::PBR_CookTorrance(const Elite::FVector3& l, const Elite::FVector3& v, const Elite::FVector3& normal, float roughness, const Elite::RGBColor& albedo, Elite::RGBColor& fresnel, bool metal)
{
	Elite::FVector3 halfVector{ GetHalfVector(l,v) };
	Elite::Normalize(halfVector);

	const float roughnessSquared{ powf(roughness,2) };
	const float k{ powf(roughnessSquared + 1,2) / 8 };

	const float D{ NormalDistribution(normal,halfVector,roughnessSquared) };
	fresnel = Fresnel(halfVector, v, albedo, metal);
	const float G{ Geometry(normal,v,l, k) };

	const float denominator{ 4 * Elite::Dot(v,normal) * Elite::Dot(l,normal) };
	const Elite::RGBColor nominator{ D * fresnel.r * G,D * fresnel.g * G,D * fresnel.b * G };

	return nominator / denominator;
}

float BRDF::NormalDistribution(const Elite::FVector3& normal, const Elite::FVector3& halfVector, float alpha)
{
	const float alphaSquared{ powf(alpha,2) };
	const float denominator{ float(E_PI * powf(powf(Elite::Dot(normal,halfVector) ,2) * (alphaSquared - 1) + 1,2)) };

	return alphaSquared / denominator;
}

Elite::RGBColor BRDF::Fresnel(const Elite::FVector3& halfVector, const Elite::FVector3& v, const Elite::RGBColor& albedo, bool metal)
{
	const Elite::RGBColor F0{ (metal == false) ? Elite::RGBColor{0.04f,0.04f,0.04f} : albedo };

	return F0 + (Elite::RGBColor(1.f, 1.f, 1.f) - F0) * powf(1 - Elite::Dot(halfVector, v), 5);
}

float BRDF::Geometry(const Elite::FVector3& normal, const Elite::FVector3& v, const Elite::FVector3& l, float k)
{
	return Geometry(normal, v, k) * Geometry(normal, l, k);
}

float BRDF::Geometry(const Elite::FVector3& normal, const Elite::FVector3& v, float k)
{
	const float nominator{ Elite::Dot(normal,v) };
	const float denominator{ nominator * (1 - k) + k };

	return nominator / denominator;
}

const Elite::FVector3 BRDF::GetHalfVector(const Elite::FVector3& l, const Elite::FVector3& v)
{
	return (v + l) / Elite::SqrMagnitude(v + l);
}

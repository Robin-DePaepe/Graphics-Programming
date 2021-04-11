#pragma once
#include "ERGBColor.h"
#include "EMath.h"
class BRDF
{
public:
	//functions
	static const Elite::RGBColor Lambert(float diffuseReflactance, const Elite::RGBColor& color);
	static const Elite::RGBColor Lambert(const Elite::RGBColor& diffuseReflactance, const Elite::RGBColor& color);
	static const Elite::RGBColor Phong(float specularReflactance, float phongExponent, const Elite::FVector3& l, const Elite::FVector3& v, const Elite::FVector3& normal);
	static const Elite::RGBColor PBR_CookTorrance(const Elite::FVector3& l, const Elite::FVector3& v, const Elite::FVector3& normal, float roughness, const Elite::RGBColor& albedo, Elite::RGBColor& fresnel, bool metal);

private:
	//functions
	static float NormalDistribution(const Elite::FVector3& normal, const Elite::FVector3& halfVector, float alpha);
	static  Elite::RGBColor Fresnel(const Elite::FVector3& halfVector, const Elite::FVector3& v,const Elite::RGBColor& albedo, bool metal);
	static float Geometry(const Elite::FVector3& normal, const Elite::FVector3& v, const Elite::FVector3& l, float k);
	static float Geometry(const Elite::FVector3& normal, const Elite::FVector3& v, float k);
	static const Elite::FVector3 GetHalfVector(const Elite::FVector3& l, const Elite::FVector3& v);
};


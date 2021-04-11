#include "pch.h"
#include "Mat_LambertPhong.h"
#include "BRDF.h"

Mat_LambertPhong::Mat_LambertPhong(float diffuseReflactance, const RGBColor& color, float specularReflectance, float phongExponent)
	:m_DiffuseColor{ color }
	, m_PhongExponent{ phongExponent }
	, m_SpecularReflectance{ specularReflectance }
	, m_DiffuseReflectance{ diffuseReflactance }
{
}

RGBColor Mat_LambertPhong::Shade(const HitRecord& hitRecord, const FVector3& l, const FVector3& v)
{
	return BRDF::Lambert(m_DiffuseReflectance, m_DiffuseColor) + BRDF::Phong(m_SpecularReflectance, m_PhongExponent, l, v, hitRecord.normal);
}

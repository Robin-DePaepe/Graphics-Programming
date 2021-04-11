#include "pch.h"
#include "Mat_Lambert.h"
#include "BRDF.h"

Mat_Lambert::Mat_Lambert(float reflectance, const RGBColor& color)
	:m_DiffuseColor{ color }
	, m_DiffuseReflectance{ reflectance }
{
}

RGBColor Mat_Lambert::Shade(const HitRecord& hitRecord, const FVector3& l, const FVector3& v)
{
	return BRDF::Lambert(m_DiffuseReflectance, m_DiffuseColor);
}
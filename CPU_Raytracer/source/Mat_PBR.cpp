#include "pch.h"
#include "Mat_PBR.h"
#include "BRDF.h"

Mat_PBR::Mat_PBR(float roughness, const RGBColor& albedo, bool metal)
	:m_Albedo{ albedo }
	, m_Roughness{ roughness }
	, m_Fresnel{}
	, m_Metal{ metal }
{
}

RGBColor Mat_PBR::Shade(const HitRecord& hitRecord, const FVector3& l, const FVector3& v)
{
const	Elite::RGBColor specular{ BRDF::PBR_CookTorrance(l,-v,hitRecord.normal,m_Roughness,m_Albedo,m_Fresnel,m_Metal) };

	if (m_Metal)
	{
		return  specular;
	}

	const Elite::RGBColor kd{ Elite::RGBColor{1.f,1.f,1.f} -m_Fresnel };
	const Elite::RGBColor diffuse{ BRDF::Lambert(kd,m_Albedo) };

	return  diffuse + specular;
}

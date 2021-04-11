#pragma once
#include "Material.h"

class Mat_PBR : public Material
{
public:
	//rule of 5
	Mat_PBR( float roughness, const RGBColor& albedo , bool metal);
	~Mat_PBR() = default;

	Mat_PBR(const Mat_PBR& other) = delete;
	Mat_PBR(Mat_PBR&& other) = delete;
	Mat_PBR& operator=(const Mat_PBR& rhs) = delete;
	Mat_PBR& operator=(Mat_PBR&& other) = delete;

	//functions
	 RGBColor Shade(const HitRecord& hitRecord, const FVector3& l, const FVector3& v)  override;

private:
	//datamembers
	const Elite::RGBColor m_Albedo;
	Elite::RGBColor m_Fresnel;
	const float m_Roughness;
	const bool m_Metal;
};


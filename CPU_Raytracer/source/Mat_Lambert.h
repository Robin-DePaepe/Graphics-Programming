#pragma once
#include "Material.h"

class Mat_Lambert : public Material
{
public:
	//rule of 5
	Mat_Lambert(float reflectance, const RGBColor& color);
	~Mat_Lambert() = default;

	Mat_Lambert(const Mat_Lambert& other) = delete;
	Mat_Lambert(Mat_Lambert&& other) = delete;
	Mat_Lambert& operator=(const Mat_Lambert& rhs) = delete;
	Mat_Lambert& operator=(Mat_Lambert&& other) = delete;

	//functions
	RGBColor Shade(const HitRecord& hitRecord, const FVector3& l, const FVector3& v)  override;

private:
	//datamembers
	const float m_DiffuseReflectance; //value between [0,1]
	const RGBColor m_DiffuseColor;
};

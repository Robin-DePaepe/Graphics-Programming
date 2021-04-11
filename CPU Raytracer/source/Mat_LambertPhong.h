#pragma once
#include "Material.h"


class Mat_LambertPhong : public Material
{
public:
	//rule of 5
	Mat_LambertPhong(float diffuseReflactance, const RGBColor& color, float specularReflectance, float phongExponent);
	~Mat_LambertPhong() = default;

	Mat_LambertPhong(const Mat_LambertPhong& other) = delete;
	Mat_LambertPhong(Mat_LambertPhong&& other) = delete;
	Mat_LambertPhong& operator=(const Mat_LambertPhong& rhs) = delete;
	Mat_LambertPhong& operator=(Mat_LambertPhong&& other) = delete;

	//functions
	RGBColor Shade(const HitRecord& hitRecord, const FVector3& l, const FVector3& v)  override;

private:
	//datamembers
	const float m_SpecularReflectance; //value between [0,1]
	const float m_DiffuseReflectance; //value between [0,1]
	const float m_PhongExponent;
	const RGBColor m_DiffuseColor;
};


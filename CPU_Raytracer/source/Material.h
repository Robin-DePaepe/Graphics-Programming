#pragma once
#include "ERGBColor.h"
#include "EMath.h"
#include "Structs.h"

using namespace Elite;

class Material
{
public:
	//rule of 5
	Material() = default;
	virtual ~Material() = default;

	Material(const Material& other) = delete;
	Material(Material&& other) = delete;
	Material& operator=(const Material& rhs) = delete;
	Material& operator=(Material&& other) = delete;

	//functions
	virtual RGBColor Shade(const HitRecord& hitRecord, const FVector3& l, const FVector3& v) = 0;
};


#pragma once
#include "ERGBColor.h"
#include "EMath.h"
#include <cstdint>

struct SDL_Surface;
using namespace Elite;

class Texture
{
public:
	//rule of 5
	Texture(const char* imagePath);
	~Texture();

	Texture(const Texture& other) = delete;
	Texture(Texture&& other) = delete;
	Texture& operator=(const Texture& rhs) = delete;
	Texture& operator=(Texture&& other) = delete;

	//functions
	RGBColor Sample(const FVector2& uv) const;
	bool IsValid() const;
private:
	//datamembers
	SDL_Surface *m_pSurface;
	uint32_t* m_Pixels;
	bool m_Valid;
	//fuctions
std::uint32_t	 GetPixel32(const FVector2& uv) const;
};


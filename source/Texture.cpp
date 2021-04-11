#include "Texture.h"
#include <SDL_image.h>

Texture::Texture(const char* imagePath)
	:m_pSurface{ IMG_Load(imagePath) }
	, m_Valid{ false }
{
	if (m_pSurface != nullptr)
	{
		m_Pixels = { (uint32_t*)m_pSurface->pixels };
		m_Valid = true;
	}
}

Texture::~Texture()
{
	if (m_Valid)
	{
		SDL_FreeSurface(m_pSurface);
		m_pSurface = nullptr;
	}
}

RGBColor Texture::Sample(const FVector2& uv) const
{
	Uint8 r, g, b;

	if (uv.x > 0.f && uv.x < 1.f && uv.y > 0.f && uv.y < 1.f) //check for valid uv
	{
		SDL_GetRGB(GetPixel32(uv), m_pSurface->format, &r, &g, &b);
	}

	return RGBColor{ r / 255.f, g / 255.f, b / 255.f };
}

bool Texture::IsValid() const
{
	return m_Valid;
}

std::uint32_t Texture::GetPixel32(const FVector2& uv) const
{
	const int y{ int(uv.y * m_pSurface->w) };
	const int x{ int(uv.x * m_pSurface->h) };

	return m_Pixels[(y * m_pSurface->w) + x];
}

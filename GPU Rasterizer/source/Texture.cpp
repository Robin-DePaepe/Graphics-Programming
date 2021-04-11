#include "pch.h"
#include "Texture.h"
#include <SDL_image.h>

Texture::Texture(const char* imagePath, ID3D11Device* pDevice)
	:m_Valid{ false }
	, m_pSurface{ IMG_Load(imagePath) }
{
	if (m_pSurface != nullptr)
	{
		if (InitializeTexureInDirectX(m_pSurface, pDevice))
		{
			m_Valid = true;
		}
		m_Pixels = { (uint32_t*)m_pSurface->pixels };
	}
}

Texture::~Texture()
{
	if (m_Valid)
	{
		m_pTextureResourceVieuw->Release();
		m_pTexture->Release();
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

ID3D11ShaderResourceView* Texture::GetShaderResourceVieuw() const
{
	return m_pTextureResourceVieuw;
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

bool Texture::InitializeTexureInDirectX(SDL_Surface* pSurface, ID3D11Device* pDevice)
{
	HRESULT result;

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = pSurface->w;
	desc.Height = pSurface->h;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = pSurface->pixels;
	initData.SysMemPitch = static_cast<UINT>(pSurface->pitch);
	initData.SysMemSlicePitch = static_cast<UINT>(pSurface->h * pSurface->pitch);

	result = pDevice->CreateTexture2D(&desc, &initData, &m_pTexture);

	if (result != S_OK) return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.Format = desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	result = pDevice->CreateShaderResourceView(m_pTexture, &SRVDesc, &m_pTextureResourceVieuw);

	return result == S_OK;
}


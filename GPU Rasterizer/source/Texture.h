#pragma once
#include "ERGBColor.h"
#include "EMath.h"
#include <cstdint>

using namespace Elite;
struct ID3D11ShaderResourceView;
struct ID3D11Device;
struct ID3D11Texture2D;

class Texture
{
public:
	//rule of 5
	Texture(const char* imagePath, ID3D11Device* pDevice);
	~Texture();

	Texture(const Texture& other) = delete;
	Texture(Texture&& other) = delete;
	Texture& operator=(const Texture& rhs) = delete;
	Texture& operator=(Texture&& other) = delete;

	//functions
	RGBColor Sample(const FVector2& uv) const;
	ID3D11ShaderResourceView* GetShaderResourceVieuw() const;
	bool IsValid() const;
private:
	//datamembers
	bool m_Valid;
	SDL_Surface* m_pSurface;
	uint32_t* m_Pixels;
	ID3D11ShaderResourceView* m_pTextureResourceVieuw;
	ID3D11Texture2D* m_pTexture;
	//fuctions
	std::uint32_t	 GetPixel32(const FVector2& uv) const;
	bool InitializeTexureInDirectX(SDL_Surface* pSurface, ID3D11Device* pDevice);
};


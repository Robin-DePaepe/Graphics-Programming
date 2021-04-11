/*=============================================================================*/
// Copyright 2017-2019 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ERenderer.h: class that holds the surface to render too + DirectX initialization.
/*=============================================================================*/
#ifndef ELITE_RAYTRACING_RENDERER
#define	ELITE_RAYTRACING_RENDERER

#include <cstdint>
#include <dxgi.h>
#include <vector>
#include "structs.h"

struct SDL_Window;
struct SDL_Surface;
class Mesh;
class Camera;

namespace Elite
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render(const std::vector<Mesh*>& pMeshes, const Camera* pCamera);
		ID3D11Device* GetDevice() const;
		void SwitchRenderState();
		void ToggleDirectX();
		void ToggleTransparency();

	private:
		//datamembers
		SDL_Window* m_pWindow;
		uint32_t m_Width;
		uint32_t m_Height;
		SDL_Surface* m_pFrontBuffer = nullptr;
		SDL_Surface* m_pBackBuffer = nullptr;
		uint32_t* m_pBackBufferPixels = nullptr;
		float** m_pDepthBuffer;
		const DirectionalLight m_DirectionalLight;

		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		IDXGIFactory* m_pDXGIFactory;
		IDXGISwapChain* m_pSwapChain;
		ID3D11Texture2D* m_pDepthStencilBuffer;
		ID3D11DepthStencilView* m_pDethStencilVieuw;
		ID3D11Resource *m_pRenderTargetBuffer;
		ID3D11RenderTargetView* m_pRenderTargetVieuw;
					
		bool m_IsInitialized;
		bool m_RenderDirectX;
		bool m_RenderFinalColor;
		bool m_RenderTransparency;

		//functions
		HRESULT InitializeDirectX();
		void ClearBuffers();
		void StartRenderer();
		void EndRenderer();
		void ColorPixel(uint32_t c, uint32_t r, const RGBColor& color);
		const FPoint4 GetScreenSpace(const FPoint4& point) const;

		const FPoint2 CalculateBoundingBoxY(const TriangleOutput& triangle) const;
		const FPoint2 CalculateBoundingBoxX(const TriangleOutput& triangle) const;

		const RGBColor PixelShading(const OutputVertex& vertex, const TextureManager& textures);
		void RenderDirectX(const std::vector<Mesh*>& pMeshes, const Camera* pCamera);
		void RenderOnCpu(const std::vector<Mesh*>& pMeshes, const Camera* pCamera);
	};
}

#endif
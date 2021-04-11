#ifndef ELITE_RAYTRACING_RENDERER
#define	ELITE_RAYTRACING_RENDERER

#include <cstdint>
#include "Structs.h"
#include "ERGBColor.h"
#include <array>

struct SDL_Window;
struct SDL_Surface;

namespace Elite
{
	class Renderer final
	{
	public:
		//rule of 5
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		//functions
		void RenderTriangle(const TriangleOutput& triangle, const TextureManager* pTextures);
		bool SaveBackbufferToImage() const;

		void StartRender();
		void EndRender();
		float GetAspectRatio() const;
		void SwitchRenderState();
	private:
		//datamembers
		SDL_Window* m_pWindow = nullptr;
		SDL_Surface* m_pFrontBuffer = nullptr;
		SDL_Surface* m_pBackBuffer = nullptr;
		uint32_t* m_pBackBufferPixels = nullptr;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		float ** m_pDepthBuffer;
		bool m_RenderFinalColor;
		const DirectionalLight m_DirectionalLight;
		//functions
		const FPoint4 GetScreenSpace(const FPoint4& point) const;
		void ColorPixel(uint32_t c, uint32_t r, const RGBColor& color);
		void ClearBackBuffer();
		const FPoint2 CalculateBoundingBoxY(const TriangleOutput& triangle) const;
		const FPoint2 CalculateBoundingBoxX(const TriangleOutput& triangle) const;
		const RGBColor PixelShading(const OutputVertex& vertex, const TextureManager* pTextures);
	};
}

#endif
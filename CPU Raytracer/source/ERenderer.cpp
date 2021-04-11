#include "pch.h"
//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "ERenderer.h"
#include "Camera.h"
#include "SceneManager.h"

Elite::Renderer::Renderer(SDL_Window* pWindow)
	:m_pWindow{ pWindow }
	, m_pFrontBuffer{ SDL_GetWindowSurface(pWindow) }
	, m_pBackBuffer{}
	, m_pBackBufferPixels{}
	, m_Width{}
	, m_Height{}
{
	//Initialize
	int width, height = 0;
	SDL_GetWindowSize(pWindow, &width, &height);
	m_Width = static_cast<uint32_t>(width);
	m_Height = static_cast<uint32_t>(height);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;
}

void Elite::Renderer::Render()
{
	SDL_LockSurface(m_pBackBuffer);

	//Loop over all the pixels
	for (uint32_t r = 0; r < m_Height; ++r)
	{
		float ySS{ GetScreenSpaceY(r) };

		for (uint32_t c = 0; c < m_Width; ++c)
		{
			float xSS{ GetScreenSpaceX(c) };

			RGBColor uColor{ SceneManager::RenderScene(xSS,ySS) };

			//draw the pixel
			m_pBackBufferPixels[c + (r * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
				static_cast<uint8_t>(uColor.r * 255.f),
				static_cast<uint8_t>(uColor.g * 255.f),
				static_cast<uint8_t>(uColor.b * 255.f));
		}
	}

	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Elite::Renderer::SaveBackbufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "BackbufferRender.bmp");
}

float Elite::Renderer::GetScreenSpaceX(Uint32 x) const
{
	float aspectRatio{ float(m_Width) / m_Height };

	return { (2.f * ((x + 0.5f) / m_Width) - 1) * aspectRatio * SceneManager::GetCameraFov() };
}

float Elite::Renderer::GetScreenSpaceY(Uint32 y) const
{
	return float{ (1 - (2 * ((y + 0.5f) / m_Height))) * SceneManager::GetCameraFov() };
}






//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "ERenderer.h"
#include "ERGBColor.h"
#include "EMath.h"
//#include "Texture.h"

Elite::Renderer::Renderer(SDL_Window* pWindow)
	: m_RenderFinalColor{ true }
	, m_DirectionalLight{ {0.577f,-0.577f,-0.577f},2.f }
{
	//Initialize
	m_pWindow = pWindow;
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	int width, height = 0;
	SDL_GetWindowSize(pWindow, &width, &height);
	m_Width = static_cast<uint32_t>(width);
	m_Height = static_cast<uint32_t>(height);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;

	m_pDepthBuffer = new float* [m_Height];

	for (size_t i = 0; i < m_Height; i++)
	{
		m_pDepthBuffer[i] = new float[m_Width] {FLT_MAX};
	}
}

Elite::Renderer::~Renderer()
{
	for (size_t h = 0; h < m_Height; h++)
	{
		delete m_pDepthBuffer[h];
	}
	delete m_pDepthBuffer;
}

void Elite::Renderer::RenderTriangle(const TriangleOutput& triangle, const TextureManager* pTextures)
{
	TriangleOutput triangleSS{ std::move(triangle) }; //move is available caus the triangle will go out of scope after this function is done

	triangleSS.v0.point = GetScreenSpace(triangleSS.v0.point);
	triangleSS.v1.point = GetScreenSpace(triangleSS.v1.point);
	triangleSS.v2.point = GetScreenSpace(triangleSS.v2.point);

	FPoint2 boundingBoxX{ CalculateBoundingBoxX(triangleSS) };
	FPoint2 boundingBoxY{ CalculateBoundingBoxY(triangleSS) };

	//Loop over all the pixels
	for (uint32_t r = uint32_t(boundingBoxY.x); r < uint32_t(boundingBoxY.y); ++r)
	{
		for (uint32_t c = uint32_t(boundingBoxX.x); c < uint32_t(boundingBoxX.y); ++c)
		{
			OutputVertex pixel{ {float(c),float(r),0.f ,0.f},{},{},{} ,{} };

			//check if the pixel is in the triangle
			if (triangleSS.Hit(pixel))
			{
				//calculate the interpolated values
				triangleSS.InterpolatePixelValues(pixel);

				//depth buffer test
				if (pixel.point.z > 0.f && pixel.point.z < 1.f && m_pDepthBuffer[r][c] > pixel.point.z)
				{
					m_pDepthBuffer[r][c] = pixel.point.z;

					if (m_RenderFinalColor)
					{
						//check if their is a valid normal map
						if (pTextures->normalMap.IsValid())
						{
							//calc tangent space axis
							const FVector3 binormal = Cross(pixel.tangent, pixel.normal);
							FMatrix3 tangentSpaceAxis = FMatrix3(pixel.tangent, binormal, pixel.normal);

							//sample from normal map
							RGBColor normalSample = pTextures->normalMap.Sample(pixel.uv);

							//[0,1] to range [-1,1]
							normalSample *= 2.f;
							normalSample.r -= 1.f;
							normalSample.g -= 1.f;
							normalSample.b -= 1.f;

							//place sampled normal in correct space
							pixel.normal = tangentSpaceAxis * FVector3{ normalSample.r,normalSample.g,normalSample.b };

							Normalize(pixel.normal);
						}
						//check if their is a valid texture
						if (pTextures->diffuseTexture.IsValid())
						{
							//sample from texture
							pixel.color = pTextures->diffuseTexture.Sample(pixel.uv);
						}
						else
						{
							pixel.color = { 0.4f,0.4f,0.4f }; //grey if the the object has no texture
						}
						//shade and color the pixel
						ColorPixel(c, r, PixelShading(pixel, pTextures));
					}
					else
					{
						float remappedValue = Elite::Remap(m_pDepthBuffer[r][c], 0.985f, 1.f);
						ColorPixel(c, r, RGBColor{ remappedValue,remappedValue,remappedValue });
					}
				}
			}
		}
	}
}


bool Elite::Renderer::SaveBackbufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "BackbufferRender.bmp");
}

void Elite::Renderer::StartRender()
{
	//reset depth buffer
	for (size_t h = 0; h < m_Height; h++)
	{
		for (size_t w = 0; w < m_Width; w++)
		{
			m_pDepthBuffer[h][w] = FLT_MAX;
		}
	}

	SDL_LockSurface(m_pBackBuffer);
}

void Elite::Renderer::EndRender()
{
	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
	ClearBackBuffer();
}

float Elite::Renderer::GetAspectRatio() const
{
	return float(m_Width) / float(m_Height);
}

void Elite::Renderer::SwitchRenderState()
{
	m_RenderFinalColor = !m_RenderFinalColor;
}

const FPoint4 Elite::Renderer::GetScreenSpace(const FPoint4& point) const
{
	const float x = (point.x + 1) / 2.f * m_Width;
	const float y = (1 - point.y) / 2.f * m_Height;
	return { x,y, point.z , point.w };
}

void Elite::Renderer::ColorPixel(uint32_t c, uint32_t r, const RGBColor& color)
{
	m_pBackBufferPixels[c + (r * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
		static_cast<uint8_t>(color.r * 255.f),
		static_cast<uint8_t>(color.g * 255.f),
		static_cast<uint8_t>(color.b * 255.f));
}

void Elite::Renderer::ClearBackBuffer()
{
	//Loop over all the pixels
	for (uint32_t r = 0; r < m_Height; ++r)
	{
		for (uint32_t c = 0; c < m_Width; ++c)
		{
			ColorPixel(c, r, RGBColor{ 0.2f,0.2f,0.2f }); //grey tint for better visualization
		}
	}
}

const FPoint2 Elite::Renderer::CalculateBoundingBoxY(const TriangleOutput& triangle) const
{
	float  smallestY, highestY;

	smallestY = std::min(triangle.v0.point.y, triangle.v1.point.y);
	smallestY = std::min(triangle.v2.point.y, smallestY);

	highestY = std::max(triangle.v0.point.y, triangle.v1.point.y);
	highestY = std::max(triangle.v2.point.y, highestY);

	//make the box a little larget to avoid floating point errors or truncation in conversion errors
	++highestY;

	//clamp within boundaries 
	Elite::Clamp(smallestY, 0.f, float(m_Height));
	Elite::Clamp(highestY, 0.f, float(m_Height));

	return FPoint2{ smallestY,highestY };
}

const FPoint2 Elite::Renderer::CalculateBoundingBoxX(const TriangleOutput& triangle) const
{
	float smallestX, highestX;

	smallestX = std::min(triangle.v0.point.x, triangle.v1.point.x);
	smallestX = std::min(triangle.v2.point.x, smallestX);

	highestX = std::max(triangle.v0.point.x, triangle.v1.point.x);
	highestX = std::max(triangle.v2.point.x, highestX);

	//make the box a little larget to avoid floating point errors or truncation in conversion errors
	++highestX;

	//clamp within boundaries 
	Elite::Clamp(smallestX, 0.f, float(m_Height));
	Elite::Clamp(highestX, 0.f, float(m_Height));

	return FPoint2{ smallestX,highestX };
}

const RGBColor Elite::Renderer::PixelShading(const OutputVertex& vertex, const TextureManager* pTextures)
{
	//standard black
	RGBColor finalColor{ 0.f,0.f,0.f };

	float observedArea{ Dot(-vertex.normal, m_DirectionalLight.lightDirection) };

	//shade the color
	if (observedArea > 0.f)
	{
		finalColor = m_DirectionalLight.lightColor * m_DirectionalLight.intensity * vertex.color * observedArea; //diffuse lambert color

		//adding the specular phong
		if (pTextures->usePhong && pTextures->glossMap.IsValid() && pTextures->specularMap.IsValid())
		{
			Elite::FVector3 reflect{ Elite::GetNormalized(-m_DirectionalLight.lightDirection + 2 * (Elite::Dot(vertex.normal,m_DirectionalLight.lightDirection) * vertex.normal)) };

			const float cosine{ Elite::Dot(reflect,vertex.vieuwDirection) };

			if (cosine > 0)
			{
				const RGBColor phongSpecular{ pTextures->specularMap.Sample(vertex.uv) * powf(cosine, pTextures->glossMap.Sample(vertex.uv).r * pTextures->shininess) };

				finalColor += phongSpecular;
			}
		}
		finalColor.MaxToOne();
	}
	return finalColor;
}



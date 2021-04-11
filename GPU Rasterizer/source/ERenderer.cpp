#include "pch.h"
//Project includes
#include "ERenderer.h"
#include "Mesh.h"
#include "Camera.h"

Elite::Renderer::Renderer(SDL_Window* pWindow)
	: m_pWindow{ pWindow }
	, m_Width{}
	, m_Height{}
	, m_IsInitialized{ false }
	, m_RenderFinalColor{ true }
	, m_RenderDirectX{ true }
	, m_RenderTransparency{ true }
	, m_DirectionalLight{ {0.577f,-0.577f,-0.577f},2.f }
{
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

	//direct x initializiation
	HRESULT result{ InitializeDirectX() };

	if (result == S_OK)
	{
		m_IsInitialized = true;
		std::cout << "DirectX is ready\n";
	}
	//handle error output
	//else {}
}

Elite::Renderer::~Renderer()
{
	if (m_pRenderTargetVieuw)
	{
		m_pRenderTargetVieuw->Release();
	}
	if (m_pRenderTargetBuffer)
	{
		m_pRenderTargetBuffer->Release();
	}
	if (m_pDethStencilVieuw)
	{
		m_pDethStencilVieuw->Release();
	}
	if (m_pDepthStencilBuffer)
	{
		m_pDepthStencilBuffer->Release();
	}

	if (m_pSwapChain)
	{
		m_pSwapChain->Release();
	}
	if (m_pDXGIFactory)
	{
		m_pDXGIFactory->Release();
	}

	if (m_pDeviceContext)
	{
		m_pDeviceContext->ClearState();
		m_pDeviceContext->Flush();
		m_pDeviceContext->Release();
	}

	if (m_pDevice)
	{
		m_pDevice->Release();
	}

	for (size_t h = 0; h < m_Height; h++)
	{
		delete m_pDepthBuffer[h];
	}
	delete m_pDepthBuffer;
}

void Elite::Renderer::Render(const std::vector<Mesh*>& pMeshes, const Camera* pCamera)
{
	if (m_RenderDirectX)
	{
		RenderDirectX(pMeshes, pCamera);
	}
	else
	{
		RenderOnCpu(pMeshes, pCamera);
	}
}

ID3D11Device* Elite::Renderer::GetDevice() const
{
	return m_pDevice;
}

void Elite::Renderer::SwitchRenderState()
{
	m_RenderFinalColor = !m_RenderFinalColor;
}

void Elite::Renderer::ToggleDirectX()
{
	m_RenderDirectX = !m_RenderDirectX;
}

void Elite::Renderer::ToggleTransparency()
{
	m_RenderTransparency = !m_RenderTransparency;
}


HRESULT Elite::Renderer::InitializeDirectX()
{
	//create Device and Device context, using hardware acceleration
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	uint32_t createDeviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT result = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, 0, 0, D3D11_SDK_VERSION, &m_pDevice, &featureLevel, &m_pDeviceContext);
	if (FAILED(result)) return result;

	//Create DXGI Factory to  create SwapChain based on hardware
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&m_pDXGIFactory));
	if (FAILED(result)) return result;

	//create swapchain discriptor
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferDesc.Width = m_Width;
	swapChainDesc.BufferDesc.Height = m_Height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	//Get the handle HWND from the sdl backbuffer
	SDL_SysWMinfo sysWMInfo{};
	SDL_VERSION(&sysWMInfo.version);
	SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
	swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

	//Create Swapchain and hook it into the handle of the sdl_window
	result = m_pDXGIFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
	if (FAILED(result)) return result;

	//Create the Depth/Stencil Buffer and vieuw
	D3D11_TEXTURE2D_DESC depthStencilDesc{};
	depthStencilDesc.Width = m_Width;
	depthStencilDesc.Height = m_Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	//create the resource vieuw
	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilVieuwDesc{};
	DepthStencilVieuwDesc.Format = depthStencilDesc.Format;
	DepthStencilVieuwDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilVieuwDesc.Texture2D.MipSlice = 0;

	//Create the resource
	result = m_pDevice->CreateTexture2D(&depthStencilDesc, 0, &m_pDepthStencilBuffer);
	if (FAILED(result)) return result;

	result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &DepthStencilVieuwDesc, &m_pDethStencilVieuw);
	if (FAILED(result)) return result;

	//create the RenderTargetVieuw
	result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
	if (FAILED(result)) return result;

	result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, 0, &m_pRenderTargetVieuw);
	if (FAILED(result)) return result;

	//Bind the vieuws to the output merger stage
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetVieuw, m_pDethStencilVieuw);

	//set the vieuwport
	D3D11_VIEWPORT vieuwPort{};
	vieuwPort.Width = static_cast<float>(m_Width);
	vieuwPort.Height = static_cast<float>(m_Height);
	vieuwPort.TopLeftX = 0.f;
	vieuwPort.TopLeftY = 0.f;
	vieuwPort.MinDepth = 0.f;
	vieuwPort.MaxDepth = 1.f;
	m_pDeviceContext->RSSetViewports(1, &vieuwPort);

	return result;
}

void Elite::Renderer::ClearBuffers()
{
	RGBColor clearColor = RGBColor(0.f, 0.f, 0.3f);
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetVieuw, &clearColor.r);
	m_pDeviceContext->ClearDepthStencilView(m_pDethStencilVieuw, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}

void Elite::Renderer::StartRenderer()
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

void Elite::Renderer::EndRenderer()
{
	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);

	//Clear Back buffer

		//Loop over all the pixels
	for (uint32_t r = 0; r < m_Height; ++r)
	{
		for (uint32_t c = 0; c < m_Width; ++c)
		{
			ColorPixel(c, r, RGBColor{ 0.2f,0.2f,0.2f }); //grey tint for better visualization
		}
	}
}



void Elite::Renderer::ColorPixel(uint32_t c, uint32_t r, const RGBColor& color)
{
	m_pBackBufferPixels[c + (r * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
		static_cast<uint8_t>(color.r * 255.f),
		static_cast<uint8_t>(color.g * 255.f),
		static_cast<uint8_t>(color.b * 255.f));
}

const FPoint4 Elite::Renderer::GetScreenSpace(const FPoint4& point) const
{
	const float x = (point.x + 1) / 2.f * m_Width;
	const float y = (1 - point.y) / 2.f * m_Height;
	return { x,y, point.z , point.w };
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

const RGBColor Elite::Renderer::PixelShading(const OutputVertex& vertex, const TextureManager& textures)
{
	//standard black
	RGBColor finalColor{ 0.f,0.f,0.f };

	float observedArea{ Dot(-vertex.normal, m_DirectionalLight.lightDirection) };

	//shade the color
	if (observedArea > 0.f)
	{
		finalColor = m_DirectionalLight.lightColor * m_DirectionalLight.intensity * vertex.color * observedArea; //diffuse lambert color

		//adding the specular phong
		if (textures.glossMap.IsValid() && textures.specularMap.IsValid())
		{
			Elite::FVector3 reflect{ Elite::GetNormalized(-m_DirectionalLight.lightDirection + 2 * (Elite::Dot(vertex.normal,m_DirectionalLight.lightDirection) * vertex.normal)) };

			const float cosine{ Elite::Dot(reflect,vertex.vieuwDirection) };

			if (cosine > 0)
			{
				const RGBColor phongSpecular{ textures.specularMap.Sample(vertex.uv) * powf(cosine, textures.glossMap.Sample(vertex.uv).r * textures.shininess) };

				finalColor += phongSpecular;
			}
		}
		finalColor.MaxToOne();
	}
	return finalColor;
}

void Elite::Renderer::RenderDirectX(const std::vector<Mesh*>& pMeshes, const Camera* pCamera)
{
	if (!m_IsInitialized)
		return;

	ClearBuffers();

	//render
	for (Mesh* pMesh : pMeshes)
	{
		if (m_RenderTransparency || !pMesh->UsesTransparency())
		{
			pMesh->RenderInDirectX(m_pDeviceContext, pCamera->GetVieuwMatrix());
		}
	}

	//present the content of the backbuffer to the screen
	m_pSwapChain->Present(0, 0);
}

void Elite::Renderer::RenderOnCpu(const std::vector<Mesh*>& pMeshes, const Camera* pCamera)
{
	StartRenderer();

	//render
	for (Mesh* pMesh : pMeshes)
	{
		if (pMesh->UsesTransparency()) continue;

		for (const TriangleOutput& triangle : pMesh->GetTriangles(pCamera))
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
								if (pMesh->GetTextureManager().normalMap.IsValid())
								{
									//calc tangent space axis
									const FVector3 binormal = Cross(pixel.tangent, pixel.normal);
									FMatrix3 tangentSpaceAxis = FMatrix3(pixel.tangent, binormal, pixel.normal);

									//sample from normal map
									RGBColor normalSample = pMesh->GetTextureManager().normalMap.Sample(pixel.uv);

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
								if (pMesh->GetTextureManager().diffuseTexture.IsValid())
								{
									//sample from texture
									pixel.color = (pMesh->GetTextureManager().diffuseTexture.Sample(pixel.uv));
								}
								else
								{
									pixel.color = { 0.4f,0.4f,0.4f }; //grey if the the object has no texture
								}
								//shade and color the pixel
								ColorPixel(c, r, PixelShading(pixel, pMesh->GetTextureManager()));
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
	}
	EndRenderer();
}





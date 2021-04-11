#include "pch.h"
//#undef main

//Standard includes
#include <iostream>
#include "vld.h"

//Project includes
#include "ETimer.h"
#include "ERenderer.h"
#include "Mesh.h"
#include "Camera.h"
#include "ObjReader.h"
#include "PhongEffect.h"

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//give info
	std::cout << "Welcome to the merged rasterizer made by Robin De Paepe. \n";
	std::cout << "R: Swap between DirectX and software rasterizer output.  \n";
	std::cout << "F: Swap between the texture sampling states.  \n";
	std::cout << "T: Toggle the transparency on/off.  \n \n \n";

	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;
	SDL_Window* pWindow = SDL_CreateWindow(
		"Merged Version - **Robin De Paepe**",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	auto pTimer{ std::make_unique<Elite::Timer>() };
	auto pRenderer{ std::make_unique<Elite::Renderer>(pWindow) };

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;


	//Camera
	Camera* pCamera = new Camera{ {0.f,5.f,60.f}, 60.f, float(width) / height };

	//Meshes
	std::vector<Mesh*> pMeshes;

	ObjReader reader;

	//create and add the ship
	PhongEffect* pShipEffect = new PhongEffect{ pRenderer->GetDevice() ,pCamera->GetProjectionMatrixLeftHanded() };
	Mesh* pShip = new Mesh{ pRenderer->GetDevice(),"Resources/vehicle_diffuse.png", "Resources/vehicle_normal.png", "Resources/vehicle_specular.png", "Resources/vehicle_gloss.png",pShipEffect,false,25.f };

	reader.ReadFile("Resources/vehicle.obj", *pShip);

	pShip->CalculateAndInitializeVertexTangent();
	pShip->InvertZComponentsVertices();
	pShip->CreateBuffersForGpu();
	pShip->InvertZComponentsVertices();

	pMeshes.push_back(pShip);

	//create and add the rocket boost

	Effect* pRocketEffect = new Effect{ pRenderer->GetDevice(),pCamera->GetProjectionMatrixLeftHanded() };
	Mesh* pRocket = new Mesh{ pRenderer->GetDevice(),"Resources/fireFX_diffuse.png", "", "", "",pRocketEffect,true };

	reader.ReadFile("Resources/fireFX.obj", *pRocket);

	pRocket->InvertZComponentsVertices();
	pRocket->CreateBuffersForGpu();

	pMeshes.push_back(pRocket);

	//Event loop
	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYDOWN:
				if (e.key.keysym.scancode == SDL_SCANCODE_W)
					pCamera->SetGoingForward(true);

				if (e.key.keysym.scancode == SDL_SCANCODE_S)
					pCamera->SetGoingBackward(true);

				if (e.key.keysym.scancode == SDL_SCANCODE_D)
					pCamera->SetGoingUp(true);

				if (e.key.keysym.scancode == SDL_SCANCODE_A)
					pCamera->SetGoingDown(true);
				break;
			case SDL_KEYUP:
				if (e.key.keysym.scancode == SDL_SCANCODE_F)
				{
					for (Mesh* pMesh : pMeshes)
					{
						pMesh->SwitchTextureFilter();
					}
				}

				if (e.key.keysym.scancode == SDL_SCANCODE_R)
				{
					pRenderer->ToggleDirectX();
					pCamera->ToggleDirectX();
				}

				if (e.key.keysym.scancode == SDL_SCANCODE_T)
				{
					pRenderer->ToggleTransparency();
				}

				if (e.key.keysym.scancode == SDL_SCANCODE_W)
					pCamera->SetGoingForward(false);

				if (e.key.keysym.scancode == SDL_SCANCODE_S)
					pCamera->SetGoingBackward(false);

				if (e.key.keysym.scancode == SDL_SCANCODE_D)
					pCamera->SetGoingUp(false);

				if (e.key.keysym.scancode == SDL_SCANCODE_A)
					pCamera->SetGoingDown(false);
				break;
			}
		}
		//--------- Update ---------
		pCamera->Update(pTimer->GetElapsed());

		for (Mesh* pMesh : pMeshes)
		{
			pMesh->Update(pTimer->GetElapsed());
		}

		//--------- Render ---------
		pRenderer->Render(pMeshes, pCamera);

		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "FPS: " << pTimer->GetFPS() << std::endl;
		}
	}
	pTimer->Stop();

	//Shutdown "framework"
	ShutDown(pWindow);

	//deletions
	delete pCamera;

	for (Mesh* pMesh : pMeshes)
	{
		delete pMesh;
	}
	return 0;
}

//External includes
#include "vld.h"
#include "SDL.h"
#include "SDL_surface.h"
#undef main

//Standard includes
#include <iostream>

//Project includes
#include "ETimer.h"
#include "ERenderer.h"
#include "SceneManager.h"
#include "ObjReader.h"
#include <chrono>

class Mesh;
class Scene;
void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

void InitializeScene1(std::shared_ptr<Renderer> spRenderer)
{
	ObjReader reader;

	Scene* pScene1{ new Scene{ FPoint3{0.0f,5.f,65.f}, 60.f , spRenderer } };
	Mesh* pMesh{ new Mesh{ "Resources/vehicle_diffuse.png","Resources/vehicle_normal.png","Resources/vehicle_specular.png","Resources/vehicle_gloss.png", Mesh::PrimitiveTopology::triangleList, true,25.f} };

	reader.ReadFile("Resources/vehicle.obj", *pMesh);

	pScene1->AddMesh(pMesh);

	SceneManager::AddScene(pScene1);
}

int main()
{
	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;
	SDL_Window* pWindow = SDL_CreateWindow(
		"Rasterizer - **Robin De Paepe**",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	Elite::Timer* pTimer = new Elite::Timer();
	std::shared_ptr<Renderer> spRenderer{ std::make_shared<Renderer>(pWindow) };
	SceneManager* pSceneManager = new SceneManager{};

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	bool takeScreenshot = false;

	//initialize scenes
	InitializeScene1(spRenderer);

	//give some info
	std::cout << "Press R to toggle depthbuffer. \n";
	std::cout << "Press X  for a screenshot. \n";

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
			case SDL_KEYUP:
				if (e.key.keysym.scancode == SDL_SCANCODE_X)
					takeScreenshot = true;
				if (e.key.keysym.scancode == SDL_SCANCODE_R)
					spRenderer->SwitchRenderState();
				if (e.key.keysym.scancode == SDL_SCANCODE_S)
					pSceneManager->SwitchToNextScene();
				break;
			}
		}

		//--------- Render ---------
		SceneManager::RenderScene();
		//--------- Timer ---------
		pTimer->Update();
		SceneManager::Update(pTimer->GetElapsed());
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "FPS: " << pTimer->GetFPS() << std::endl;
		}

		//Save screenshot after full render
		if (takeScreenshot)
		{
			if (!spRenderer->SaveBackbufferToImage())
				std::cout << "Screenshot saved!" << std::endl;
			else
				std::cout << "Something went wrong. Screenshot not saved!" << std::endl;
			takeScreenshot = false;
		}
	}
	pTimer->Stop();

	//Shutdown "framework"
	delete pTimer;
	delete pSceneManager;

	ShutDown(pWindow);
	return 0;
}


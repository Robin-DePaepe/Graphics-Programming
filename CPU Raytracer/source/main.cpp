#include "pch.h"
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
#include "Scene.h"
#include "LightManager.h"
#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"
//object includes
#include "Structs.h"
#include "Mat_Lambert.h"
#include "Mat_LambertPhong.h"
#include "Mat_PBR.h"
#include "ObjReader.h"


void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

void InitializeLights()
{
	LightManager::PushLight(new PointLight{ 40, RGBColor{1.f,1.f,1.f} ,FPoint3{0.f,2.f,-5.f} });
	LightManager::PushLight(new PointLight{ 25 , RGBColor{1.f,1.f,1.f} ,FPoint3{0.f,3.5f,3.f} });
	LightManager::PushLight(new DirectionalLight{ 1.f, RGBColor{1.f,1.f,1.f} ,FVector3{1.f,0.9f,1.f} });
}
void InitializeScene1()
{
	Scene* pScene1 = new Scene{ FPoint3{0.f,3.5f,10.f} };

	pScene1->AddObject(new Sphere{ FPoint3{-3.f,1.f,0.f},1.f,new Mat_PBR{1.f ,RGBColor{0.972f,0.930f,0.915f}, false } });  //silver non metal
	pScene1->AddObject(new Sphere{ FPoint3{0.f,1.f,0.f},1.f ,new Mat_PBR{0.6f ,RGBColor{0.972f,0.930f,0.915f}, false  } });
	pScene1->AddObject(new Sphere{ FPoint3{3.f,1.f,0.f},1.f ,new Mat_PBR{0.1f ,RGBColor{0.972f,0.930f,0.915f}, false } });

	pScene1->AddObject(new Sphere{ FPoint3{-3.f,3.5f,0.f},1.f,new Mat_PBR{1.f ,RGBColor{0.972f,0.930f,0.915f}, true } });  //silver metal
	pScene1->AddObject(new Sphere{ FPoint3{0.f,3.5f,0.f},1.f ,new Mat_PBR{0.6f ,RGBColor{0.972f,0.930f,0.915f}, true  } });
	pScene1->AddObject(new Sphere{ FPoint3{3.f,3.5f,0.f},1.f ,new Mat_PBR{0.1f ,RGBColor{0.972f,0.930f,0.915f}, true } });

	pScene1->AddObject(new Plane{ FPoint3{0.f,0.f,0.f}, FVector3{0.f,1.f,0.f}, new Mat_Lambert{1.f,RGBColor{0.392f, 0.584f, 0.929f}} }); //corn flower blue (the best blue)
	pScene1->AddObject(new Plane{ FPoint3{0.f,0.f,-10.f}, FVector3{0.f,0.f,1.f}, new Mat_Lambert{1.f,RGBColor{0.392f, 0.584f, 0.929f}} });
	pScene1->AddObject(new Plane{ FPoint3{-7.f,0.f,0.f}, FVector3{1.f,0.f,0.f}, new Mat_Lambert{1.f,RGBColor{0.392f, 0.584f, 0.929f}} });
	pScene1->AddObject(new Plane{ FPoint3{ 7.f,0.f,0.f}, FVector3{1.f,0.f,0.f}, new Mat_Lambert{1.f,RGBColor{0.392f, 0.584f, 0.929f}} });
	
	pScene1->AddObject(new Triangle{ FPoint3{-0.75f,1.5f,0.f},FPoint3{-0.75f,0.f,0.f},FPoint3{0.75f,0.f,0.f},FPoint3{3.f,5.f,0.f},new Mat_Lambert{1.f ,RGBColor{1.f,0.782f,0.344f}},false, Triangle::CullModes::backFaceCulling });  //gold
	pScene1->AddObject(new Triangle{ FPoint3{-0.75f,1.5f,0.f},FPoint3{-0.75f,0.f,0.f},FPoint3{0.75f,0.f,0.f},FPoint3{0.f,5.f,0.f},new Mat_Lambert{1.f ,RGBColor{1.f,0.782f,0.344f}},false });  //gold
	pScene1->AddObject(new Triangle{ FPoint3{-0.75f,1.5f,0.f},FPoint3{-0.75f,0.f,0.f},FPoint3{0.75f,0.f,0.f},FPoint3{-3.f,5.f,0.f},new Mat_Lambert{1.f ,RGBColor{1.f,0.782f,0.344f}},false,Triangle::CullModes::frontFaceCulling });  //gold

	SceneManager::AddScene(pScene1);
}
void InitializeScene2()
{
	Scene* pScene2 = new Scene{ FPoint3{0.f,3.f,10.f} };

	pScene2->AddObject(new Sphere{ FPoint3{-3.f,1.f,0.f},1.f,new Mat_PBR{1.f ,RGBColor{1.f,0.782f,0.344f}, false } });  //gold non metal
	pScene2->AddObject(new Sphere{ FPoint3{0.f,1.f,0.f},1.f ,new Mat_PBR{0.6f ,RGBColor{1.f,0.782f,0.344f}, false  } });
	pScene2->AddObject(new Sphere{ FPoint3{3.f,1.f,0.f},1.f ,new Mat_PBR{0.1f ,RGBColor{1.f,0.782f,0.344f}, false } });

	pScene2->AddObject(new Sphere{ FPoint3{-3.f,3.5f,0.f},1.f,new Mat_PBR{1.f ,RGBColor{1.f,0.782f,0.344f}, true } });  //gold metal
	pScene2->AddObject(new Sphere{ FPoint3{0.f,3.5f,0.f},1.f ,new Mat_PBR{0.6f ,RGBColor{1.f,0.782f,0.344f}, true  } });
	pScene2->AddObject(new Sphere{ FPoint3{3.f,3.5f,0.f},1.f ,new Mat_PBR{0.1f ,RGBColor{1.f,0.782f,0.344f}, true } });

	pScene2->AddObject(new Plane{ FPoint3{0.f,0.f,0.f}, FVector3{0.f,1.f,0.f}, new Mat_Lambert{1.f,RGBColor{0.392f, 0.584f, 0.929f}} }); //corn flower blue 
	pScene2->AddObject(new Plane{ FPoint3{0.f,0.f,-10.f}, FVector3{0.f,0.f,1.f}, new Mat_Lambert{1.f,RGBColor{0.392f, 0.584f, 0.929f}} });
	pScene2->AddObject(new Plane{ FPoint3{-7.f,0.f,0.f}, FVector3{1.f,0.f,0.f}, new Mat_Lambert{1.f,RGBColor{0.392f, 0.584f, 0.929f}} });
	pScene2->AddObject(new Plane{ FPoint3{ 7.f,0.f,0.f}, FVector3{1.f,0.f,0.f}, new Mat_Lambert{1.f,RGBColor{0.392f, 0.584f, 0.929f}} });

	pScene2->AddObject(new Triangle{ FPoint3{-0.75f,1.5f,0.f},FPoint3{-0.75f,0.f,0.f},FPoint3{0.75f,0.f,0.f},FPoint3{3.f,5.f,0.f},new Mat_Lambert{1.f ,RGBColor{0.972f,0.930f,0.915f}},false, Triangle::CullModes::backFaceCulling });  //silver
	pScene2->AddObject(new Triangle{ FPoint3{-0.75f,1.5f,0.f},FPoint3{-0.75f,0.f,0.f},FPoint3{0.75f,0.f,0.f},FPoint3{0.f,5.f,0.f},new Mat_Lambert{1.f ,RGBColor{0.972f,0.930f,0.915f}},false });  //silver
	pScene2->AddObject(new Triangle{ FPoint3{-0.75f,1.5f,0.f},FPoint3{-0.75f,0.f,0.f},FPoint3{0.75f,0.f,0.f},FPoint3{-3.f,5.f,0.f},new Mat_Lambert{1.f ,RGBColor{0.972f,0.930f,0.915f}},false,Triangle::CullModes::frontFaceCulling });  //silver

	SceneManager::AddScene(pScene2);
}
void InitializeScene3()
{
	Scene* pScene3 = new Scene{ FPoint3{0.f,3.f,10.f} };

	ObjReader bunnyReader;

	ObjData bunnyData = bunnyReader.ReadFile("lowpoly_bunny.obj");

	//bunny
	pScene3->AddObject(new TriangleMesh{ new std::vector<FPoint3>{ bunnyData.m_Vertexes}, new std::vector<IPoint3>{ bunnyData.m_Faces},new Mat_PBR{0.8f ,RGBColor{1.f,0.782f,0.344f}, true  }, FPoint3{0.f,0.f,1.f} });

	//planes
	pScene3->AddObject(new Plane{ FPoint3{0.f,0.f,0.f}, FVector3{0.f,1.f,0.f}, new Mat_Lambert{1.f,RGBColor{0.392f, 0.584f, 0.929f}} }); //corn flower blue 
	pScene3->AddObject(new Plane{ FPoint3{0.f,0.f,-10.f}, FVector3{0.f,0.f,1.f}, new Mat_Lambert{1.f,RGBColor{0.392f, 0.584f, 0.929f}} });
	pScene3->AddObject(new Plane{ FPoint3{-7.f,0.f,0.f}, FVector3{1.f,0.f,0.f}, new Mat_Lambert{1.f,RGBColor{0.392f, 0.584f, 0.929f}} });
	pScene3->AddObject(new Plane{ FPoint3{ 7.f,0.f,0.f}, FVector3{1.f,0.f,0.f}, new Mat_Lambert{1.f,RGBColor{0.392f, 0.584f, 0.929f}} });

	SceneManager::AddScene(pScene3);
}


int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;

	SDL_Window* pWindow = SDL_CreateWindow(
		"RayTracer - Robin De Paepe",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	Timer* pTimer = new Timer();
	Renderer* pRenderer = new Renderer(pWindow);
	SceneManager* pScenes = new SceneManager{};

	InitializeLights();
	InitializeScene1();
	InitializeScene2();
	InitializeScene3();

	std::cout << "Press 's' to switch scenes.\n";

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	bool takeScreenshot = false;

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
				break;
			}
		}

		//--------- Render ---------
		pRenderer->Render();

		////-----update--------
		SceneManager::CheckAndHandleInput();
		SceneManager::Update(pTimer->GetElapsed());

		//--------- Timer ---------
		pTimer->Update();

		printTimer += pTimer->GetElapsed();

		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "FPS: " << pTimer->GetFPS() << std::endl;
		}

		//Save screenshot after full render
		if (takeScreenshot)
		{
			if (!pRenderer->SaveBackbufferToImage())
				std::cout << "Screenshot saved!" << std::endl;
			else
				std::cout << "Something went wrong. Screenshot not saved!" << std::endl;
			takeScreenshot = false;
		}
	}
	pTimer->Stop();

	//Shutdown "framework"
	delete pRenderer;
	delete pTimer;
	delete pScenes;

	ShutDown(pWindow);
	return 0;
}

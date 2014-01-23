#include "Demo2D_pch.h"

#include "Application\Engine.h"
#include "2D\Renderer2D.h"
//#include "Deferred\DeferredRenderer.h"
//#include "ForwardPlus\ForwardPlusRenderer.h"
#include "Havok\Havok.h"
#include "NullPhysics.h"
#include "Messenger.h"
#include "Scene.h"
#include "DirectAudio.h"
#include "Resources\WAVE.h"
#include "..\..\ResourceManager\Source\ResourceCache.h"
#include "..\..\ResourceManager\Source\ResourceFile.h"
#include "..\..\ResourceManager\Source\ResourceLoader.h"
#include "Game\Demo2D.h"
#include "Game\DemoStateFactory.h"
#include "Game\States\BoxTestState.h"
#include "Resources\MeshResource.h"

using namespace Anubis;

Anubis::Engine* Anubis::g_pEngine = NULL;
Anubis::IAudio* Anubis::g_pAudio = NULL;

////////////////////////////////////
//Entry point for the application
////////////////////////////////////
int WINAPI wWinMain(	HINSTANCE hInstance, 
						HINSTANCE hPrevInstance,
						LPWSTR lpCmdLine,
						int	   nShowCmd)
{
	AINT32 g_iExit = 0;

	Anubis::g_pEngine = new Engine(); //Allocate memory for the engine

	/* ==== Initialize window === */
	g_pEngine->InitializeWindow(hInstance, nShowCmd, SCREEN_WIDTH, SCREEN_HEIGHT, true);

	/* =========================
		Initialize Game Logic
	   ========================= */
	Demo2D g_Game;

	//Manage states factory and set initial state
	g_Game.SetStateFactory(new DemoStateFactory());
	g_Game.SetNextState(GS_BOXTEST);

	/* === Initialize Game === */
	g_pEngine->InitializeGame(&g_Game);

	/* ==============================
		Initialize Engine Components
	   ============================== */

	//Allocate memory for engine subsystems!
	Anubis::g_pAudio	= new DirectSoundAudio();
	//Renderer*	g_pRenderer = new DeferredRenderer();
	Renderer2D* g_pRenderer = new Renderer2D();
	IPhysics*	g_pPhysics = CreateHavokPhysics();
	//IPhysics*	g_pPhysics = CreateNullPhysics();
	Messenger*	g_pMessenger = new Messenger();
	
	ResourceDir* g_pFile = new ResourceDir(L"Resources");
	//g_pFile->VOpen();

	ResourceCache*	g_pCache = new ResourceCache(300, g_pFile);
	g_pCache->Init();

	//add resource loaders
	IResourceLoader* pObjLoader = new ObjResourceLoader();
	IResourceLoader* pWaveLoader = new WaveResourceLoader();
	g_pCache->RegisterLoader(shared_ptr<IResourceLoader>(pObjLoader));
	g_pCache->RegisterLoader(shared_ptr<IResourceLoader>(pWaveLoader));

	/* === Initialize main engine components === */
	g_pEngine->InitializeComponents(g_pAudio, g_pRenderer, g_pPhysics, g_pMessenger, g_pCache);

	// **************************** //
	// ===						=== //
	// ===		Game Loop		=== //
	// ===						=== //
	// **************************** //
	g_iExit = g_pEngine->Run();

	//Deallocate all the memory and shut down application
	//NOTE!: Engine is responsible for destroying all subsystems in the correct order!!!
	delete g_pEngine;

	return g_iExit;
}
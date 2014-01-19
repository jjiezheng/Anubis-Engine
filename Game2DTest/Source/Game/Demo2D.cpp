#include "Demo2D_pch.h"
#include "Demo2D.h"

#include "../../../Math/Source/Geometry/Frustum.h"
#include "Controllers.h"

using namespace Anubis;

ABOOL Demo2D::VInitialize()
{
	/*** Initialize player view ***/
	shared_ptr<PlayerView> pView = static_pointer_cast<PlayerView>(VAddGameView(make_shared<PlayerView>(PlayerView())));
	AddPlayerView(pView);

	//Add camera
	Frustum & frustum = Frustum();
	//frustum.Init(Pi / 2.0, 1280.0 / 720.0, 0.1, 100.0);
	frustum.Init( 0.25f * 3.14f, (AREAL)SCREEN_WIDTH / (AREAL)SCREEN_HEIGHT, 1.0f, VIEW_DISTANCE);
	Viewport & viewport = Viewport();
	viewport.Init(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f, 500.0f);
	/*CameraPtr pCamera = make_shared<Camera>(Camera(frustum, Vector(0.0f, 0.0f, -70.0, 1.0f),
		Vector(0.0f, 0.0f, 1.0f, 0.0f),
		Vector(0.0f, 1.0f, 0.0f, 0.0f),
		0.0f, 0.0, 0.0, viewport)); */
	CameraPtr pCamera = make_shared<Camera>(Camera(frustum, Vector(0.0f, 0.0f, 0.0, 1.0f),
		Vector(0.0f, 0.0f, 1.0f, 0.0f),
		Vector(0.0f, 1.0f, 0.0f, 0.0f),
		0.0f, 0.0, 0.0, viewport));

	pView->VSetCamera(pCamera);

	//Input controllers
	//FreeCameraController* m_pFreeCameraController = new FreeCameraController();
	//PlayerWASDController* m_pPlayerController = new PlayerWASDController();
	shared_ptr<FreeCameraController> m_pFreeCameraController = make_shared<FreeCameraController>(FreeCameraController());
	shared_ptr<PlayerWASDController> m_pPlayerController = make_shared<PlayerWASDController>(PlayerWASDController());

	//pView->SetMouseHandler(m_pFreeCameraController);
	//pView->SetKeyboardHandler(m_pFreeCameraController);
	//pView->SetController(m_pFreeCameraController);

	//pView->SetMouseHandler(m_pPlayerController);
	//pView->SetKeyboardHandler(m_pPlayerController);
	//pView->SetController(m_pPlayerController);

	//call base class initialization
	return Game::VInitialize();
}
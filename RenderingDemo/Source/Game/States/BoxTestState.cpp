#include "RenderingDemo_pch.h"
#include "BoxTestState.h"

#include "Application\Engine.h"
#include "Entities\EntityResource.h"

using namespace Anubis;

//Initialize game state
AVOID BoxTestState::VInitialize( Game * pGame, AUINTPTR uptrData)
{
	m_pScene->VInitialize();

	EntityResource & box_resource = EntityResource("BoxEntity.xml");
	EntityPtr pBox = pGame->VAddEntity(box_resource.VCreateEntity(pGame));
	box_resource.VCreateRepresentation(m_pScene, pBox);
	box_resource.VCreatePhysicalBody(g_pEngine->GetPhysics(), pBox);

	/*EntityResource & box_resource1 = EntityResource("BoxEntityPlane.xml");
	EntityPtr pBox1 = pGame->VAddEntity(box_resource1.VCreateEntity(pGame));
	box_resource1.VCreateRepresentation(m_pScene, pBox1);
	box_resource1.VCreatePhysicalBody(g_pEngine->GetPhysics(), pBox1);

	EntityResource & box_resource2 = EntityResource("BoxEntityPlaneXY.xml");
	EntityPtr pBox2 = pGame->VAddEntity(box_resource2.VCreateEntity(pGame));
	box_resource2.VCreateRepresentation(m_pScene, pBox2);
	box_resource2.VCreatePhysicalBody(g_pEngine->GetPhysics(), pBox2);

	box_resource2.m_vPos = box_resource2.m_vPos - Vector(0, 0, 120, 0);
	EntityPtr pBox3 = pGame->VAddEntity(box_resource2.VCreateEntity(pGame));
	box_resource2.VCreateRepresentation(m_pScene, pBox3);
	box_resource2.VCreatePhysicalBody(g_pEngine->GetPhysics(), pBox3);

	EntityResource & box_resource3 = EntityResource("BoxEntityPlaneYZ.xml");
	EntityPtr pBox4 = pGame->VAddEntity(box_resource3.VCreateEntity(pGame));
	box_resource3.VCreateRepresentation(m_pScene, pBox4);
	box_resource3.VCreatePhysicalBody(g_pEngine->GetPhysics(), pBox4);

	box_resource3.m_vPos = box_resource3.m_vPos + Vector(120, 0, 0, 0);
	EntityPtr pBox5 = pGame->VAddEntity(box_resource3.VCreateEntity(pGame));
	box_resource3.VCreateRepresentation(m_pScene, pBox5);
	box_resource3.VCreatePhysicalBody(g_pEngine->GetPhysics(), pBox5); */

	/*EntityResource & box_resource_small = EntityResource("BoxEntitySmall.xml");
	EntityPtr pBoxSmall = pGame->VAddEntity(box_resource_small.VCreateEntity(pGame));
	box_resource_small.VCreateRepresentation(m_pScene, pBoxSmall);
	box_resource_small.VCreatePhysicalBody(g_pEngine->GetPhysics(), pBoxSmall); */

	EntityResource & box_resource_big = EntityResource("BoxEntityBig.xml");
	EntityPtr pBoxBig = pGame->VAddEntity(box_resource_big.VCreateEntity(pGame));
	box_resource_big.VCreateRepresentation(m_pScene, pBoxBig);
	box_resource_big.VCreatePhysicalBody(g_pEngine->GetPhysics(), pBoxBig);

	EntityResource & plane_resource = EntityResource("Plane.xml");
	EntityPtr pPlane = pGame->VAddEntity(plane_resource.VCreateEntity(pGame));
	plane_resource.VCreateRepresentation(m_pScene, pPlane);
	plane_resource.VCreatePhysicalBody(g_pEngine->GetPhysics(), pPlane);

	EntityResource & projector_resource = EntityResource("Light.xml");
	EntityPtr pProjector = pGame->VAddEntity(projector_resource.VCreateEntity(pGame));
	projector_resource.VCreateRepresentation(m_pScene, pProjector);
	projector_resource.VCreatePhysicalBody(g_pEngine->GetPhysics(), pProjector); 

	//projector_resource.m_vPos = projector_resource.m_vPos + Vector(0.0f, 0.0f, 4.0f, 0.0f);
	//EntityPtr pProjector1 = pGame->VAddEntity(projector_resource.VCreateEntity(pGame));
	//projector_resource.VCreateRepresentation(m_pScene, pProjector1);
	//projector_resource.VCreatePhysicalBody(g_pEngine->GetPhysics(), pProjector1); 

	EntityResource & sphere_resource = EntityResource("StaticSphere.xml");
	EntityPtr pSphere1 = pGame->VAddEntity(sphere_resource.VCreateEntity(pGame));
	sphere_resource.VCreateRepresentation(m_pScene, pSphere1);
	sphere_resource.VCreatePhysicalBody(g_pEngine->GetPhysics(), pSphere1); 

	sphere_resource.m_vPos = sphere_resource.m_vPos + Vector(4.0f, 0.0f, 0.0f, 0.0f);
	EntityPtr pSphere2 = pGame->VAddEntity(sphere_resource.VCreateEntity(pGame));
	sphere_resource.VCreateRepresentation(m_pScene, pSphere2);
	sphere_resource.VCreatePhysicalBody(g_pEngine->GetPhysics(), pSphere2); 

	sphere_resource.m_vPos = sphere_resource.m_vPos + Vector(4.0f, 0.0f, 0.0f, 0.0f);
	EntityPtr pSphere3 = pGame->VAddEntity(sphere_resource.VCreateEntity(pGame));
	sphere_resource.VCreateRepresentation(m_pScene, pSphere3);
	sphere_resource.VCreatePhysicalBody(g_pEngine->GetPhysics(), pSphere3); 

	/*projector_resource.m_vPos = projector_resource.m_vPos - Vector(0, 0, 20, 0);
	EntityPtr pProjector1 = pGame->VAddEntity(projector_resource.VCreateEntity(pGame));
	projector_resource.VCreateRepresentation(m_pScene, pProjector1);
	projector_resource.VCreatePhysicalBody(g_pEngine->GetPhysics(), pProjector1); 

	projector_resource.m_vPos = projector_resource.m_vPos - Vector(0, 0, 20, 0);
	EntityPtr pProjector2 = pGame->VAddEntity(projector_resource.VCreateEntity(pGame));
	projector_resource.VCreateRepresentation(m_pScene, pProjector2);
	projector_resource.VCreatePhysicalBody(g_pEngine->GetPhysics(), pProjector2); 

	projector_resource.m_vPos = projector_resource.m_vPos - Vector(0, 0, 20, 0);
	EntityPtr pProjector3 = pGame->VAddEntity(projector_resource.VCreateEntity(pGame));
	projector_resource.VCreateRepresentation(m_pScene, pProjector3);
	projector_resource.VCreatePhysicalBody(g_pEngine->GetPhysics(), pProjector3); 
	*/
	EntityResource & player_resource = EntityResource("Player.xml");
	EntityPtr pPlayer = pGame->VAddEntity(player_resource.VCreateEntity(pGame));
	player_resource.VCreateRepresentation(m_pScene, pPlayer);
	player_resource.VCreatePhysicalBody(g_pEngine->GetPhysics(), pPlayer); 

	pPlayer->SetController(pGame->GetPlayerView(0)->GetController());
	pGame->GetPlayerView(0)->VGetCamera()->SetTarget(&*pPlayer);

	//create resource
	//ObstacleEntityResource & obstacle_params = ObstacleEntityResource("H1.obj", Vector(0.0f, 0.0f, 0.0f, 1.0f));
	//ObstacleEntityResource & obstacle_params = ObstacleEntityResource("Chest.obj", Vector(0.0f, 0.0f, 0.0f, 1.0f));
	//EntityPtr pObstacle = pGame->VAddEntity(obstacle_params.VCreateEntity(pGame));
	//obstacle_params.VCreateRepresentation(m_pScene, pObstacle); 
	//obstacle_params.VCreatePhysicalBody(g_pEngine->GetPhysics(), pObstacle); 

	//add point light
	//PointLightEntityResource light_params = PointLightEntityResource();
	//EntityPtr pLight = pGame->VAddEntity(light_params.VCreateEntity(pGame));
	//light_params.VCreateRepresentation(m_pScene, pLight);
	
	//SpotLightEntityResource spotLight_params = SpotLightEntityResource(Vector(1.0f, 1.0f, 1.0f, 1.0f), Vector(-10.0f, 4.0f, -15.0f, 1.0f), Vector(0.4f, -0.5f, 0.8f, 0.0f),
	//	(AREAL)Pi / 8, (AREAL)Pi / 4, 200.0f);
	//SpotLightEntityResource spotLight_params = SpotLightEntityResource(Vector(1.0f, 1.0f, 1.0f, 1.0f), Vector(0.0f, 16.0f, 0.0f, 1.0f), Vector(0.0f, -1.0f, 0.0f, 0.0f),
	//	(AREAL)Pi / 8, (AREAL)Pi / 4, 30.0f);
	//m_pLight = pGame->VAddEntity(spotLight_params.VCreateEntity(pGame));
	//spotLight_params.VCreateRepresentation(m_pScene, m_pLight); 
}

//Destroy the state
AVOID BoxTestState::VRelease( Game * pGame ) 
{
	SAFE_DELETE(m_pScene);
}

//Update the state
AVOID BoxTestState::VUpdate( Game * pGame, AREAL64 r64Time, AREAL64 r64ElapsedTime )
{
	m_pScene->VUpdate(r64Time, r64ElapsedTime);
}

//Render State
AVOID BoxTestState::VRender( Game * pGame, Renderer * pRenderer, AREAL64 r64Time, AREAL64 r64ElapsedTime )
{
	m_pScene->VRender(pRenderer, r64Time);
}
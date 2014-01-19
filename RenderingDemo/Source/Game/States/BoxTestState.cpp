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


	EntityResource & sphere_resource = EntityResource("StaticSphere.xml");
	EntityPtr pSphere1 = pGame->VAddEntity(sphere_resource.VCreateEntity(pGame));
	sphere_resource.VCreateRepresentation(m_pScene, pSphere1);
	sphere_resource.VCreatePhysicalBody(g_pEngine->GetPhysics(), pSphere1);

	EntityResource & logo_resource = EntityResource("StaticObstacle.xml");
	EntityPtr pLogo = pGame->VAddEntity(logo_resource.VCreateEntity(pGame));
	logo_resource.VCreateRepresentation(m_pScene, pLogo);
	logo_resource.VCreatePhysicalBody(g_pEngine->GetPhysics(), pLogo); 

	EntityResource & statue_resource = EntityResource("StaticObstacleTwo.xml");
	EntityPtr pStatue = pGame->VAddEntity(statue_resource.VCreateEntity(pGame));
	statue_resource.VCreateRepresentation(m_pScene, pStatue);
	statue_resource.VCreatePhysicalBody(g_pEngine->GetPhysics(), pStatue); 
	

	/*projector_resource.m_vOffset = projector_resource.m_vOffset - Vector(0, 0, 20, 0);
	EntityPtr pProjector1 = pGame->VAddEntity(projector_resource.VCreateEntity(pGame));
	projector_resource.VCreateRepresentation(m_pScene, pProjector1);
	projector_resource.VCreatePhysicalBody(g_pEngine->GetPhysics(), pProjector1); 

	projector_resource.m_vOffset = projector_resource.m_vOffset - Vector(0, 0, 20, 0);
	EntityPtr pProjector2 = pGame->VAddEntity(projector_resource.VCreateEntity(pGame));
	projector_resource.VCreateRepresentation(m_pScene, pProjector2);
	projector_resource.VCreatePhysicalBody(g_pEngine->GetPhysics(), pProjector2); 

	projector_resource.m_vOffset = projector_resource.m_vOffset - Vector(0, 0, 20, 0);
	EntityPtr pProjector3 = pGame->VAddEntity(projector_resource.VCreateEntity(pGame));
	projector_resource.VCreateRepresentation(m_pScene, pProjector3);
	projector_resource.VCreatePhysicalBody(g_pEngine->GetPhysics(), pProjector3); 

	m_lights.push_back(pProjector);
	m_lights.push_back(pProjector1);
	m_lights.push_back(pProjector2);
	m_lights.push_back(pProjector3); */
	//m_lights.push_back(pProjectorex);
	for (int i =1; i < 256; i++)
		//for (int j =0; j < 9; j++)
	{
		int n = i / 16;
		int m = i % 16;
		//m_pLightGeometryData[i].posANDrang = Vector(-50 + n*8, -12, -45 + m*8, 10);

		projector_resource.m_vOffset = Vector(n*22, 0.0, -m*22, 0.0f);
		EntityPtr pProjectorNew = pGame->VAddEntity(projector_resource.VCreateEntity(pGame));
		projector_resource.VCreateRepresentation(m_pScene, pProjectorNew);
		projector_resource.VCreatePhysicalBody(g_pEngine->GetPhysics(), pProjectorNew); 

		m_lights.push_back(pProjectorNew);
	}
	
	EntityResource & player_resource = EntityResource("Player.xml");
	EntityPtr pPlayer = pGame->VAddEntity(player_resource.VCreateEntity(pGame));
	player_resource.VCreateRepresentation(m_pScene, pPlayer); 
	//player_resource.VCreatePhysicalBody(g_pEngine->GetPhysics(), pPlayer); 

	//pPlayer->SetController(pGame->GetPlayerView(0)->GetController());
	//pGame->GetPlayerView(0)->VGetCamera()->SetTarget(&*pPlayer);

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

	m_lightVelocity = Vector(0.0f, -0.05f, 0.0f, 1.0f);
}

//Destroy the state
AVOID BoxTestState::VRelease( Game * pGame ) 
{
	SAFE_DELETE(m_pScene);
}

//Update the state
AVOID BoxTestState::VUpdate( Game * pGame, AREAL64 r64Time, AREAL64 r64ElapsedTime )
{
	for (vector<EntityPtr>::iterator light = m_lights.begin(); light != m_lights.end(); light++)
	{
		Vec pos = Vector(0.1f, 0.1f, 0.1f, 1.0f);
		Vec curPos = light->get()->GetCurrentTransform().GetPosition();
		Mat4x4 trans;

		if (curPos.y > -6) 
		{
			m_lightVelocity.y = -m_lightVelocity.y;
			trans.CreateTranslation(m_lightVelocity + Vector(0.0f, -1.0f, 0.0f, 0.0f));
		}
		if (curPos.y < -14)
		{
			m_lightVelocity.y = -m_lightVelocity.y;
			trans.CreateTranslation(m_lightVelocity + Vector(0.0f, 1.0f, 0.0f, 0.0f));
		}
		else
		{
			trans.CreateTranslation(m_lightVelocity);
		}
		light->get()->SetCurrentTransform(light->get()->GetCurrentTransform() * trans, r64Time);	
	}

	m_pScene->VUpdate(r64Time, r64ElapsedTime);
}

//Render State
AVOID BoxTestState::VRender( Game * pGame, Renderer * pRenderer, AREAL64 r64Time, AREAL64 r64ElapsedTime )
{
	m_pScene->VRender(pRenderer, r64Time);
}
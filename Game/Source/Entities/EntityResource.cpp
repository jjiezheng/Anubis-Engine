//====================================================================================
//EntityResource.cpp
//
//This code is part of Anubis Engine.
//
//Anubis Engine is a free game engine created as a fan project to be
//awesome platform for developing games!
//
//All sources can be found here:
//	https://github.com/Dgek/Anubis-Engine
//
//Demos based on Anubis Engine can be found here:
//	https://github.com/Dgek/Demos
//
//Copyright (c) 2013, Muralev Evgeny
//All rights reserved.
//
//Redistribution and use in source and binary forms, with
//or without modification, are permitted provided that the
//following conditions are met:
//
//Redistributions of source code must retain the above copyright notice,
//this list of conditions and the following disclaimer.
//
//Redistributions in binary form must reproduce the above copyright notice,
//this list of conditions and the following disclaimer in the documentation
//and/or other materials provided with the distribution.
//
//Neither the name of the Minotower Games nor the names of its contributors 
//may be used to endorse or promote products derived from this software 
//without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY MURALEV EVGENY "AS IS"
//AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
//THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//ARE DISCLAIMED. IN NO EVENT SHALL MURALEV EVGENY BE LIABLE
//FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
//ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//====================================================================================

#include "Game_pch.h"
#include "EntityResource.h"

#include "Data\JSONLoader.h"
#include "Data\XML\tinystr.h"
#include "Data\XML\tinyxml.h"

#include "Application\Engine.h"

#include "Resources\MeshResource.h"
#include "Meshes\Meshes.h"
#include "Meshes\BoxMesh.h"
#include "Meshes\PlaneMesh.h"
#include "Meshes\Sprite.h"

using namespace Anubis;

/*AVOID EntityResource::VLoad(const ASTRING & filename)
{
	//parse file
	JSONLoader::Parse(filename);

	//read position data
	m_pos = Vector(JSONLoader::GetAREAL32("position_x"), JSONLoader::GetAREAL32("position_y"), JSONLoader::GetAREAL32("position_z"), 1.0f);
} */

EntityResource::EntityResource(const ACHAR * filename)
{
	//start loading the document
	TiXmlDocument doc(filename);
	doc.LoadFile();

	TiXmlElement* pActorRoot = doc.RootElement();

	//Go through each component current actor has
	TiXmlElement* pComponent = pActorRoot->FirstChildElement();
	while (pComponent)
	{
		ASTRING componentName = pComponent->Value();

		//Get transform data
		if (componentName == "Transform")			InitTransform(pComponent);
		if (componentName == "PhysicsComponent")	InitPhysicsComponent(pComponent);
		if (componentName == "GraphicsComponent")	InitGraphicsComponent(pComponent);

		pComponent = pComponent->NextSiblingElement();
	}

	m_vOffset = Vector(0.0f, 0.0f, 0.0f, 0.0f);
}

EntityResource::~EntityResource()
{
	while (!m_graphics.empty())
	{
		GraphicsComponent* pDeleteMe = m_graphics.back();
		m_graphics.pop_back();

		SAFE_DELETE(pDeleteMe);
	}

	while (!m_physics.empty())
	{
		PhysicsComponent* pDeleteMe = m_physics.back();
		m_physics.pop_back();

		SAFE_DELETE(pDeleteMe);
	}
}

AVOID EntityResource::InitGraphicsComponent(TiXmlElement* pComponent)
{
	TiXmlElement*pTempEl = pComponent->FirstChildElement();
	TiXmlAttribute* pTempAtt;

	while (pTempEl)
	{
		ASTRING componentName = pTempEl->Value();

		if (componentName == "BoxComponent")
		{
			GraphicsBoxComponent* box = new GraphicsBoxComponent();

			pTempAtt = pTempEl->FirstAttribute();
			box->m_fXHalfExtent = pTempAtt->DoubleValue();

			pTempAtt = pTempAtt->Next();
			box->m_fYHalfExtent = pTempAtt->DoubleValue();

			pTempAtt = pTempAtt->Next();
			box->m_fZHalfExtent = pTempAtt->DoubleValue();

			TiXmlElement* pLocal = pTempEl->FirstChildElement("Material");
			box->m_material = s2ws(pLocal->GetText());

			m_graphics.push_back(box);
		}
		else if(componentName == "PlaneComponent")
		{
			GraphicsPlaneComponent* plane = new GraphicsPlaneComponent();

			pTempAtt = pTempEl->FirstAttribute();
			plane->m_fXHalfExtent = pTempAtt->DoubleValue();

			pTempAtt = pTempAtt->Next();
			plane->m_fYHalfExtent = pTempAtt->DoubleValue();

			pTempAtt = pTempAtt->Next();
			plane->m_fZHalfExtent = pTempAtt->DoubleValue();

			TiXmlElement* pLocal = pTempEl->FirstChildElement("Material");
			plane->m_material = s2ws(pLocal->GetText());

			m_graphics.push_back(plane);
		}
		else if(componentName == "SpriteComponent")
		{
			GraphicsSpriteComponent* plane = new GraphicsSpriteComponent();

			pTempAtt = pTempEl->FirstAttribute();
			plane->m_fXHalfExtent = pTempAtt->DoubleValue();

			pTempAtt = pTempAtt->Next();
			plane->m_fYHalfExtent = pTempAtt->DoubleValue();

			TiXmlElement* pLocal = pTempEl->FirstChildElement("Material");
			plane->m_material = s2ws(pLocal->GetText());

			m_graphics.push_back(plane);
		}
		else if(componentName == "MeshComponent")
		{
			GraphicsMeshComponent* mesh = new GraphicsMeshComponent();

			pTempAtt = pTempEl->FirstAttribute();
			mesh->m_filename = pTempAtt->Value();

			m_graphics.push_back(mesh);
		}
		else if(componentName == "SpotLightComponent")
		{
			GraphicsSpotLightComponent* spotLight = new GraphicsSpotLightComponent();

			pTempAtt = pTempEl->FirstAttribute();
			spotLight->m_range = pTempAtt->DoubleValue();

			pTempAtt = pTempAtt->Next();
			spotLight->m_innerAngle = pTempAtt->DoubleValue();

			pTempAtt = pTempAtt->Next();
			spotLight->m_outerAngle = pTempAtt->DoubleValue();

			//Load color
			TiXmlElement* pLocal = pTempEl->FirstChildElement("Color");
			pTempAtt = pLocal->FirstAttribute();
			AREAL red = pTempAtt->DoubleValue();

			pTempAtt = pTempAtt->Next();
			AREAL green = pTempAtt->DoubleValue();

			pTempAtt = pTempAtt->Next();
			AREAL blue = pTempAtt->DoubleValue();

			spotLight->m_color = Vector(red, green, blue, 1.0f);

			//Load position
			pLocal = pTempEl->FirstChildElement("Position");
			if (pLocal)
			{
				pTempAtt = pLocal->FirstAttribute();
				AREAL x = pTempAtt->DoubleValue();
				
				pTempAtt = pTempAtt->Next();
				AREAL y = pTempAtt->DoubleValue();

				pTempAtt = pTempAtt->Next();
				AREAL z = pTempAtt->DoubleValue();

				spotLight->m_position = Vector(x, y, z, 1.0f);
			}
			else
			{
				spotLight->m_position = Vector(0.0f, 0.0f, 0.0f, 1.0f);
			}

			//Load direction
			pLocal = pTempEl->FirstChildElement("Direction");
			pTempAtt = pLocal->FirstAttribute();
			AREAL x = pTempAtt->DoubleValue();

			pTempAtt = pTempAtt->Next();
			AREAL y = pTempAtt->DoubleValue();

			pTempAtt = pTempAtt->Next();
			AREAL z = pTempAtt->DoubleValue();

			spotLight->m_direction = Vector(x, y, z, 1.0f);

			m_graphics.push_back(spotLight);
		}
		else if(componentName == "PointLightComponent")
		{
			GraphicsPointLightComponent* pointLight = new GraphicsPointLightComponent();

			pTempAtt = pTempEl->FirstAttribute();
			pointLight->m_range = pTempAtt->DoubleValue();

			//Load color
			TiXmlElement* pLocal = pTempEl->FirstChildElement("Color");
			pTempAtt = pLocal->FirstAttribute();
			AREAL red = pTempAtt->DoubleValue();

			pTempAtt = pTempAtt->Next();
			AREAL green = pTempAtt->DoubleValue();

			pTempAtt = pTempAtt->Next();
			AREAL blue = pTempAtt->DoubleValue();

			pointLight->m_color = Vector(red, green, blue, 1.0f);

			//Load position
			/*pLocal = pTempEl->FirstChildElement("Position");
			pTempAtt = pLocal->FirstAttribute();
			AREAL x = pTempAtt->DoubleValue();

			pTempAtt = pTempAtt->Next();
			AREAL y = pTempAtt->DoubleValue();

			pTempAtt = pTempAtt->Next();
			AREAL z = pTempAtt->DoubleValue();

			pointLight->m_position = Vector(x, y, z, 1.0f); */
			pointLight->m_position = m_vPos;

			m_graphics.push_back(pointLight);
		}

		pTempEl = pTempEl->NextSiblingElement();
	}
}

AVOID EntityResource::InitPhysicsComponent(TiXmlElement* pComponent)
{
	//Get physics representation
	TiXmlElement* pTempEl = pComponent->FirstChildElement("BoxComponent");
	TiXmlAttribute* pTempAtt;
	if (pTempEl)
	{
		//Create box representation

		PhysicsBoxComponent* box = new PhysicsBoxComponent();

		//X width
		pTempAtt = pTempEl->FirstAttribute();
		box->m_fXHalfExtent = pTempAtt->DoubleValue();

		//Y width
		pTempAtt = pTempAtt->Next();
		box->m_fYHalfExtent = pTempAtt->DoubleValue();

		//Z width
		pTempAtt = pTempAtt->Next();
		box->m_fZHalfExtent = pTempAtt->DoubleValue();

		//Mass
		pTempAtt = pTempAtt->Next();
		box->m_mass = pTempAtt->DoubleValue();

		//Density
		TiXmlElement* pLocal = pTempEl->FirstChildElement("Density");
		//box->m_density = pLocal->Value();
		box->m_density = pLocal->GetText();

		//Material
		pLocal = pLocal->NextSiblingElement();
		box->m_material = pLocal->GetText();

		//Is object static
		pLocal = pLocal->NextSiblingElement();
		ASTRING type = pLocal->GetText();
		box->m_bIsStatic = (type == "Static") ? true : false;

		m_physics.push_back(box);
	}

	pTempEl = pComponent->FirstChildElement("PlaneComponent");
	if (pTempEl)
	{
		//Create plane representation
		PhysicsPlaneComponent* plane = new PhysicsPlaneComponent();

		//X width
		pTempAtt = pTempEl->FirstAttribute();
		plane->m_fXHalfExtent = pTempAtt->DoubleValue();

		//Y width
		pTempAtt = pTempAtt->Next();
		plane->m_fYHalfExtent = pTempAtt->DoubleValue();

		//Z width
		pTempAtt = pTempAtt->Next();
		plane->m_fZHalfExtent = pTempAtt->DoubleValue();

		//Mass
		pTempAtt = pTempAtt->Next();
		plane->m_mass = pTempAtt->DoubleValue();

		//Density
		TiXmlElement* pLocal = pTempEl->FirstChildElement("Density");
		plane->m_density = pLocal->GetText();

		//Material
		pLocal = pLocal->NextSiblingElement();
		plane->m_material = pLocal->GetText();

		//Is object static
		pLocal = pLocal->NextSiblingElement();
		ASTRING type = pLocal->GetText();
		plane->m_bIsStatic = (type == "Static") ? true : false;

		m_physics.push_back(plane);
	}

	pTempEl = pComponent->FirstChildElement("SphereComponent");
	if (pTempEl)
	{
		//Create plane representation
		//PhysicsPlaneComponent* plane = new PhysicsPlaneComponent();
		PhysicsSphereComponent* pSphere = new PhysicsSphereComponent();

		//radius
		pTempAtt = pTempEl->FirstAttribute();
		//plane->m_fXHalfExtent = pTempAtt->DoubleValue();
		pSphere->m_fRadius = pTempAtt->DoubleValue();

		//Mass
		pTempAtt = pTempAtt->Next();
		pSphere->m_mass = pTempAtt->DoubleValue();

		//Density
		TiXmlElement* pLocal = pTempEl->FirstChildElement("Density");
		pSphere->m_density = pLocal->GetText();

		//Material
		pLocal = pLocal->NextSiblingElement();
		pSphere->m_material = pLocal->GetText();

		//Is object static
		pLocal = pLocal->NextSiblingElement();
		ASTRING type = pLocal->GetText();
		pSphere->m_bIsStatic = (type == "Static") ? true : false;

		m_physics.push_back(pSphere);
	}

	pTempEl = pComponent->FirstChildElement("MeshComponent");
	if (pTempEl)
	{
		//Create mesh representation
		PhysicsMeshComponent* mesh = new PhysicsMeshComponent();

		pTempAtt = pTempEl->FirstAttribute();
		mesh->m_filename = pTempAtt->Value();

		//Is object static
		pTempAtt = pTempAtt->Next();
		mesh->m_bIsStatic = (pTempAtt->Value() == "true") ? true : false;

		m_physics.push_back(mesh);
	}

	pTempEl = pComponent->FirstChildElement("CharacterComponent");
	if (pTempEl)
	{
		//Create character representation
		PhysicsCharacterComponent* pCharacter = new PhysicsCharacterComponent();

		//height
		pTempAtt = pTempEl->FirstAttribute();
		pCharacter->m_height = pTempAtt->DoubleValue();

		//width
		pTempAtt = pTempAtt->Next();
		pCharacter->m_width = pTempAtt->DoubleValue();

		//depth
		pTempAtt = pTempAtt->Next();
		pCharacter->m_depth = pTempAtt->DoubleValue();

		//mass
		pTempAtt = pTempAtt->Next();
		pCharacter->m_mass = pTempAtt->DoubleValue();

		//force
		pTempEl = pTempEl->FirstChildElement("Force");
		pTempAtt = pTempEl->FirstAttribute();
		pCharacter->m_forceInNewtons = pTempAtt->DoubleValue();

		//max slope
		pTempEl = pTempEl->NextSiblingElement("MaxSlope");
		pTempAtt = pTempEl->FirstAttribute();
		pCharacter->m_maxSlopeInRadians = DEGREES_TO_RADIANS(pTempAtt->DoubleValue());

		m_physics.push_back(pCharacter);
	}
}

AVOID EntityResource::InitTransform(TiXmlElement* pComponent)
{
	//Acquire position
	TiXmlElement* pTempEl = pComponent->FirstChildElement("Position");
	TiXmlAttribute* pTempAtt;

	if (pTempEl)
	{
		pTempAtt = pTempEl->FirstAttribute();
		AREAL x = pTempAtt->DoubleValue();

		pTempAtt = pTempAtt->Next();
		AREAL y = pTempAtt->DoubleValue();

		pTempAtt = pTempAtt->Next();
		AREAL z = pTempAtt->DoubleValue();

		m_vPos = Vector(x, y, z, 1.0f);
	}
	else
	{
		m_vPos = Vector(0, 0, 0, 1.0f);
	}

	//Acquire orientation
	pTempEl = pComponent->FirstChildElement("Orientation");

	if (pTempEl)
	{
		pTempAtt = pTempEl->FirstAttribute();
		AREAL x = pTempAtt->DoubleValue();

		pTempAtt = pTempAtt->Next();
		AREAL y = pTempAtt->DoubleValue();

		pTempAtt = pTempAtt->Next();
		AREAL z = pTempAtt->DoubleValue();

		pTempAtt = pTempAtt->Next();
		AREAL w = pTempAtt->DoubleValue();

		m_qRot = Quaternion(x, y, z, w);
	}

	//Acquire scaling
	pTempEl = pComponent->FirstChildElement("Scale");
	m_vScale = Vector(1.0f, 1.0f, 1.0f, 1.0f);
	if (pTempEl)
	{
		pTempAtt = pTempEl->FirstAttribute();
		AREAL x = pTempAtt->DoubleValue();

		pTempAtt = pTempAtt->Next();
		AREAL y = pTempAtt->DoubleValue();

		pTempAtt = pTempAtt->Next();
		AREAL z = pTempAtt->DoubleValue();

		m_vScale = Vector(x, y, z, 1.0f);
	}
}

EntityPtr EntityResource::VCreateEntity(Game * pGame)
{
	return pGame->VAddEntity(make_shared<Entity>(Entity(pGame->GetNewEntityId())));
}

AVOID EntityResource::VCreateRepresentation(Scene * pScene, EntityPtr pEntity)
{
	//Create representation
	EntityRepresentation* pRepresentation = new EntityRepresentation(pEntity->GetId());
	pEntity->SetRepresentation(pRepresentation);

	//Apply transformations
	Mat4x4 trans, scale;
	trans.CreateTranslation(m_vPos);
	scale.CreateScaling(m_vScale);
	pRepresentation->SetCurrentTransform(scale * trans, g_pEngine->GameTimeInSeconds());
	pScene->AddRepresentation(pRepresentation);

	//Now add all graphics components to its representation
	for (auto component = m_graphics.begin(); component != m_graphics.end(); component++)
	{
		ASTRING type = (*component)->VGetType();
		if (type == "Box")
		{
			GraphicsBoxComponent * box = static_cast<GraphicsBoxComponent *>((*component));

			//Create box mesh
			BoxMesh* pBoxMesh = new BoxMesh(box->m_fXHalfExtent, box->m_fYHalfExtent, box->m_fZHalfExtent);
			pBoxMesh->VSetMaterial(box->m_material);
			shared_ptr<BoxMesh> pBoxMeshPtr = make_shared<BoxMesh>(*pBoxMesh);
			pRepresentation->VAddMesh(static_pointer_cast<Mesh>(pBoxMeshPtr));
		}
		else if (type == "Plane")
		{
			GraphicsPlaneComponent * plane = static_cast<GraphicsPlaneComponent *>((*component));

			//Create plane mesh
			PlaneMesh* pPlaneMesh = new PlaneMesh(plane->m_fXHalfExtent, plane->m_fYHalfExtent, plane->m_fZHalfExtent);
			pPlaneMesh->VSetMaterial(plane->m_material);
			shared_ptr<PlaneMesh> pPlaneMeshPtr = make_shared<PlaneMesh>(*pPlaneMesh);
			pRepresentation->VAddMesh(static_pointer_cast<Mesh>(pPlaneMeshPtr));
		}
		else if (type == "Sprite")
		{
			GraphicsSpriteComponent * sprite = static_cast<GraphicsSpriteComponent *>((*component));

			//Create plane mesh
			Sprite* pSpriteMesh = new Sprite(sprite->m_fXHalfExtent, sprite->m_fYHalfExtent);
			pSpriteMesh->VSetMaterial(sprite->m_material);
			shared_ptr<Sprite> pSpriteMeshPtr = make_shared<Sprite>(*pSpriteMesh);
			pRepresentation->VAddMesh(static_pointer_cast<Mesh>(pSpriteMeshPtr));
		}
		else if (type == "Mesh")
		{
			GraphicsMeshComponent * mesh = static_cast<GraphicsMeshComponent *>((*component));

			//Create mesh
			Resource meshResource(mesh->m_filename);
			shared_ptr<ResHandle> pMeshes = Anubis::SafeGetHandle(&meshResource);
			std::shared_ptr<MeshResourceExtraData> pData = static_pointer_cast<MeshResourceExtraData>(pMeshes->GetExtra());

			AUINT32 uNumMeshes = pData->GetNumMeshes();
			for (AUINT32 i = 0; i < uNumMeshes; i++)
			{
				//m_meshes.push_back(pData->m_pMeshes[i]);
				pRepresentation->VAddMesh(pData->m_pMeshes[i]);
			}
		}
		else if (type == "PointLight")
		{
			GraphicsPointLightComponent * pointLight = static_cast<GraphicsPointLightComponent *>((*component));

			//Create point light
			PointLight* pLight = new PointLight(pointLight->m_color, pointLight->m_position + m_vOffset, pointLight->m_range);
			//PointLight* pLight = new PointLight(pointLight->m_color, Vector(0, 0, 0, 1), pointLight->m_range);
			shared_ptr<PointLight> pLightPtr = make_shared<PointLight>(*pLight);
			pRepresentation->VAddLight(pLightPtr);
		}
		else if (type == "SpotLight")
		{
			GraphicsSpotLightComponent * spotLight = static_cast<GraphicsSpotLightComponent *>((*component));

			//Create point light
			//SpotLight* pLight = new SpotLight(spotLight->m_color, spotLight->m_position, spotLight->m_direction,
			//	spotLight->m_range, spotLight->m_innerAngle, spotLight->m_outerAngle);
			SpotLight* pLight = new SpotLight(spotLight->m_color, spotLight->m_position + m_vOffset, spotLight->m_direction,
				spotLight->m_range, spotLight->m_innerAngle, spotLight->m_outerAngle);
			shared_ptr<SpotLight> pLightPtr = make_shared<SpotLight>(*pLight);
			pRepresentation->VAddLight(pLightPtr);
		} 
	}
}

AVOID EntityResource::VCreatePhysicalBody(IPhysics * pPhysics, EntityPtr pEntity)
{
	//pPhysics->VAddBox(pEntity, m_pos, Vector(5, 5, 5, 1.0f), 10.0, ASTRING("Normal"), true);
	//Now add all graphics components to its representation
	for (auto component = m_physics.begin(); component != m_physics.end(); component++)
	{
		ASTRING type = (*component)->VGetType();
		if (type == "Box")
		{
			PhysicsBoxComponent * box = static_cast<PhysicsBoxComponent *>((*component));
			pPhysics->VAddBox(pEntity, m_vPos + m_vOffset, m_qRot, Vector(box->m_fXHalfExtent, box->m_fYHalfExtent, box->m_fZHalfExtent, 1.0f),
				box->m_mass, box->m_material, box->m_bIsStatic);
		}
		if (type == "Plane")
		{
			PhysicsPlaneComponent * plane = static_cast<PhysicsPlaneComponent *>((*component));
			pPhysics->VAddPlane(pEntity, m_vPos + m_vOffset, m_qRot, Vector(plane->m_fXHalfExtent, plane->m_fYHalfExtent, plane->m_fZHalfExtent, 1.0f),
				plane->m_mass, plane->m_material, plane->m_bIsStatic);
		}
		if (type == "Sphere")
		{
			PhysicsSphereComponent * sphere = static_cast<PhysicsSphereComponent *>((*component));
			pPhysics->VAddSphere(pEntity, m_vPos + m_vOffset, m_qRot, sphere->m_fRadius, sphere->m_mass, sphere->m_material, sphere->m_bIsStatic);
		}
		if (type == "Character")
		{
			PhysicsCharacterComponent * character = static_cast<PhysicsCharacterComponent *>((*component));
			pPhysics->VAddCharacter(pEntity, m_vPos + m_vOffset, m_qRot, Vector(character->m_width, character->m_height, character->m_depth, 0.0f),
				character->m_mass, character->m_forceInNewtons, character->m_maxSlopeInRadians);
		}
	}
}
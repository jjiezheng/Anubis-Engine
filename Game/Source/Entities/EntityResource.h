#pragma once
//====================================================================================
//EntityResource.h
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

#include "../../../Math/Source/Vectors.h"
#include "../../../Math/Source/Quaternion.h"

#include "../GameLogic/Game.h"
#include "PhysicsInterface.h"
#include "Havok\Havok.h"
#include "Representations\Representation.h"
#include "Data\XML\tinyxml.h"

namespace Anubis
{
	class Game;
	/*struct EntityResource
	{
		//position
		Vec	m_pos;

		ASTRING	m_name; //mainly for debugging purposes

		//Mat4x4	m_mat;

		//identifier
		EntityId	m_id;

		EntityResource(EntityId id = 0) : m_id(id)	{}
		AVIRTUAL AVOID VLoad(const ASTRING & filename);
		AVIRTUAL ~EntityResource()	{}

		//	 =====================	 //
		//		Factory Pattern		 //
		//	 =====================	 //

		//create entity
		AVIRTUAL EntityPtr	VCreateEntity(Game * pGame) = 0;

		//create its representation
		AVIRTUAL EntityRepresentation* VCreateRepresentation(Scene * pScene, EntityPtr pEntity) = 0;

		//create its physics model
		AVIRTUAL AVOID VCreatePhysicalBody(IPhysics * pPhysics, EntityPtr pEntity) = 0;

		/// ***	Copy resource data *** ///
		AVIRTUAL EntityResource & VCopy() = 0;
	}; */


	struct EntityComponent
	{
		AVIRTUAL ASTRING VGetType() const { return "Component"; }
	};
	///////////////////////////////////////////////////////////
	//Graphics Component
	///////////////////////////////////////////////////////////
	struct GraphicsComponent : public EntityComponent
	{
		AWSTRING m_material;
	};

	struct GraphicsPlaneComponent : public GraphicsComponent
	{
		AVIRTUAL ASTRING VGetType() const { return "Plane"; }

		AREAL m_fXHalfExtent;
		AREAL m_fYHalfExtent;
		AREAL m_fZHalfExtent;
	};

	struct GraphicsBoxComponent : public GraphicsComponent
	{
		ASTRING VGetType() const { return "Box"; }

		AREAL m_fXHalfExtent;
		AREAL m_fYHalfExtent;
		AREAL m_fZHalfExtent;
	};

	struct GraphicsSphereComponent : public GraphicsComponent
	{
		ASTRING VGetType() const { return "Sphere"; }

		AREAL m_fRadius;
	};

	struct GraphicsMeshComponent : public GraphicsComponent
	{
		ASTRING VGetType() const { return "Mesh"; }

		ASTRING m_filename;
	};

	struct GraphicsLightComponent : public GraphicsComponent
	{
		Vec m_color;
	};

	struct GraphicsPointLightComponent : public GraphicsLightComponent
	{
		ASTRING VGetType() const { return "PointLight"; }

		Vec m_position;
		AREAL m_range;
	};

	struct GraphicsDirectionalLightComponent : public GraphicsLightComponent
	{
		ASTRING VGetType() const { return "DirectionalLight"; }

		Vec m_direction;
	};

	struct GraphicsSpotLightComponent : public GraphicsLightComponent
	{
		ASTRING VGetType() const { return "SpotLight"; }

		Vec m_direction;
		Vec m_position;

		AREAL m_innerAngle;
		AREAL m_outerAngle;
		AREAL m_range;
	};


	///////////////////////////////////////////////////////////
	//Physics Component
	///////////////////////////////////////////////////////////
	struct PhysicsComponent : public EntityComponent
	{
		//Convert to actual values when creating physics model
		ASTRING m_material;
		ASTRING	m_density;
		AREAL	m_mass;

		ABOOL	m_bIsStatic;
	};

	struct PhysicsPlaneComponent : public PhysicsComponent
	{
		AVIRTUAL ASTRING VGetType() const { return "Plane"; }

		AREAL m_fXHalfExtent;
		AREAL m_fYHalfExtent;
		AREAL m_fZHalfExtent;
	};

	struct PhysicsBoxComponent : public PhysicsComponent
	{
		ASTRING VGetType() const { return "Box"; }

		AREAL m_fXHalfExtent;
		AREAL m_fYHalfExtent;
		AREAL m_fZHalfExtent;
	};

	struct PhysicsSphereComponent : public PhysicsComponent
	{
		ASTRING VGetType() const { return "Sphere"; }

		AREAL m_fRadius;
	};

	struct PhysicsMeshComponent : public PhysicsComponent
	{
		ASTRING VGetType() const { return "Mesh"; }

		ASTRING m_filename;
	};

	struct PhysicsCharacterComponent : public PhysicsComponent
	{
		ASTRING VGetType() const { return "Character"; }

		AREAL m_height;
		AREAL m_width;
		AREAL m_depth;

		AREAL m_forceInNewtons;
		AREAL m_maxSlopeInRadians;
	};

	struct EntityResource
	{
		Vec m_vPos;
		Quaternion m_qRot;

		////////////////////////////
		//Entity Components
		//GraphicsComponent* m_pGraphics;
		//PhysicsComponent* m_pPhysics;

		typedef vector<GraphicsComponent*> Graphics;
		typedef vector<PhysicsComponent*> Physics;

		Graphics m_graphics;
		Physics  m_physics;

		EntityResource(const ACHAR* filename);
		//AVIRTUAL AVOID VLoad(const ASTRING & filename);
		AVIRTUAL ~EntityResource();

		//	 =====================	 //
		//		Factory Pattern		 //
		//	 =====================	 //

		//create entity
		AVIRTUAL EntityPtr	VCreateEntity(Game * pGame);

		//create its representation
		AVIRTUAL AVOID VCreateRepresentation(Scene * pScene, EntityPtr pEntity);

		//create its physics model
		AVIRTUAL AVOID VCreatePhysicalBody(IPhysics * pPhysics, EntityPtr pEntity);

		/// ***	Copy resource data *** ///
		//AVIRTUAL EntityResource & VCopy();

	private:

		AVOID InitTransform(TiXmlElement* pComponent);
		AVOID InitPhysicsComponent(TiXmlElement* pComponent);
		AVOID InitGraphicsComponent(TiXmlElement* pComponent);

	};
};
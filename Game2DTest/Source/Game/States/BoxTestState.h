#pragma once

#include "GameStateMachine\State.h"
#include "Entities\Entity.h"
#include "Scene.h"

namespace Anubis
{
	class BoxTestState : public State
	{
	protected:
		EntityPtr m_pLight;
		Vec m_lightVelocity;
		vector<EntityPtr> m_lights;

	public:

		//constructor
		BoxTestState() : State() {}

		AVIRTUAL AVOID VInitialize( Game * pGame, AUINTPTR uptrData = 0 );										//Initialize game state

		AVIRTUAL AVOID VRelease( Game * pGame );																//Destroy the state

		AVIRTUAL AVOID VUpdate( Game * pGame, AREAL64 r64Time, AREAL64 r64ElapsedTime );						//Update the state

		AVIRTUAL AVOID VRender( Game * pGame, Renderer * pRenderer, AREAL64 r64Time, AREAL64 r64ElapsedTime );	//Render State
	};

}; //Anubis
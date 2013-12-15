#pragma once
//====================================================================================
//PointLight.h
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

#include "Light.h"
#include "Final\Resources\Buffers\BufferParams.h"

namespace Anubis
{
	class PointLight : public Light
	{
	protected:

		TextureCube* m_pShadowTex;
		ShaderResourceView* m_pShadowSRV;
		RenderTargetView* m_pShadowRTV;
		RenderTargetView* m_pShadowOneRTV[6];
		
		Texture2D*			m_pShadowDepthTex;
		DepthStencilView*	m_pShadowDSV;
		DepthStencilView*	m_pShadowOneDSV;

	public:

		//Constructor
		PointLight(const Vec color, const Vec pos, const AREAL32 r32Range) 
			: Light(color, pos, Vector(0.0f, -1.0f, 0.0f, 1))
		{
			m_r32Range = r32Range;

			m_pShadowTex = new TextureCube();
			m_pShadowSRV = new ShaderResourceView();
			m_pShadowRTV = new RenderTargetView();

			m_pShadowDepthTex = new Texture2D();
			m_pShadowDSV = new DepthStencilView();
			m_pShadowOneDSV = new DepthStencilView();

			for (AINT8 i = 0; i < 6; i++)
			{
				m_pShadowOneRTV[i] = new RenderTargetView();
			}
		};

		AVIRTUAL ABOOL VInitialize(INPUT_LAYOUT * pLayout);


		/**
			== Rendering ==
							**/
		AVOID VPreRender(Renderer *pRenderer);
		//AVOID VRender();
		AVOID VPostRender(Renderer *pRenderer);
		AVOID VSetScissorRect(Renderer* pRenderer, CameraPtr pCamera);

		AVOID VPrepareToGenerateShadowMap(const Mat4x4 & world, Renderer * pRenderer);
		AVOID VClearShadowMap();
		AVOID VFinishShadowMapGeneration(Renderer *pRenderer);

		/**
			== Getters ==
							**/
		AVIRTUAL LightType VGetType() const { return LT_Point; }

		//Vec GetPosition() const { return m_pos; }
	};

}; //Anubis
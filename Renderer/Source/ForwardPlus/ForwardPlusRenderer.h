#pragma once
//====================================================================================
//ForwardPlusRenderer.h
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

#include "Renderer.h"

#include "../Cameras/Camera.h"
#include "../RenderQueue.h"

#define VOXEL_GRID_SIZE 32

namespace Anubis
{
	class ForwardPlusRenderer : public Renderer
	{
	protected:
		AVOID ZPrepass();
		AVOID LightCulling(CameraPtr pCamera);
		AVOID FinalShading();

		AVOID NullLightCounter();

		/////////////////////////////////////
		//		== Shaders ==
		ShaderBunch* m_pZPrepassShaders;
		ComputeShader* m_pLightCullingShader;
		ShaderBunch* m_pFinalShadingShaders;

		INPUT_LAYOUT* m_pZPrepassLayout;
		INPUT_LAYOUT* m_pDefaultLayout;

		/////////////////////////////////////
		// Resources
		Texture2D* m_pLinearZTexture;
		ShaderResourceView* m_pLinearZSRV;
		RenderTargetView* m_pLinearZRTV;
		//Texture2D* m_pZTexture;
		//DepthStencilView* m_pZDSV;

		//meshes
		RenderQueue m_queue;

		//lights
		typedef vector<Light*> LightList;
		LightList	m_lights;

		StructuredBuffer*		m_psbLightIdx;
		UnorderedAccessView*	m_pLightIdxUAV;
		ShaderResourceView*		m_pLightIdxSRV;

		StructuredBuffer*		m_psbLowerBound;
		UnorderedAccessView*	m_pLowerBoundUAV;
		ShaderResourceView*		m_pLowerBoundSRV;

		StructuredBuffer*		m_psbHigherBound;
		UnorderedAccessView*	m_pHigherBoundUAV;
		ShaderResourceView*		m_pHigherBoundSRV;

		StructuredBuffer*		m_psbLightCounter;
		UnorderedAccessView*	m_pLightCounterUAV;
		ShaderResourceView*		m_pLightCounterSRV;

		StructuredBuffer*		m_psbLightGeometry;
		ShaderResourceView*		m_pLightGeometrySRV;

		StructuredBuffer*		m_psbLights;
		ShaderResourceView*		m_pLightsSRV;

		AUINT32					m_iNumLights;
		LightGeometry*			m_pLightGeometryData;
		LightParams*			m_pLightParams;

		ConstantBuffer*			m_pcbCameraCulling;
		ConstantBuffer*			m_pcbCullingData;
		struct CameraCullingData
		{
			Mat4x4 View;
			Mat4x4 ViewProj;

			Vec posANDfov;

			float  viewWidth;
			float  viewHeight;
			float  cameraNear;
			float  camersFar;

			Vec	   dir;
		};
		CameraCullingData m_cameraCullingData;

		////////////////////////////////////
		//Sky rendering
		TextureCube*	m_pSkyTexture;
		ShaderResourceView* m_pSkySRV;
		INPUT_LAYOUT*	m_pSkyLayout;
		ShaderBunch* m_pSkyShaders;
		ShaderBunch* m_pAtmoShaders;
		shared_ptr<IndexedMesh> m_pSphereMesh;

		ComputeShader*  m_pSphericalCoeffShader;
		ComputeShader*	m_pIrradianceMapShader;
		TextureCube*	m_pIrradianceMap;
		ShaderResourceView* m_pIrradianceSRV;
		//RenderTargetView*	m_pIrradianceRTV;
		RenderTargetView* m_pIrradianceRTV[6];

		////////////////////////////////////
		//Voxelized global illumination
		unique_ptr<StructuredBuffer> m_voxelGrid;
		unique_ptr<ShaderResourceView> m_voxelGridSRV;
		unique_ptr<UnorderedAccessView> m_voxelGridUAV;

		unique_ptr<Texture2D> m_voxelTexture;
		unique_ptr<RenderTargetView> m_voxelTextureRTV;

		unique_ptr<Texture2D> m_redVoxelSH;
		unique_ptr<Texture2D> m_greenVoxelSH;
		unique_ptr<Texture2D> m_blueVoxelSH;

		INPUT_LAYOUT*	m_pVoxelizationLayout;
		unique_ptr<ShaderBunchVGP> m_voxelizationShaders;
		unique_ptr<ShaderBunch> m_voxelRenderingShaders;
		Viewport m_voxelViewport;

		Viewport m_viewport;

	public:
		/***		=====		***
		 Constructor and Destructor
		 ***		=====	   ***/
		ForwardPlusRenderer();
		~ForwardPlusRenderer();

		/***	Initialize	  ***/
		ABOOL VInitialize(HWND hWnd, AUINT32 width, AUINT32 height);

		/***	Put something to render	***/
		AVOID VPushMesh(Mesh * pMesh);
		AVOID VPushLight(Light * pLight);

		/***	Update Render State		***/
		AVOID VUpdate(const AUINT32 deltaMilliseconds);

		/**		Voxelization Pass ***/
		AVOID Voxelize(const Mat4x4 & viewproj);
		AVOID RenderGrid(const Mat4x4 & viewproj);

		/***	Render Scene	***
		**************************/
		AVOID VRender();
		AVOID PrepareForZPrepass(CameraPtr pCamera);
		AVOID PrepareForShadingPass(CameraPtr pCamera);
		AVOID VFinishPass();

		AVOID VGenerateShadowMaps();
		AVOID VRenderSky(Anubis::CameraPtr,const Anubis::Mat4x4 &);
		AVOID VGenerateIrradianceMap();

	protected:
		AVOID UpdateLightBuffers();
	};
}; //namespace Anubis
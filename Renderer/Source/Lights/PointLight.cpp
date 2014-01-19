//====================================================================================
//PointLight.cpp
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
#include "Renderer_pch.h"
#include "PointLight.h"
#include "../Renderer.h"
#include <Final\General.h>
#include "../../Math/Source/Math.h"

using namespace Anubis;

ABOOL PointLight::VInitialize(INPUT_LAYOUT * pLayout)
{
	//Initialize default light shaders
	m_pShaders = new ShaderBunch();
	m_pShaders->VSetVertexShader(m_vertexShaderFile,	DEFAULT_VERTEX_SHADER_NAME, pLayout, 2, TOPOLOGY_TRIANGLELIST, "vs_4_0_level_9_3");
	m_pShaders->VSetPixelShader(m_pixelShaderFile,	DEFAULT_PIXEL_SHADER_NAME, "ps_4_0_level_9_3");

	//Initialize textures for shadow mapping
	/*Texture2DParams tex2DParams;
	tex2DParams.Init(SCREEN_WIDTH, SCREEN_HEIGHT, 1, DXGI_FORMAT_R32_TYPELESS, true, false, false, true, 1, 0,
		1, true, false, false);
	m_pShadowMapTex->Create(&tex2DParams);

	//create shader resource and render target view for the texture
	ShaderResourceViewParams srvParams;
	srvParams.InitForTexture2D(DXGI_FORMAT_R32_FLOAT, 0, 0, false);
	m_pShadowMapTex->CreateShaderResourceView(&m_pShadowMapSRV->m_pView, &srvParams);

	DepthStencilViewParams dsvParams;
	dsvParams.InitForTexture2D(DXGI_FORMAT_D32_FLOAT, 0, false);
	if (!m_pShadowMapTex->CreateDepthStencilView(&m_pShadowMapDSV->m_pView, &dsvParams))	return false;
	*/
	/*** Initialize texture for variance shadow mapping ***/
	//tex2DParams.Init(SCREEN_WIDTH, SCREEN_HEIGHT, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, true, true, true, false,
	//	1, 0, 8, true, false, false, true);
	//m_pTempTexture->Create(&tex2DParams);

/*	const AINT32 shadowMapSize = 1024;
	tex2DParams.InitCubeTexture(shadowMapSize, shadowMapSize, 6, TEX_R32G32B32A32_FLOAT, true, false, true, false,
		1, 0, 1, true, false, false);
	m_pShadowTex->Create(&tex2DParams);


	//create shadow resource view
	//srvParams.InitForTexture2D(DXGI_FORMAT_R32G32B32A32_FLOAT, 8, 0);
	//m_pVarianceShadowTex->CreateShaderResourceView(&m_pVarianceShadowSRV->m_pView, &srvParams);
	//m_pTempTexture->CreateShaderResourceView(&m_pTempSRV->m_pView, &srvParams);
	srvParams.InitForCubeTexture(TEX_R32G32B32A32_FLOAT, 1, 0);
	m_pShadowTex->CreateShaderResourceView(&m_pShadowSRV->m_pView, &srvParams);

	RenderTargetViewParams rtvParams;
	rtvParams.InitForTexture2DArray(6, tex2DParams.Format, 0, 0);
	m_pShadowTex->CreateRenderTargetView(&m_pShadowRTV->m_pView, &rtvParams);

	rtvParams.Texture2DArray.ArraySize = 1;
	for (AINT8 i = 0; i < 6; i++)
	{
		rtvParams.Texture2DArray.FirstArraySlice = i;
		m_pShadowTex->CreateRenderTargetView(&m_pShadowOneRTV[i]->m_pView, &rtvParams);
	}
	
	m_shadowViewport = Viewport(0, 0, shadowMapSize, shadowMapSize, 0.0f, 1.0f);

	//////////////////////////////////////////////
	// 
	tex2DParams.Init(shadowMapSize, shadowMapSize, 6, DXGI_FORMAT_D32_FLOAT,false, false, false, true,
		1, 0, 1, true, false, false);
	m_pShadowDepthTex->Create(&tex2DParams);

	dsvParams.InitForTexture2DArray(6, DXGI_FORMAT_D32_FLOAT, 0, 0);
	m_pShadowDepthTex->CreateDepthStencilView(&m_pShadowDSV->m_pView, &dsvParams);

	dsvParams.Texture2DArray.ArraySize = 1;
	m_pShadowDepthTex->CreateDepthStencilView(&m_pShadowOneDSV->m_pView, &dsvParams);
	*/
	m_bInitialized = true;

	return true;
}

AVOID PointLight::VPreRender(Renderer *pRenderer)
{
	Light::VPreRender(pRenderer);

	//bind point light specific data
	Vec range = Vector(m_r32Range, m_r32Range, m_r32Range, m_r32Range);
	pRenderer->m_pcbPointLight->UpdateSubresource(0, NULL, &range, 0, 0);
//	pRenderer->m_pcbPointLight->UpdateSubresource(0, nullptr, &m_vec, 0, 0);
	pRenderer->m_pcbPointLight->Set(2, ST_Pixel);

	m_pShadowSRV->Set(9, ST_Pixel);
}

AVOID PointLight::VPostRender(Renderer* pRenderer)
{
	Light::VPostRender(pRenderer);
	D3D11DeviceContext()->RSSetScissorRects(0, nullptr);
	UnbindShaderResourceViews(9, 1, ST_Pixel);
}

AVOID PointLight::VSetScissorRect(Renderer* pRenderer, CameraPtr pCamera)
{
	Vec vCenterWS = m_pData->m_pos;

	//convert center to view space
	Vec vCenterVS = vCenterWS * pCamera->GetView();

	//calculate four points (top, bottom, left, right) around the light
	Vec vTopVS = vCenterVS + Vector(0.0f, m_r32Range, 0.0f, 0.0f);
	Vec vBottomVS = vCenterVS - Vector(0.0f, m_r32Range, 0.0f, 0.0f);
	Vec vLeftVS = vCenterVS - Vector(m_r32Range, 0.0f, 0.0f, 0.0f);
	Vec vRightVS = vCenterVS + Vector(m_r32Range, 0.0f, 0.0f, 0.0f);

	//decide which points we take to bound whole light volume to the quad
	vTopVS.z	= vTopVS.y < 0.0f ? vTopVS.z + m_r32Range : vTopVS.z - m_r32Range;
	vBottomVS.z = vBottomVS.y < 0.0f ? vBottomVS.z - m_r32Range : vBottomVS.z + m_r32Range;
	vLeftVS.z	= vLeftVS.x < 0.0f ? vLeftVS.z - m_r32Range : vLeftVS.z + m_r32Range;
	vRightVS.z	= vRightVS.x < 0.0f ? vRightVS.z + m_r32Range : vRightVS.z - m_r32Range;

	//clamp positions to near and far frustum planes
	AREAL r32Near = pCamera->GetNearZ();
	AREAL r32Far = pCamera->GetFarZ();

	vTopVS.z	= Clamp(vTopVS.z, r32Near, r32Far);
	vBottomVS.z = Clamp(vBottomVS.z, r32Near, r32Far);
	vLeftVS.z	= Clamp(vLeftVS.z, r32Near, r32Far);
	vRightVS.z	= Clamp(vRightVS.z, r32Near, r32Far);

	//now transform to clip space
	Mat4x4 & proj = pCamera->GetProjection();

	//!! perspective projection matrix has following view,
	//so, we can easily economize computations
	// s		0		0			0
	// 0		s		0			0
	// 0		0		-f/(f-n)	-1
	// 0		0		-f*n/(f-n)	0
	AREAL topCS		= vTopVS.y * proj.GetElement(1, 1) / vTopVS.z;
	AREAL bottomCS	= vBottomVS.y * proj.GetElement(1, 1) / vBottomVS.z;
	AREAL leftCS	= vLeftVS.x * proj.GetElement(0, 0) / vLeftVS.z;
	AREAL rightCS	= vRightVS.x * proj.GetElement(0, 0) / vRightVS.z;

	//clamp coordinates
	topCS		= Clamp(topCS, -1.0f, 1.0f);
	bottomCS	= Clamp(bottomCS, -1.0f, 1.0f);
	leftCS		= Clamp(leftCS, -1.0f, 1.0f);
	rightCS		= Clamp(rightCS, -1.0f, 1.0f);

	//convert to screen coordinates
	AREAL topSS		= (topCS -1.0f) * (-0.5f);
	AREAL bottomSS	= (bottomCS - 1.0f) * (-0.5f);
	AREAL leftSS	= (leftCS + 1.0f) * 0.5f;
	AREAL rightSS	= (rightCS + 1.0f) * 0.5f;

	//viewport coordinates
	topSS		*= pCamera->GetViewportHeight();
	bottomSS	*= pCamera->GetViewportHeight();
	leftSS		*= pCamera->GetViewportWidth();
	rightSS		*= pCamera->GetViewportWidth();

	//A little bit API specific code here...
	D3D11_RECT scissorRect;
	scissorRect.top		= static_cast<AINT32>(topSS);
	scissorRect.bottom	= static_cast<AINT32>(bottomSS);
	scissorRect.left	= static_cast<AINT32>(leftSS);
	scissorRect.right	= static_cast<AINT32>(rightSS);

	//clamp values, yes again...
	scissorRect.top		= max(0.0f, scissorRect.top);
	scissorRect.bottom	= min(static_cast<AINT32>(pCamera->GetViewportHeight()), scissorRect.bottom);
	scissorRect.left	= max(0.0f, scissorRect.left);
	scissorRect.right	= min(static_cast<AINT32>(pCamera->GetViewportWidth()), scissorRect.right);

	D3D11DeviceContext()->RSSetScissorRects(1, &scissorRect);
}

AVOID PointLight::VPrepareToGenerateShadowMap(const Mat4x4 & world, Renderer * pRenderer)
{
	//Set depth stencil view
	//if (pRenderer->m_bVarianceShadows)
	//{
	//	m_pVarianceShadowRTV->Set(*m_pShadowMapDSV);
		//m_pVarianceShadowRTV->Set();
	//}
	//else
	//{
	//	SetDepthStencilView(m_pShadowMapDSV);
	//}
	//GetRenderTargetView()->Set(*m_pShadowMapDSV);

	m_pShadowRTV->Set(*m_pShadowDSV);

	//set constant buffer with orthographic projection
	Vec pos = m_pData->m_pos * m_worldTransform;
	Vec dir = Vector(m_pData->m_dir.x, m_pData->m_dir.y, m_pData->m_dir.z, 0.0f);
	Mat4x4 view = CreateViewMatrixLH(pos, dir, Vector(0.0f, 1.0f, 0.0f, 0.0f));
	//Mat4x4 ortho = CreateOrthoProjectionLH(m_iShadowMapWidth * 1.5f, m_iShadowMapHeight * 1.5f, 0.5f, m_r32Range);
	//Mat4x4 ortho = CreateOrthoProjectionLH(m_iShadowMapWidth, m_iShadowMapHeight, 2.0f, 60.0f);
	//Mat4x4 ortho = CreatePerspectiveProjectionLH( 0.25f * 3.14f, (AREAL)SCREEN_WIDTH / (AREAL)SCREEN_HEIGHT, 0.5f, 30.0f);
	//Mat4x4 perspective = CreatePerspectiveProjectionLH(3.14f / 2.0f, 1.0f, 0.5f, m_r32Range);
	Mat4x4 perspective = CreatePerspectiveProjectionLH(3.14159265359f * 0.5f, 1.0f, 0.5f, m_r32Range);
	//Mat4x4 perspective = CreateOrthoProjectionLH(512 * 1.5f, 512 * 1.5f, 0.0f, m_r32Range);

	m_view = view;
	//m_viewProjection = view * ortho;
	m_viewProjection = view * perspective;

	Mat4x4 World = world;
	World.Transpose();
	pRenderer->m_pcbView->UpdateSubresource(0, nullptr, &World, 0, 0);
	pRenderer->m_pcbView->Set(0, ST_Vertex);

	struct SevenMatrices
	{
		Mat4x4 view[6];
		Mat4x4 proj;
	};
	SevenMatrices matrices;
	matrices.view[0] = CreateViewMatrixLH(pos, Vector(1.0f, 0.0f, 0.0f, 0.0f), Vector(0.0f, 1.0f, 0.0f, 0.0f));
	matrices.view[1] = CreateViewMatrixLH(pos, Vector(-1.0f, 0.0f, 0.0f, 0.0f), Vector(0.0f, 1.0f, 0.0f, 0.0f));
	matrices.view[2] = CreateViewMatrixLH(pos, Vector(0.0f, 1.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, -1.0f, 0.0f));
	matrices.view[3] = CreateViewMatrixLH(pos, Vector(0.0f, -1.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, 1.0f, 0.0f));
	matrices.view[4] = CreateViewMatrixLH(pos, Vector(0.0f, 0.0f, 1.0f, 0.0f), Vector(0.0f, 1.0f, 0.0f, 0.0f));
	matrices.view[5] = CreateViewMatrixLH(pos, Vector(0.0f, 0.0f, -1.0f, 0.0f), Vector(0.0f, 1.0f, 0.0f, 0.0f));
	for (AINT8 i = 0; i < 6; i++)
		matrices.view[i].Transpose();
	matrices.proj = perspective;
	matrices.proj.Transpose();
	pRenderer->m_pcb7Matrices->UpdateSubresource(0, nullptr, &matrices, 0, 0);
	pRenderer->m_pcb7Matrices->Set(0, ST_Geometry);
	/*Mat4x4 WVP = world * m_viewProjection;
	WVP.Transpose();

	if (pRenderer->m_bVarianceShadows)
	{
		Mat4x4 WorldView = world * view;
		WorldView.Transpose();

		struct MatrixBuffer
		{
			Mat4x4 WorldView;
			Mat4x4 WVP;
		};
		MatrixBuffer buffer;
		buffer.WorldView = WorldView;
		buffer.WVP = WVP;
		pRenderer->m_pcbWorldPlusWVP->UpdateSubresource(0, NULL, &buffer, 0, 0);
		pRenderer->m_pcbWorldPlusWVP->Set(0, ST_Vertex);
	}
	else
	{
		pRenderer->m_pcbWVP->UpdateSubresource(0, NULL, &WVP, 0, 0);
		pRenderer->m_pcbWVP->Set(0, ST_Vertex);
	} */

	//Actually stores length of light frustum
	Vec frustumSize = Vector(m_r32Range, m_r32Range, m_r32Range, m_r32Range);
	pRenderer->m_pcbCameraPos->UpdateSubresource(0, NULL, &frustumSize, 0, 0);
	pRenderer->m_pcbCameraPos->Set(0, ST_Pixel);

	m_shadowViewport.Set();
}

AVOID PointLight::VClearShadowMap()
{
	AREAL bgColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	ClearDepthStencilView(true, false, 1.0f, 0xFF, m_pShadowDSV);
	ClearRenderTargetView(bgColor, m_pShadowRTV);
} 

AVOID PointLight::VFinishShadowMapGeneration(Renderer * pRenderer)
{
	if (pRenderer->m_bVarianceShadows)
	{
		SetRenderTargetView();
	}
	UnbindGeometryShader();
}
//====================================================================================
//BoxMesh.h
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
#include "BoxMesh.h"

#include "Graphics_pch.h"

#include "Final\Resources\Buffers\BufferParams.h"
#include "Final\Resources\ResourceData.h"
#include "../../Math/Source/Vectors.h"

#include "../../Graphics/Source/Algorithms.h"
#include "Final\Stages\Shaders\Shaders.h"

#include "Renderer.h"

using namespace Anubis;

BoxMesh::BoxMesh(const AREAL scalex, const AREAL scaley, const AREAL scalez)
{
	//Initialize buffers
	m_pVertices = new VertexBuffer();
	m_pTexCoords = new VertexBuffer();
	m_pNormals = new VertexBuffer();

	//m_pShaders = new ShaderBunch();

	//Buffer resource
	BufferParams params;
	params.FillVertexBufferParams(sizeof(float3), 36, true, false, false, false);

	//resource data
	SubresourceData data;

	#pragma region Positions
	float3 pos [] = 
	{
		float3(-scalex, scaley, -scalez),
		float3(scalex, scaley, -scalez), 
		float3(-scalex, -scaley, -scalez),

		float3(-scalex, -scaley, -scalez),
		float3(scalex, scaley, -scalez),
		float3(scalex, -scaley, -scalez),
		
		float3(scalex, scaley, -scalez),
		float3(scalex, scaley, scalez),
		float3(scalex, -scaley, -scalez),
		
		float3(scalex, -scaley, -scalez),
		float3(scalex, scaley, scalez),
		float3(scalex, -scaley, scalez),
		
		float3(scalex, scaley, scalez),
		float3(-scalex, scaley, scalez),
		float3(scalex, -scaley, scalez),
		
		float3(scalex, -scaley, scalez),
		float3(-scalex, scaley, scalez),
		float3(-scalex, -scaley, scalez),
		
		float3(-scalex, scaley, scalez),
		float3(-scalex, scaley, -scalez),
		float3(-scalex, -scaley, scalez),
		
		float3(-scalex, -scaley, scalez),
		float3(-scalex, scaley, -scalez),
		float3(-scalex, -scaley, -scalez),
		
		float3(-scalex, scaley, scalez),
		float3(scalex, scaley, scalez),
		float3(-scalex, scaley, -scalez),
		
		float3(-scalex, scaley, -scalez),
		float3(scalex, scaley, scalez),
		float3(scalex, scaley, -scalez),
		
		float3(-scalex, -scaley, -scalez),
		float3(scalex, -scaley, -scalez),
		float3(-scalex, -scaley, scalez),
		
		float3(-scalex, -scaley, scalez),
		float3(scalex, -scaley, -scalez),
		float3(scalex, -scaley, scalez),
	};

	params.SetSize(sizeof(float3) * 36);
	data.SetData(pos);
	if (!m_pVertices->Create(&params, &data, 36, sizeof(float3)))
	{
		assert(0 && "Error creating vertex buffer in Box Mesh initialization!");
	}
#pragma endregion

	#pragma region Texture Coordinates
	float2 texCoords[] =
	{
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f),
		float2(0.0f, 1.0f),

		float2(0.0f, 1.0f),
		float2(1.0f, 0.0f),
		float2(1.0f, 1.0f),
		
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f),
		float2(0.0f, 1.0f),
		
		float2(0.0f, 1.0f),
		float2(1.0f, 0.0f),
		float2(1.0f, 1.0f),
		
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f),
		float2(0.0f, 1.0f),
		
		float2(0.0f, 1.0f),
		float2(1.0f, 0.0f),
		float2(1.0f, 1.0f),
		
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f),
		float2(0.0f, 1.0f),
		
		float2(0.0f, 1.0f),
		float2(1.0f, 0.0f),
		float2(1.0f, 1.0f),
		
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f),
		float2(0.0f, 1.0f),
		
		float2(0.0f, 1.0f),
		float2(1.0f, 0.0f),
		float2(1.0f, 1.0f),
		
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f),
		float2(0.0f, 1.0f),
		
		float2(0.0f, 1.0f),
		float2(1.0f, 0.0f),
		float2(1.0f, 1.0f),
	};

	params.SetSize(sizeof(float2) * 36);
	data.SetData(texCoords);
	if (!m_pTexCoords->Create(&params, &data, 36, sizeof(float2)))
	{
		assert(0 && "Error creating vertex buffer with texture coordinates in Box Mesh initialization!");
	}

#pragma endregion

	#pragma region Normal Data

	NormalData normalData[36];

	//generate normals data for each face
	for (int i = 0; i < 12; i++)
	{
		float3 normal;
		float3 tangent;
		float3 binormal;

		CreateNormalTangentBinormal(&pos[i*3], &texCoords[i*3], normal, tangent, binormal);

		//fill the data for vertex buffer
		normalData[i*3+2].normal = normal;
		normalData[i*3+2].tangent = tangent;
		normalData[i*3+2].binormal = binormal;

		normalData[i*3+1].normal = normal;
		normalData[i*3+1].tangent = tangent;
		normalData[i*3+1].binormal = binormal;

		normalData[i*3].normal = normal;
		normalData[i*3].tangent = tangent;
		normalData[i*3].binormal = binormal;
	}

	params.SetSize(sizeof(NormalData) * 36);
	data.SetData(normalData);
	if (!m_pNormals->Create(&params, &data, 36, sizeof(NormalData)))
	{
		assert(0 && "Error creating vertex buffer with Normal data in Box Mesh initialization!");
	}

#pragma endregion

	//initialize input layout 
	/*INPUT_LAYOUT layout[] =
	{
		{ "POSITION", 0, TEX_R32G32B32_FLOAT, 0, 0,  IA_PER_VERTEX_DATA, 0},
		{ "TEXCOORDS", 0, TEX_R32G32_FLOAT,	  1, 0,  IA_PER_VERTEX_DATA, 0},
		{ "NORMAL",   0, TEX_R32G32B32_FLOAT, 2, 0,  IA_PER_VERTEX_DATA, 0},
		{ "TANGENT", 0, TEX_R32G32B32_FLOAT, 2, 12, IA_PER_VERTEX_DATA, 0},
		{ "BINORMAL",  0, TEX_R32G32B32_FLOAT, 2, 24, IA_PER_VERTEX_DATA, 0},
	};

	//Initialize shaders
	m_pShaders->VSetVertexShader(L"GBufferShader.hlsl", "VS", layout, 5, TOPOLOGY_TRIANGLELIST);
	m_pShaders->VSetPixelShader(L"GBufferShader.hlsl", "PS"); */
}

AVOID BoxMesh::VPreRender(Renderer* pRenderer, const Mat4x4 & view, const Mat4x4 & viewprojection)
{
	Mesh::VPreRender(pRenderer, view, viewprojection);

	pRenderer->AnisotropySampler16()->Set(0, ST_Pixel);
}
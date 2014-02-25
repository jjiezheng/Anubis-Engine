//====================================================================================
//TextResource.cpp
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
#include "TextResource.h"

#include "../../Math/Source/Vectors.h"

#include "Final\Resources\Buffers\VertexBuffer.h"
#include "Final\Resources\Buffers\IndexBuffer.h"
#include "Final\Resources\Buffers\BufferParams.h"
#include "Final\\Resources\ResourceData.h"
#include "../Meshes/Meshes.h"

using namespace Anubis;
using namespace std;

Vec FT_Vector_to_Vec(const FT_Vector & vec)
{
	return Vector(vec.x, vec.y, 0, 1);
}

TextResourceExtraData::TextResourceExtraData()
{
}

TextResourceLoader::TextResourceLoader()
{
	FT_Error error = FT_Init_FreeType(&m_library);
	if (!m_library)
	{
		assert(0 && error);
	}
}

AUINT32 TextResourceLoader::VGetLoadedResourceSize(ACHAR *pRawBuffer, AUINT32 uRawSize)
{
	return 10; 
}

ABOOL TextResourceLoader::VLoadResource(ACHAR *pRawBuffer, unsigned int uRawSize, shared_ptr<ResHandle> pHandle)
{
	std::shared_ptr<TextResourceExtraData> pExtra = std::shared_ptr<TextResourceExtraData>(new TextResourceExtraData());
	pHandle->SetExtra(pExtra);
	if (!ParseTtf(pRawBuffer, uRawSize, pHandle))
	{
		return false;
	}
	return true;
}

ABOOL TextResourceLoader::ParseTtf(ACHAR *pStream, size_t bufferLength, shared_ptr<ResHandle> pHandle)
{
	std::shared_ptr<TextResourceExtraData> pExtra = std::static_pointer_cast<TextResourceExtraData>(pHandle->GetExtra());

	FT_Face face;

	FT_Error error = FT_New_Face( m_library, pStream, 0, &face);
	if (error == FT_Err_Unknown_File_Format)
	{
		assert(0 && "The font file was opened andread but it seems its font format is unsupported");
	}
	else if(error)
	{
		assert(0 && "The font file couldn't be opened or read ot it's broken");
	}

	//set glyph size
	AINT32 h = 32;
	FT_Set_Char_Size(face, h, h, 96, 96);

	//load glyphs
	//error = FT_Load_Glyph(face, 'e', FT_LOAD_DEFAULT);
	//error = FT_Load_Glyph(face, 'T', FT_LOAD_DEFAULT);
	error = FT_Load_Glyph(face,  FT_Get_Char_Index(face, 'T'), FT_LOAD_DEFAULT);
	if (error)
	{
		assert(0 && "Some error loading glyph!");
	}

	FT_GlyphSlot glyph = face->glyph;
	FT_Outline outline = glyph->outline;

	CreateMesh(pExtra, outline);

	/*AUINT32 num_points = outline.n_points;
	AUINT32 num_contours = outline.n_contours;

	Vec point;
	for (auto i = 0; i < num_points; i++)
	{
		point = FT_Vector_to_Vec(outline.points[i]);
	} */

	return true;
}

AVOID TextResourceLoader::CreateMesh(std::shared_ptr<TextResourceExtraData> pResource, FT_Outline outline)
{
	AUINT32 num_points = outline.n_points;
	AUINT32 num_contours = outline.n_contours;

	Vec point;
	FT_Tag tag;
	/*for (auto i = 0; i < num_points; i++)
	{
		point = FT_Vector_to_Vec(outline.points[i]);
		tag = outline.tags[i];
	} */
	vector<float3> vertices;
	vector<float2> texCoords;
	vector<AUINT32> indexes;
	AUINT32 num_vertices = 0;

	vector<float3> vertices_convex;
	vector<float2> texCoords_convex;
	vector<AUINT32> indexes_convex;
	AUINT32 num_vertices_convex = 0;

	vector<float3> vertices_concave;
	vector<float2> texCoords_concave;
	vector<AUINT32> indexes_concave;
	AUINT32 num_vertices_concave = 0;

	struct Vertex 
	{
		Vec point;
		FT_Tag tag;
	};
	Vertex first;
	Vertex second;
	Vertex third;
	Vertex forth;

	AUINT32 on_curve_points = 0;
	AUINT32 conic_points = 0;
	AUINT32 cubic_points = 0;

	for(auto i = 0; i < num_points; i++)
	{
		first.point = FT_Vector_to_Vec(outline.points[i]);
		first.tag = outline.tags[i];

		/*second.point = FT_Vector_to_Vec(outline.points[i+1]);
		second.tag = outline.tags[i+1];

		auto third_index = (i+2) >= num_points ? 0 : i+2;
		third.point = FT_Vector_to_Vec(outline.points[third_index]);
		third.tag = outline.tags[third_index]; */

		if (first.tag == FT_CURVE_TAG_ON)
		{
			//just connect line segments
			vertices.push_back(float3(0,0,0));
			on_curve_points++;
		}
		else if (first.tag == FT_CURVE_TAG_CONIC)
		{
			conic_points++;
		}
		else if (first.tag == FT_CURVE_TAG_CUBIC)
		{
			cubic_points++;
		}
	}

	AUINT32 overall_points = 4;
}
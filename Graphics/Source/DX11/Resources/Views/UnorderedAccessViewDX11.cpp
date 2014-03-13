//====================================================================================
//UnorderedAccessViewDX11.cpp
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

#include "Graphics_pch.h"
#include "../../../Final/General.h"
#include "UnorderedAccessViewDX11.h"
//#include "../Buffers/StructuredBufferDX11.h"

using namespace Anubis;

ABOOL UnorderedAccessViewParamsDX11::InitForTexture2D(	AUINT8 format,
														AUINT16 mipslice )
{
	Format = static_cast<DXGI_FORMAT>(format);
	Texture2D.MipSlice = mipslice;
	ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

	return true;
}

ABOOL UnorderedAccessViewParamsDX11::InitForStructuredBuffer( AUINT8 format, AUINT32 elementOffset, AUINT32 numElements, D3D11_BUFFER_UAV_FLAG flag)
{
	Format = static_cast<DXGI_FORMAT>(format);
	Buffer.FirstElement = elementOffset;
	Buffer.NumElements = numElements;
	Buffer.Flags = flag;
	ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

	return true;
}

AVOID UnorderedAccessViewDX11::Set(AUINT16 slot, const AUINT32 * pUAVInitialCounts, ShaderType type)
{
	switch (type)
	{
	case ST_Pixel:
		//D3D11DeviceContext()->PSSetUnorderedAccessViews(slot, 1, &view.m_pView);
		ID3D11RenderTargetView* pNullRTV;
		D3D11DeviceContext()->OMSetRenderTargetsAndUnorderedAccessViews(0, &pNullRTV, nullptr, 0, 1, &m_pView, pUAVInitialCounts);
		break;
	case ST_Compute:
		D3D11DeviceContext()->CSSetUnorderedAccessViews(slot, 1, &m_pView, pUAVInitialCounts);
		break;
	default:
		ASSERT(0);
		break;
	};
}
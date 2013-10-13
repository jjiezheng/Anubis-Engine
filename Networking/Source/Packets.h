#pragma once
//====================================================================================
//Packets.h
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

namespace Anubis
{
	/*** Interface class for packets in the game ***/
	class IPacket
	{
	public:

		AVIRTUAL u_long	VGetSize() const = 0;
		AVIRTUAL ACHAR*	VGetData() const = 0;
		AVIRTUAL AVOID	VMemCpy(ACHAR const * const pData, size_t size, AINT32 offset);
	};

	///////////////////////////////////////////
	//Network packet with binary data
	///////////////////////////////////////////
	class BinaryPacket : public IPacket
	{
	public:

		//Constructors
		AINLINE BinaryPacket(const u_long size);
		AINLINE BinaryPacket(ACHAR const * const pData, const u_long size);

		//Destructor
		AVIRTUAL ~BinaryPacket() { SAFE_DELETE(m_pData); }

		AINLINE u_long	VGetSize() const { return ntohl(*(u_long*)m_pData); }
		AINLINE ACHAR*	VGetData() const { return m_pData; }
		AINLINE AVOID	VMemCpy(ACHAR const * const pData, size_t size, AINT32 offset);

	protected:

		//packet data in network order
		ACHAR* m_pData;
	};

	BinaryPacket::BinaryPacket(const u_long size)
	{
		m_pData = new ACHAR[sizeof(u_long) + size];

		//write size of the packet
		*(u_long*)m_pData = htonl(sizeof(u_long) + size);
	}

	BinaryPacket::BinaryPacket(ACHAR const * const pData, const u_long size)
	{
		m_pData = new ACHAR[sizeof(u_long) + size];

		//write size of the packet
		*(u_long*)m_pData = htonl(sizeof(u_long) + size);

		//copy data to the packet
		memcpy(m_pData+sizeof(u_long), pData, size);
	}

	AVOID BinaryPacket::VMemCpy(ACHAR const * const pData, size_t size, AINT32 offset)
	{
		assert(VGetSize() >= (size+offset+sizeof(u_long)));
		memcpy(m_pData+offset+sizeof(u_long), pData, size);
	}

}; //Anubis
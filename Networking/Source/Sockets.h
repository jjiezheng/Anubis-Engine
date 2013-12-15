#pragma once
//====================================================================================
//Sockets.h
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

#define MAX_PACKET_SIZE		256
#define RECV_BUFFER_SIZE	(MAX_PACKET_SIZE * 512)

#include "Packets.h"

namespace Anubis
{
	class Socket
	{
		friend class SocketManager;

		typedef std::list<shared_ptr<IPacket> > Packets;

	public:

		Socket(SOCKET new_sock, AUINT32 hostID);
		AVIRTUAL ~Socket();

		ABOOL Connect(AUINT32 ip, AUINT32 port, ABOOL forceCoalesce = 0);
		AVOID SetBlocking(ABOOL blocking);
		AVOID Send(shared_ptr<IPacket> pPkt, ABOOL clearTimeOut=1);

		AVIRTUAL AINT32 VHasOutput() { return !m_OutList.empty(); }
		AVIRTUAL AVOID VHandleOutput();
		AVIRTUAL AVOID VHandleInput();
		AVIRTUAL AVOID VTimeOut() { m_timeOut = 0; }
		AVOID HandleException() { m_deleteFlag |= 1; }
		AVOID SetTimeOut(AUINT32 ms=45*1000) { m_timeOut = timeGetTime() + ms; }
		AINT32 SetIpAddress() { return m_ipaddr; }

	protected:
		SOCKET m_sock;	//socket hanble
		AINT32 m_id;	//unique socket identifier

		AINT32 m_deleteFlag;

		Packets m_OutList; //packets to send
		Packets m_InList; //packets just received

		ACHAR m_recvBuf[RECV_BUFFER_SIZE]; //receive buffer
		AUINT32 m_recvOfs, m_recvBegin;

		AINT32 m_sendOfs;
		AUINT32 m_timeOut;
		AUINT32 m_ipaddr; //ip address to the remote connection

		AINT32 m_internal; //is the remote ip internal or external
		AINT32 m_timeCreated; //time when the socket was created
	};

	//allows to listen for client conections and accept them
	class ListenSocket : public Socket
	{
	public:
		ListenSocket() {};
		ListenSocket(AINT32 portnum) { m_port = 0; Init(portnum); }

		AVOID Init(AINT32 portnum);
		SOCKET AcceptConnection(AUINT32 * pAddr);

		AUINT32 m_port;
	};
}; //Anubis
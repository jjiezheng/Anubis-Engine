//====================================================================================
//Sockets.cpp
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

#include "Networking_pch.h"
#include "Sockets.h"

using namespace Anubis;

Socket::Socket()
{
	m_sock = INVALID_SOCKET;
	m_deleteFlag = 0;
	m_sendOfs = 0;
	m_timeOut = 0;
	m_recvOfs = m_recvBegin = 0;
	m_internal = 0;
	m_bBinaryProtocol = 1;
}

Socket::Socket(SOCKET new_sock, AUINT32 hostIP)
{
	m_deleteFlag = 0;
	m_sendOfs = 0;
	m_timeOut = 0;
	m_recvOfs = m_recvBegin = 0;
	m_internal = 0;

	//check the time
	m_timeCreated = timeGetTime();

	m_sock = new_sock;
	m_ipaddr = hostIP;

	//check if the socket is on internal network
	m_internal = g_pSocketManager->IsInternal(m_ipaddr);

	setsockopt(m_sock, SOL_SOCKET, SO_DONTLINGER, NULL, 0);
}

Socket::~Socket()
{
	if (m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}
}

ABOOL Socket::Connect(AUINT32 ip, AUINT32 port, ABOOL forceCoalesce)
{
	struct socketaddr_in sa;
	AINT32 x = 1;

	//create the socket handle
	if ((m_sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		return false;

	//set socket options
	if (!forceCoalesce)
		setsockopt(m_sock, IPPROTO_TCP, TCP_NODELAY, (ACHAR*)&x, sizeof(x));

	//set ip address and port of the server and try to connect
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(ip);
	sa.sin_port = htons(port);

	if (connect(m_sock, (struct sockaddr*)&sa, sizeof(sa)))
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		return false;
	}

	return true;
}

AVOID Socket::SetBlocking(ABOOL blocking)
{
	u_long val = blocking ? 0 : 1;
	ioctlsocket(m_sock, FIONBIO, &val);
}

AVOID Socket::Send(shared_ptr<IPacket> pPkt, ABOOL clearTimeOut)
{
	if (clearTimeOut)
		m_timeOut = 0;
	m_OutList.push_back(pPkt);
}

AVOID Socket::VHandleOutput()
{
	AINT32 fSent = 0;
	do
	{
		assert(!m_OutList.empty());
		Packets::iterator it = m_OutList.begin();

		shared_ptr<IPacket> pPkt = *i;
		const ACHAR *buf = pPkt->VGetData();
		AINT32 len = static_cast<AINT32>(pPkt->VGetSize());

		AINT32 rc = send(m_sock, buf+m_sendOfs, len-m_sendOfs, 0);
		if (rc > 0)
		{
			g_pSocketManager->AddToOutbound(rc);
			m_sendOfs += rc;
			fSent = 1;
		}
		else if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			HandleException();
			fSent = 0;
		}
		else
		{
			fSent = 0;
		}

		if (m_sendOfs == pPkt->VGetSize())
		{
			m_OutList.pop_front();
			m_sendOfs = 0;
		}
	} while( fSent && !m_OutList.empty() );
}

AVOID Socket::VHandleInput()
{
	ABOOL bPktReceived = false;
	u_long packetSize = 0;

	AINT32 rc = recv(m_sock, m_recvBuf+m_recvBegin+m_recvOfs,
		RECV_BUFFER_SIZE-(m_recvBegin+m_recvOfs), 0);

	if (rc == 0)
		return;

	if (rc == SOCKET_ERROR)
	{
		m_deleteFlag = 1;
		return;
	}

	const AINT32 hdrSize = sizeof(u_long);
	AUINT32 newData = m_recvOfs+rc;
	AINT32 processedData = 0;

	while (newData > hdrSize)
	{
		packetSize = *(reinterpret_cast<u_long*>(m_recvBuf+m_recvBegin));
		packetSize = ntohl(packetSize);

		//don't have enough new data to grag the next packet
		if (newData < packetSize)
			break;

		if (packetSize > MAX_PACKET_SIZE)
		{
			//prevent buffer overruncs
			HandleException();;
			return;
		}

		if (newData >= packetSize)
		{
			shared_ptr<BinaryPacket> pPkt(new BinaryPacket(&m_recvBuf[m_recvBegin+hdrSize], packetSize-hdrSize));
			m_InList.push_back(pPkt);
			bPktReceived = true;
			processedData += packetSize;
			newData -= packetSize;
			m_recvBegin += packetSize;
		}
	}

	g_pSocketManager->AddToInbound(rc);
	m_recvOfs = newData;

	if (bPktReceived)
	{
		if (m_recvOfs == 0)
		{
			m_recvBegin = 0;
		}
		else if (m_recvBegin + m_recvOfs + MAX_PACKET_SIZE > RECV_BUFFER_SIZE)
		{
			//do not overrun the buffer - copy leftover bits
			//to the beginning
			AINT32 leftover = m_recvOfs;
			memcpy(m_recvBuf, &m_recvBuf[m_recvBegin], m_recvOfs);
			m_recvBegin = 0;
		}
	}
}

////////////////////////////////////////////////
//Listen Socket
////////////////////////////////////////////////

AVOID ListenSocket::Init(AINT32 portnum)
{
	struct sockaddr_in sa;
	AINT32 value = 1;

	//create socket handle
	if ((m_sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		assert(0 && "Failed to create socket handle");
	}

	//set socket options ti reuse server addresses even is they are busy
	if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR,
		(ACHAR*)&value, sizeof(value)) == SOCKET_ERROR)
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		assert(0);
	}

	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = ADDR_ANY;
	sa.sin_port = htons(portnum);

	//bind to port]
	if (bind(m_sock, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		assert(0);
	}

	//non-blocking
	SetBlocking(false);

	//start listening
	if (listen(m_sock, 256) == SOCKET_ERROR)
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		assert(0);
	}

	m_port = portnum;
}

SOCKET ListenSocket::AcceptConnection(AUINT32* pAddr)
{
	SOCKET new_sock;
	struct sockaddr_in sock;
	AINT32 size = sizeof(sock);

	if ((new_sock = accept(m_sock, (struct sockaddr*)&sock, &size)) == INVALID_SOCKET)
		return INVALID_SOCKET;

	if (getpeername(new_sock, (struct sockaddr*)&sock, &size) == SOCKET_ERROR)
	{
		closesocket(new_sock);
		return INVALID_SOCKET;
	}

	*pAddr = ntohl(sock.sin_addr.s_addr);
	return new_sock;
}
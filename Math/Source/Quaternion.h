#pragma once
//====================================================================================
//Quaternion.h
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

#include "Defines.h"
#include "Mat4x4.h"

namespace Anubis
{
	//typedef __m128	Quaternion;
	struct Quaternion
	{
		/**								==
				Static methods
		==								**/
		ASTATIC Quaternion Identity()
		{
			Quaternion q;
			q = Quaternion(0, 0, 0, 1);

			return q;
		}

		//data
		Vec	m_q;
		
		Quaternion();
		Quaternion(const AREAL x, const AREAL y, const AREAL z, const AREAL w);
		Quaternion(const Vec & v);

		// ==================
		//	Operators
		// ==================
		Quaternion & operator+(const Quaternion & q) const;
		Quaternion & operator-(const Quaternion & q) const;
		//Quaternion operator*(const Quaternion & q) const; 
		Quaternion & operator*(const AREAL s) const
		{
			Quaternion res = *this;
			res.m_q = res.m_q * s;

			return res;
		}

		Quaternion & operator/(const AREAL s) const
		{
			Quaternion res = *this;
			res.m_q = res.m_q / s;

			return res;
		}

		// ==================
		//  Accessors
		// ==================
		AREAL GetX() const { return m_q.x; }
		AREAL GetY() const { return m_q.y; }
		AREAL GetZ() const { return m_q.z; }
		AREAL GetW() const { return m_q.w; }
		Vec & GetAxis() const { return Vector(m_q.x, m_q.y, m_q.z, 0.0f); }
		AREAL GetAngle() const { return m_q.w; }

		// ==================
		//  Methods
		// ==================
		Quaternion & Normalize();
		AREAL Norm() const;
		AREAL Dot(Quaternion & q) const;

		Mat4x4 ToMat4x4() const;
	};

}; //Anubis
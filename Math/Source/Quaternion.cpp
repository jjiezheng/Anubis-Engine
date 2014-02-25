//====================================================================================
//Quaternion.cpp
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

#include "Math_pch.h"
#include "Quaternion.h"

using namespace Anubis;

Quaternion::Quaternion()
{
	m_q = std::move(Vector(0, 0, 0, 1));
}

Quaternion::Quaternion(const AREAL x, const AREAL y, const AREAL z, const AREAL w)
{
	m_q = std::move(Vector(x, y, z, w));
}

Quaternion::Quaternion(const Quaternion & other)
{
	m_q = other.m_q;
}

Quaternion::Quaternion(Quaternion && other)
{
	m_q = std::move(other.m_q);
}

Quaternion::Quaternion(const Vec & v)
{
	m_q = v;
}

//----------------------------------------------------
//Operators
//----------------------------------------------------
Quaternion & Quaternion::operator=(const Quaternion & other)
{
	if (this != &other)
	{
		m_q = other.m_q;
	}

	return *this;
}

Quaternion & Quaternion::operator=(Quaternion && other)
{
	if (this != &other)
	{
		m_q = std::move(other.m_q);
	}

	return *this;
}

const Quaternion Quaternion::operator+(const Quaternion & q) const
{
	//return _mm_add_ps(v1, v2);
	return Quaternion(m_q + q.m_q);
}

const Quaternion Quaternion::operator-(const Quaternion & q) const
{
	return Quaternion(m_q - q.m_q);
}

const Quaternion Quaternion::operator*(const AREAL s) const
{
	return Quaternion(m_q.x*s, m_q.y*s, m_q.z*s, m_q.w*s);
}

const Quaternion Quaternion::operator/(const AREAL s) const
{
	return Quaternion(m_q.x/s, m_q.y/s, m_q.z/s, m_q.w/s);
}

//----------------------------------------------------
//Methods
//----------------------------------------------------
AREAL Quaternion::Norm() const
{
	return Sqrt(m_q.x * m_q.x + m_q.y * m_q.y + m_q.z * m_q.z + m_q.w * m_q.w);
}

Quaternion & Quaternion::Normalize()
{
	m_q = m_q / Norm();
	return *this;
}

AREAL Quaternion::Dot(Quaternion & q) const
{
	return (m_q.x * q.m_q.x + m_q.y * q.m_q.y + m_q.z * q.m_q.z + m_q.w * q.m_q.w);
}

Mat4x4 Quaternion::ToMat4x4() const
{
	float xx = m_q.x * m_q.x, yy = m_q.y * m_q.y, zz = m_q.z * m_q.z;
	float xy = m_q.x * m_q.y, xz = m_q.x * m_q.z, yz = m_q.y * m_q.z;
	float wx = m_q.x * m_q.w, wy = m_q.y * m_q.w, wz = m_q.z * m_q.w;

    Mat4x4 m;
	m.rows[0].x = 1.f - 2.f * (yy + zz);
	m.rows[0].y =       2.f * (xy + wz);
	m.rows[0].z =       2.f * (xz - wy);
	m.rows[1].x =       2.f * (xy - wz);
	m.rows[1].y = 1.f - 2.f * (xx + zz);
	m.rows[1].z =       2.f * (yz + wx);
	m.rows[2].x =       2.f * (xz + wy);
	m.rows[2].y =       2.f * (yz - wx);
	m.rows[2].z = 1.f - 2.f * (xx + yy);

	m.Transpose();
	return m;
}
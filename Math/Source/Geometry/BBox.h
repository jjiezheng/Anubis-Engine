#pragma once
//====================================================================================
//BBox.h
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

#include "../../Math/Source/Math.h"
#include "Ray.h"

namespace Anubis
{
	class BBox
	{
	private:
		Vec m_min;
		Vec m_max;

	public:

		BBox()
		{
			m_min = Vec(-INFINITY, -INFINITY, -INFINITY);
			m_max = Vec(INFINITY, INFINITY, INFINITY);
		}
		BBox(const Vec & p) : m_min(p), m_max(p) {}
		BBox(const Vec & p1, const Vec & p2)
		{
			m_min = Vec(min(p1.x, p2.y), min(p1.y, p2.y), min(p1.z, p2.z));
			m_max = Vec(max(p1.x, p2.y), max(p1.y, p2.y), max(p1.z, p2.z));
		}

		ABOOL overlaps(const BBox & b) const
		{
			ABOOL x = (m_max.x >= b.m_min.x) && (m_min.x <= b.m_max.x);
			ABOOL y = (m_max.y >= b.m_min.y) && (m_min.y <= b.m_max.y);
			ABOOL z = (m_max.z >= b.m_min.z) && (m_min.z <= b.m_max.z);
			return (x && y && z);
		}

		ABOOL Inside(const Vec &pt) const 
		{
			return (pt.x >= m_min.x && pt.x <= m_max.x &&
					pt.y >= m_min.y && pt.y <= m_max.y &&
					pt.z >= m_min.z && pt.z <= m_max.z);
		}

		void Expand(AREAL delta) 
		{
			m_min -= Vec(delta, delta, delta);
			m_max += Vector(delta, delta, delta);
		}

		AREAL SurfaceArea() const 
		{
			Vec d = m_max - m_min;
			return 2.f * (d.x * d.y + d.x * d.z + d.y * d.z);
		}

		AREAL64 Volume() const 
		{
			Vec d = m_max - m_min;
			return d.x * d.y * d.z;
		}

		ABOOL Intersect(const Ray & ray, AREAL* hit0 = nullptr, AREAL hit1 = nullptr) const
		{
			AREAL t0 = ray.m_minT;
			AREAL t1 = ray.m_maxT;

			for(auto i = 0; i < 3; i++)
			{
				AREAL invRayDir, tNear, tFar;
				switch(i)
				{
				case 0:
					invRayDir = 1.f / ray.m_direction.x;
					tNear = (m_min.x - ray.m_origin.x) * invRayDir;
					tFar  = (m_max.x - ray.m_origin.x) * invRayDir;
					break;
				case 1:
					invRayDir = 1.f / ray.m_direction.y;
					tNear = (m_min.y - ray.m_origin.y) * invRayDir;
					tFar  = (m_max.y - ray.m_origin.y) * invRayDir;
					break;
				case 2:
					invRayDir = 1.f / ray.m_direction.z;
					tNear = (m_min.z - ray.m_origin.z) * invRayDir;
					tFar  = (m_max.z - ray.m_origin.z) * invRayDir;
					break;
				};

				if (tNear > tFar) swap(tNear, tFar);
				t0 = tNear > t0 ? tNear : t0;
				t1 = tFar  < t1 ? tFar  : t1;
				if (t0 > t1) return false;
			}

			if (hit0) *hit0 = t0;
			if (hit1) *hit1 = t1;
			return true;
		}

		friend BBox Union(const BBox &b, const Vec &p) 
		{
			BBox ret = b;
			ret.m_min.x = min(b.m_min.x, p.x);
			ret.m_min.y = min(b.m_min.y, p.y);
			ret.m_min.z = min(b.m_min.z, p.z);
			ret.m_max.x = max(b.m_max.x, p.x);
			ret.m_max.y = max(b.m_max.y, p.y);
			ret.m_max.z = max(b.m_max.z, p.z);
			return ret;
		}


		friend BBox Union(const BBox &b, const Vec &b2) 
		{
			BBox ret;
			ret.m_min.x = min(b.m_min.x, b2.m_min.x);
			ret.m_min.y = min(b.m_min.y, b2.m_min.y);
			ret.m_min.z = min(b.m_min.z, b2.m_min.z);
			ret.m_max.x = max(b.m_max.x, b2.m_max.x);
			ret.m_max.y = max(b.m_max.y, b2.m_max.y);
			ret.m_max.z = max(b.m_max.z, b2.m_max.z);
			return ret;
		}
	};

}; //Anubis
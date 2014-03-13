#pragma once
//====================================================================================
//DualNumbers.h
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

#include <cmath>

using namespace std;

namespace Anubis
{
	template<typename T>
	class Dual
	{
	public:
		T real() const { return m_real; }
		T dual() const { return m_dual; }

		template<typename T>
		Dual(T re = T(), T du = T()) : m_real(re), m_dual(du) 
		{
		}

		template<typename T>
		Dual(const Dual<T> & other)
		{
			m_real = other.real();
			m_dual = other.dual();
		}

		template<typename T>
		Dual(Dual<T> && other)
		{
			m_real = std::move(other.m_real);
			m_dual = std::move(other.m_dual);
		}

	private:
		T m_real;
		T m_dual;
	};

	template<typename T>
	Dual<T> operator*(const Dual<T> & a, const Dual<T> & b)
	{
		return Dual<T>{a.real() * b.real(), a.real() * b.dual() + a.dual() * b.real()};
	}

	template<typename T>
	Dual<T> sqrt(Dual<T> & d)
	{
		T x = sqrt(d.real());
		return Dual<T>{x, d.dual()*T(0.5)/x};
	}


}; //Anubis
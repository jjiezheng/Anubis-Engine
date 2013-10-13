#pragma once
//========================================================================
// RenderingDemo_pch.h
//
// This code is part of Minotower Games Engine 1.0v
//
// (c) Copyright 2013 Muralev Evgeny
//========================================================================

#include <Windows.h>
#include <memory>
#include <d3d11.h>

#include <assert.h>

#include <vector>
#include <list>
#include <map>

// DirectSound includes
#include <dsound.h>
#include <mmsystem.h>

#include "Anubis.h"

using namespace std;
using namespace Anubis;

#define ADX11_API

//define game states
#define GS_BOXTEST  1
#define GS_HOMETEST 2

#define VIEW_DISTANCE 500.0f

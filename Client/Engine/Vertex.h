#pragma once


#include "EnginePch.h"

struct Vertex
{
	Vec3 pos;
	Vec3 normal;
	Vec2 uv;
};



#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
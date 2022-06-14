#pragma once



#include <d3dx9.h>
#include <d3dx9math.h>
#include <d3d9.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )

#include "CoreGlobal.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

using Vec2 = D3DXVECTOR2;
using Vec3 = D3DXVECTOR3;
using Vec4 = D3DXVECTOR4;
using Matrix = D3DXMATRIXA16;
using Color = D3DXCOLOR;

#include "StdPch.h"

#ifndef _IMGUI_
#define _IMGUI_
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"
#endif

#ifndef _FMOD_
#define _FMOD_
#include "Fmod/fmod.h"
#include "Fmod/fmod.hpp"
#pragma comment(lib, "Fmod/fmod_vc.lib")
#endif

#include "Utils.h"

/* ------ Reflection ------ */
#include "TypeInfo.h"
#include "Method.h"

/* Reflection 추가할때 concept이 들어가기때문에 20버전으로 변경되었습니다. */

#include "Macro.h"
#include "type.h"



// DX_Park.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "pch.h"
#include "Core.h"

#define MAX_LOADSTRING 100

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    return Core::GetI()->Run(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}


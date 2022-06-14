#pragma once

/* ------------------------------------
*	자주쓰이는 객체는 Engine를 참조하면
*	같이 사용할 수 있게 할려고 합니다.
------------------------------------ */
#include "Timer.h"
#include "Input.h"
#include "Device.h"
#include "SceneManager.h"
#include "ResourcesManager.h"


/* ------------------
		Engine
------------------ */

struct WindowInfo
{
public:
	int32		width = 1280;
	int32		height = 640;

	D3DXCOLOR	color = D3DCOLOR_XRGB(0, 0, 0);

	HWND		hWnd = nullptr;			// 윈도우 핸드
	HINSTANCE	hInst = nullptr;	// Inst
	wstring		title	= L"Project_DX9";
};

class Engine
{
	DECLARE_SINGLE(Engine);
public:
	/* ------------ Function --------------- */
	int32 Init(HWND hWnd, HINSTANCE hInstance);
	int32 Update();
	int32 Render();
	int32 End();


	void ResizeWindow(int32 width, int32 height);
	void SetWindowTitle(const wstring& title);
	WindowInfo& GetInfo() { return m_info; }

private:
	/* ------------ Variable --------------- */
	WindowInfo m_info;

};


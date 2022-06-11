#pragma once

#include <string>

using namespace std;

/* --------------------------
*			Core
*	간단한 Window함수들 재정의
-------------------------- */

class Core
{
public:
	static Core* GetI()
	{
		static Core instance;
		return &instance;
	}

private:
	ATOM MyRegisterClass(HINSTANCE hInstance);
	BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	void Init();
	void Update();
	void Render();
	void End();

	int Run(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow);

private:
	wstring m_strTitle = L"Project";
	HINSTANCE m_hInst;
	HWND m_hWnd;

public:
	static bool loop;
};

//Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


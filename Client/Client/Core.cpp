#include "pch.h"
#include "Core.h"
#include "StartScene.h"
#include "TextureScene.h"
#include "MeshScene.h"
#include "ServerScene.h"

bool Core::loop = true;

void Core::Init()
{
#pragma region DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //_CrtSetBreakAlloc(35632);
    //_CrtSetBreakAlloc(35169);
    //_CrtSetBreakAlloc(231);
    //_CrtSetBreakAlloc(230);
    //_CrtSetBreakAlloc(229);
#pragma endregion

    Engine::GetI()->Init(m_hWnd, m_hInst);


    //GET_SINGLE(SceneManager)->CreateScene<TextureScene>(SCENE_TYPE::TEXTURE);
    //GET_SINGLE(SceneManager)->LoadScene(SCENE_TYPE::TEXTURE);

    //GET_SINGLE(SceneManager)->CreateScene<StartScene>(SCENE_TYPE::START);
    //GET_SINGLE(SceneManager)->LoadScene(SCENE_TYPE::START);

    GET_SINGLE(SceneManager)->CreateScene<ServerScene>(SCENE_TYPE::SERVER);
    GET_SINGLE(SceneManager)->LoadScene(SCENE_TYPE::SERVER);

}

void Core::Update()
{
    GET_SINGLE(Engine)->Update();

    GET_SINGLE(Timer)->ShowFpsWindowTitle();
}

void Core::Render()
{
    GET_SINGLE(Engine)->Render();
}

void Core::End()
{
    GET_SINGLE(Engine)->End();
}

int Core::Run(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow)) {
        return FALSE;
    }

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));

    Init();

    while (Core::loop) {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT) {
                return 0;
            }
        }
        else {
            Update();
            Render();
        }
    }

    End();

    return (int)msg.wParam;
}



ATOM Core::MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, 0);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = 0;
    wcex.lpszClassName = m_strTitle.c_str();
    wcex.hIconSm = LoadIcon(wcex.hInstance, 0);

    return RegisterClassExW(&wcex);
}

BOOL Core::InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    m_hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    m_hWnd = CreateWindowW(m_strTitle.c_str(), m_strTitle.c_str(), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!m_hWnd)
    {
        return FALSE;
    }

    ShowWindow(m_hWnd, nCmdShow);
    UpdateWindow(m_hWnd);

    return TRUE;
}

LRESULT CALLBACK Core::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)) {
        return true;
    }

    switch (message) {
    case WM_DESTROY:
        Core::loop = false;
        for (auto& func : GetI()->_vecFunc) {
            func();
        }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
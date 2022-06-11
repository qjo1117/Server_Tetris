#include "pch.h"
#include "Device.h"

bool Device::Init(HWND hWnd)
{
    if ((m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr) {
        return false;
    }

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    // Create the D3DDevice
    if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &d3dpp, &m_pd3dDevice)))
    {
        return false;
    }

    return true;
}

void Device::End()
{
    m_pD3D->Release();
    m_pd3dDevice->Release();

    m_pD3D = nullptr;
    m_pd3dDevice = nullptr;
}

#pragma once


/* --------------
*	Device
-------------- */

// 일반적으로 singleton보다는 객체로 쓰는것이 맞다고 생각합니다.
// 다만 모든 곳에서 접근할 수 있도록 하기 위해서 + 전역변수로 안쓰기위해서
// 싱글톤으로 제작하고 DEVICE를 통해서 접근하는 방식을 채택했습니다.

#define DEVICE GET_SINGLE(Device)->GetDevice()

class Device
{
	DECLARE_SINGLE(Device);
public:
	bool Init(HWND hWnd);
	void End();

	LPDIRECT3DDEVICE9 GetDevice() { return m_pd3dDevice; }

private:
	LPDIRECT3D9 m_pD3D = NULL; 
	LPDIRECT3DDEVICE9 m_pd3dDevice = NULL;
};


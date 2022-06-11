#pragma once


/* --------------
*	Device
-------------- */

// �Ϲ������� singleton���ٴ� ��ü�� ���°��� �´ٰ� �����մϴ�.
// �ٸ� ��� ������ ������ �� �ֵ��� �ϱ� ���ؼ� + ���������� �Ⱦ������ؼ�
// �̱������� �����ϰ� DEVICE�� ���ؼ� �����ϴ� ����� ä���߽��ϴ�.

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


#pragma once

#include "GameObject.h"

/* ----------------------
		Camera
---------------------- */

struct CameraInfo
{
	Vec3 look = Utils::Forward();		// 앞을 본다.
	Vec3 viewUp = Utils::Up();			// 이건 고정

	float Near = 1.0f;
	float Far = 10000.0f;
	float fov = D3DX_PI / 4.0f;
	float aspect = 1.0f;
};

class Camera : public GameObject
{
public:
	Camera();
	virtual ~Camera();
public:
	virtual void Awake();
	virtual void Update();
	virtual void FinalUpdate();

public:
	CameraInfo& GetInfo() { return m_info; }

	Matrix GetMatrixView() { return m_matView; }
	Matrix GetMatrixProjection() { return m_matProj; }

private:
	void ViewMatrixUpdate();
	void ProjMatrixUpdate();

private:
	Matrix m_matView = Utils::Identity;
	Matrix m_matProj = Utils::Identity;

	CameraInfo m_info;
};


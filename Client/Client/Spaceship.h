#pragma once

#include "GameObject.h"

enum SpacesParts
{
	HEAD = 0,
	BODY,
	RIGHT_HANDLE,
	LEFT_HANDLE,
	RIGHT_FOOT,
	LEFT_FOOT,
	SPACESPARTS_END,
};

enum PlayerMode
{
	One = 0,
	Quter
};

class Spaceship :
    public GameObject
{
public:
	virtual ~Spaceship();

public:
	virtual void Init() override;
	virtual void Start() override;
	virtual void Update() override;
	
public:
	void SetCamera(Ref<class Camera> p_pCamera);

private:
	void PlayerMove();
	void PlayerRotation();

	void CameraOneMode();
	void CameraQuterMode();

private:
	vector<Ref<GameObject>> m_vecParts;

	Ref<class Camera> m_pCamera = nullptr;

	POINT m_currentMouse = { 0,0 };
	POINT m_defferMouse = { 0,0 };

	int32 m_iMode = PlayerMode::One;

	bool m_bRotation = true;

	//GENERATE_CLASS_TYPE_INFO(Spaceship)
	//PUBLIC_PROPERTY(int, temp);
};


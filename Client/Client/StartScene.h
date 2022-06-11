#pragma once


#include "Scene.h"

class StartScene : public Scene
{
public:
	StartScene();
	virtual ~StartScene();
public:
	virtual void Awake() override;
	virtual void Update() override;
	virtual void End() override;

private:
	Ref<class Planet> CreatePlanat(const wstring& p_strName, const Vec3& pos,float scaling, float speed, const Color& color, const wstring& parent = L"");

private:
	Ref<class Light> m_pPointLight;
	Ref<class Spaceship> m_pPlayer;
};


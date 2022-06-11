#include "pch.h"
#include "Tiger.h"

void Tiger::Init()
{
	GameObject::Init();
	m_pMesh->Init(L"Tiger.x");
}

void Tiger::Start()
{
	GameObject::Start();
}

void Tiger::Update()
{
	GameObject::Update();
}

void Tiger::End()
{
	GameObject::End();
}

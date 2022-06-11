#include "pch.h"
#include "ChessQueen.h"

void ChessQueen::Init()
{
	GameObject::Init();
	m_pMesh->Init(L"ChessQueen.x");
	INPUT->AddKey("3", '3');
}

void ChessQueen::Start()
{
	GameObject::Start();


}

void ChessQueen::Update()
{
	GameObject::Update();

	if (INPUT->KeyDown("3") == true) {
		m_bMesh = !m_bMesh;
	}
}

void ChessQueen::End()
{
	GameObject::End();
}

#include "pch.h"
#include "ChessKing.h"

void ChessKing::Init()
{
	GameObject::Init();
	m_pMesh->Init(L"ChessKing.x");
	INPUT->AddKey("1", '1');
}

void ChessKing::Start()
{
	GameObject::Start();

}

void ChessKing::Update()
{
	GameObject::Update();
	if (INPUT->KeyDown("1")) {
		m_bMesh = !m_bMesh;
	}
}

void ChessKing::End()
{
	GameObject::End();
}

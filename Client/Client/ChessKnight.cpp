#include "pch.h"
#include "ChessKnight.h"

void ChessKnight::Init()
{
	GameObject::Init();
	m_pMesh->Init(L"ChessKnight.x");
	INPUT->AddKey("2", '2');
}

void ChessKnight::Start()
{
	GameObject::Start();

}

void ChessKnight::Update()
{
	GameObject::Update();
	if (INPUT->KeyDown("2")) {
		m_bMesh = !m_bMesh;
	}
}

void ChessKnight::End()
{
	GameObject::End();
}

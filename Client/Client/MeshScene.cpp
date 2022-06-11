#include "pch.h"
#include "MeshScene.h"
#include "Tiger.h"
#include "Light.h"
#include "Camera.h"
#include "ChessQueen.h"
#include "ChessKing.h"
#include "ChessKnight.h"

void MeshScene::Start()
{
	Scene::Start();

	//{
	//	m_pTiger = CreateGameObject<Tiger>();
	//	m_pTiger->SetName(L"Tiger");
	//}

#pragma region °úÁ¦
	{
		Ref<ChessQueen> actor = CreateGameObject<ChessQueen>();
		actor->SetName(L"ChessQueen");
		actor->GetTransform()->SetLocalScale(Utils::One() * 30.0f);
		actor->GetTransform()->SetLocalPosition(Vec3{ 4.0f,0.0f,0.0f });
	}

	{
		Ref<ChessKing> actor = CreateGameObject<ChessKing>();
		actor->SetName(L"ChessKing");
		actor->GetTransform()->SetLocalScale(Utils::One() * 30.0f);
		actor->GetTransform()->SetLocalPosition(Vec3{ -4.0f,0.0f,0.0f });
	}

	{
		Ref<ChessKnight> actor = CreateGameObject<ChessKnight>();
		actor->SetName(L"ChessKnight");
		actor->GetTransform()->SetLocalScale(Utils::One() * 30.0f);
		actor->GetTransform()->SetLocalPosition(Vec3{ 0.0f,0.0f,0.0f });
	}
#pragma endregion

	// Camera
	{
		Ref<Camera> camera;
		camera = CreateGameObject<Camera>();
		camera->Init();
		camera->SetName(L"MainCamera");
		camera->GetTransform()->SetLocalPosition(Vec3{ 0.0f, 1.0f, -20.0f });
		m_pCamera = camera;
	}

	// Directional
	{
		Ref<Light> light = CreateGameObject<Light>();
		light->SetName(L"Directional_Light_Down");
		light->GetLightInfo().type = D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL;
		light->GetLightInfo().direction = Utils::Down();
	}
}

void MeshScene::CameraMove()
{
	Vec3 move = Utils::Zero();
	float speed = 100.0f;

	auto target = m_pCamera->GetTransform();

	if (INPUT->KeyPress("Right")) {
		move += target->GetRight();
	}
	if (INPUT->KeyPress("Left")) {
		move -= target->GetRight();
	}
	if (INPUT->KeyPress("Forward")) {
		move += target->GetForward();
	}
	if (INPUT->KeyPress("Back")) {
		move -= target->GetForward();
	}
	if (INPUT->KeyPress("Up")) {
		move += target->GetUp();
	}
	if (INPUT->KeyPress("Down")) {
		move -= target->GetUp();
	}

	move *= speed * DELTATIME;

	target->Translation(move);
}


void MeshScene::Update()
{
	Scene::Update();
	CameraMove();
}

void MeshScene::End()
{
	Scene::End();
}

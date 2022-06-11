#include "pch.h"
#include "TextureScene.h"
#include "Box.h"
#include "Quad.h"
#include "GameObject.h"
#include "Camera.h"
#include "Light.h"
#include "Sphere.h"
#include "Triangle.h"
#include "TexCube.h"

void TextureScene::Start()
{
	Scene::Start();
	//// Box
	//{
	//	Ref<Quad> quad = CreateGameObject<Quad>();
	//	quad->GetTransform()->SetLocalRotation(Utils::Forward());
	//	quad->GetTransform()->SetLocalScale(Utils::One() * 10);
	//	quad->GetMaterial().SetSimpleColor(Utils::Red());
	//}

	//// Box
	//{
	//	Ref<Sphere> quad = CreateGameObject<Sphere>();
	//	quad->GetTransform()->SetLocalPosition(Utils::Right() * 10);
	//	quad->GetTransform()->SetLocalRotation(Utils::Forward());
	//	quad->GetTransform()->SetLocalScale(Utils::One() * 10);
	//	quad->GetMaterial().SetSimpleColor(Utils::Gold());
	//}

	//// Triangle
	//{
	//	Ref<Triangle> quad = CreateGameObject<Triangle>();
	//	quad->GetTransform()->SetLocalScale(Utils::One() * 10);
	//	quad->GetMaterial().SetSimpleColor(Utils::Red());
	//}

	// Box
	{
		Ref<TexCube> quad = CreateGameObject<TexCube>();
		quad->GetTransform()->SetLocalRotation(Utils::Zero());
		quad->GetTransform()->SetLocalScale(Utils::One() * 10);
		quad->GetMaterial().SetSimpleColor(Utils::White());
	}



	{
		Ref<Camera> camera;
		camera = CreateGameObject<Camera>();
		camera->Init();
		camera->SetName(L"MainCamera");
		camera->GetTransform()->SetLocalPosition(Vec3{ 0.0f, 1.0f, -20.0f});
		m_pCamera = camera;
	}

	// Directional
	{
		Ref<Light> light = CreateGameObject<Light>();
		light->SetName(L"Directional_Light_Down");
		light->GetLightInfo().type = D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL;
		light->GetLightInfo().direction = Utils::Down();
	}

	//DEVICE->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	DEVICE->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void TextureScene::Update()
{
	Scene::Update();

	CameraMove();
}

void TextureScene::End()
{
	Scene::End();
}

void TextureScene::CameraMove()
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

#include "pch.h"
#include "StartScene.h"
#include "Engine.h"

#include "Light.h"
#include "Sphere.h"
#include "Box.h"
#include "Camera.h"
#include "Planet.h"
#include "Torus.h"
#include "Spaceship.h"

StartScene::StartScene()
{
}

StartScene::~StartScene()
{
}

void StartScene::Awake()
{
	INPUT->AddKey("Space", VK_SPACE);
	INPUT->AddKey("Z", 'Z');
	INPUT->AddKey("C", 'C');


	// 행성셋팅
	{
		CreatePlanat(L"Sun",		Vec3{ 0.0f,0.0f,0.0f },		10.0f,	50.0f, Utils::Red());
		CreatePlanat(L"Venus",		Vec3{ 30.0f,0.0f,0.0f },	7.0f,	10.0f, Utils::Red(), L"Sun");
		CreatePlanat(L"Earth",		Vec3{ 50.0f,0.0f,0.0f },	10.0f,	100.0f, Utils::Blue(), L"Sun");
		CreatePlanat(L"Mars",		Vec3{ 70.0f,0.0f,0.0f },	9.6f,	32.0f, Utils::Yellow(), L"Sun");
		CreatePlanat(L"Jupiter",	Vec3{ 100.0f,0.0f,0.0f },	10.0f,	60.0f, Utils::IceBlue(), L"Sun");
		CreatePlanat(L"Uranus",		Vec3{ 120.0f,0.0f,0.0f },	5.1f,	80.0f, Utils::Brown(), L"Sun");
		CreatePlanat(L"Neptune",	Vec3{ 150.0f,0.0f,0.0f },	7.8f,	70.0f, Utils::Gold(), L"Sun");

		// 토성
		Ref<Planet> sturn;
		sturn = CreatePlanat(L"Sturn", Vec3{ 120.0f,0.0f,30.0f }, 7.5f, 34.0f, Utils::Gold(), L"Sun");
		Ref<Torus> sturnTorus = CreateGameObject<Torus>();
		Vec3 scale = Utils::One() * 1.6f;
		scale.z = 0.3f;
		sturnTorus->GetTransform()->SetLocalScale(scale);
		sturnTorus->GetTransform()->SetParent(sturn->GetTransform());
		sturnTorus->GetMaterial().SetSimpleColor(Utils::Brown(), 1.0f);
		sturnTorus->GetTransform()->SetLocalRotation(Utils::Right() * 3.14f * 0.5f);
		sturnTorus->GetTorusInfo() = TorusInfo{ 0.4f, 0.8f, 30, 30 };

		CreatePlanat(L"Earth_Satellite_0", Vec3{ 30.0f,0.0f,0.0f }, 5.0f, 700.0f, Utils::White(), L"Earth");

		CreatePlanat(L"Jupiter_Satellite_0", Vec3{ 1.0f,0.0f,-0.5f }, 2.0f, 40.0f, Utils::Green(), L"Jupiter");
		CreatePlanat(L"Jupiter_Satellite_1", Vec3{ 2.0f,0.0f,0.0f }, 3.0f, 100.0f, Utils::Green(), L"Jupiter");
		CreatePlanat(L"Jupiter_Satellite_2", Vec3{ 0.5f,0.0f,-0.4f }, 1.0f, 5.0f, Utils::Green(), L"Jupiter");

		CreatePlanat(L"Sturn_Satellite_0",	Vec3{ 1.0f,0.0f,0.5f }, 4.3f, 100.0f, Utils::Blue(), L"Sturn");
		CreatePlanat(L"Sturn_Satellite_1",	Vec3{ 3.0f,0.0f,0.0f }, 2.1f, 30.0f, Utils::Blue(), L"Sturn");

		CreatePlanat(L"Uranus_Satellite_0", Vec3{ 1.0f,0.0f,0.5f }, 1.0f, 40.0f, Utils::Red(), L"Uranus");
	}

	// Spaceship
	{
		m_pPlayer = CreateGameObject<Spaceship>();
		m_pPlayer->SetName(L"Player");

	}

	{
		m_pPointLight = CreateGameObject<Light>();
	 	m_pPointLight->SetName(L"Point_Light");
		m_pPointLight->GetLightInfo().type = D3DLIGHTTYPE::D3DLIGHT_POINT;
		m_pPointLight->GetTransform()->SetLocalPosition(Utils::Up() * 10.0f);
		m_pPointLight->GetLightInfo().range = 1000.0f;
		m_pPointLight->GetLightInfo().diffuse = Utils::White();
		m_pPointLight->GetLightInfo().attenuation0 = 0.0f;
		m_pPointLight->GetLightInfo().attenuation1 = 0.01f;
		m_pPointLight->GetLightInfo().attenuation2 = 0.0f;
	}

	{
		Ref<Light> light = CreateGameObject<Light>();
		light->SetName(L"Player_Point_Light");

		light->GetLightInfo().type = D3DLIGHTTYPE::D3DLIGHT_POINT;
		light->GetTransform()->SetLocalPosition(Utils::Up() * 3.0f);
		light->GetLightInfo().range = 200.0f;
		light->GetLightInfo().diffuse = Utils::White();
		light->GetLightInfo().attenuation0 = 0.0f;
		light->GetLightInfo().attenuation1 = 0.007f;
		light->GetLightInfo().attenuation2 = 0.0f;

		light->GetTransform()->SetParent(m_pPlayer->GetTransform());
	}


	{
		Ref<Box> plane = CreateGameObject<Box>();
		plane->SetName(L"Plane");
		plane->GetTransform()->SetLocalScale((Utils::One() - Utils::Up()) * 10000.0f);
		plane->GetTransform()->SetLocalPosition(Utils::Down() * 100.0f);
		plane->GetMaterial().SetSimpleColor(Utils::White());
		
	}


	{
		m_pCamera = CreateGameObject<Camera>();
		m_pCamera->Init();
		m_pCamera->SetName(L"MainCamera");
		m_pCamera->GetTransform()->SetLocalPosition(Vec3{ 0.0f, 100.0f, -1000.0f });
	}

	// Directional
	{
		Ref<Light> light = CreateGameObject<Light>();
		light->SetName(L"Directional_Light_Down");
		light->GetLightInfo().type = D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL;
		light->GetLightInfo().direction = Utils::Down();
	}

	//// Directional
	//{
	//	Ref<Light> light = CreateObject<Light>();
	//	light->SetName(L"Directional_Light_Up");
	//	light->GetLightInfo().type = D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL;
	//	light->GetLightInfo().direction = Utils::Up();
	//}

	DEVICE->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	Scene::Awake();
}

void StartScene::Update()
{

	Scene::Update();


}

void StartScene::End()
{
	Scene::End();
}

Ref<Planet> StartScene::CreatePlanat(const wstring& p_strName, const Vec3& pos, float scaling, float speed, const Color& color, const wstring& p_strParent)
{
	Ref<Planet> planet = CreateGameObject<Planet>();

	planet->SetName(p_strName);
	planet->GetMaterial().SetSimpleColor(color , 0.3f);

	planet->GetTransform()->SetLocalPosition(pos);
	planet->GetTransform()->SetLocalScale(Utils::One() * scaling);

	planet->SetSpeed(speed);


	if ((p_strParent == L"" || p_strParent.empty()) == false) {
		Ref<GameObject> parent = FindGameObject(p_strParent);
		if (parent == nullptr) {
			return planet;
		}
		Ref<GameObject> obj = CreateGameObject<GameObject>();
		obj->SetName(planet->GetName() + L"_Parent");
		obj->GetTransform()->SetLocalScale(Utils::One());
		obj->GetTransform()->SetLocalRotation(Utils::One());
		planet->GetTransform()->SetLocalPosition(parent->GetTransform()->GetLocalPosition() + pos);
		planet->GetTransform()->SetParent(obj->GetTransform());
	}

	return planet;
}

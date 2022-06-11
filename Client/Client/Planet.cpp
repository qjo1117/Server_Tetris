#include "pch.h"
#include "Planet.h"
#include "Scene.h"

bool Planet::m_bRotation = true;

Planet::Planet()
{
}

Planet::~Planet()
{

}

// 대충 해야할꺼 썬을 기준으로 공전을 따로 셋팅해준다.
void Planet::Init()
{
	Sphere::Init();
}

void Planet::Awake()
{
	Sphere::Awake();

}

void Planet::Start()
{
	Sphere::Start();
}

void Planet::Update()
{
	Sphere::Update();

	if (Planet::m_bRotation == false) {
		return;
	}

	Vec3 rotation = Utils::Zero();
	rotation = GetTransform()->GetLocalRotation();
	rotation.y += D3DXToRadian(m_fSpeed) * DELTATIME;
	
	GetTransform()->SetLocalRotation(rotation);
	Ref<Transform> parent = GetTransform()->GetParent();
	if (parent) {
		parent->SetLocalRotation(rotation);
		
		vector<wstring> tokens = Utils::Split(parent->GetObj()->GetName(), '_');
		Ref<GameObject> originParent = GET_SINGLE(SceneManager)->GetCurrentScene()->FindGameObject(tokens[0]);
		
		Vec3 position = originParent->GetTransform()->GetLocalPosition() + GetTransform()->GetLocalPosition();
		position = Utils::Vec3ToMatrix(position, originParent->GetTransform()->GetMatrixRotation());
		parent->SetLocalPosition(position);
	}



}

void Planet::End()
{
	Sphere::End();
}

void Planet::SetSpeed(float p_fSpeed)
{
	m_fSpeed = p_fSpeed;
}

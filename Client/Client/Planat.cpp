#include "pch.h"
#include "Planat.h"

Planat::Planat()
{
}

Planat::~Planat()
{

}

void Planat::Awake()
{
	Sphere::Awake();
}

void Planat::Start()
{
	Sphere::Start();
}

void Planat::Update()
{
	Sphere::Update();

	// TODO : ȸ������ �ش�.
	Vec3 rotation = GetTransform()->GetLocalRotation();
	rotation.y += D3DXToRadian(m_fSpeed) * DELTATIME;
	GetTransform()->SetLocalRotation(rotation);
}

void Planat::End()
{
	Sphere::End();
}

void Planat::SetSpeed(float p_fSpeed)
{
	m_fSpeed = p_fSpeed;
}
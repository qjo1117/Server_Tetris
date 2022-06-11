#include "pch.h"
#include "Torus.h"

void Torus::Init()
{
	GameObject::Init();

	::D3DXCreateTorus(DEVICE, m_info.innerRadius, m_info.outerRadius, m_info.sides, m_info.rings,
		&m_pMeshBuffer, nullptr);
}

void Torus::Awake()
{
	GameObject::Awake();


}

void Torus::Update()
{
	GameObject::Update();

}

void Torus::End()
{
	GameObject::End();
}

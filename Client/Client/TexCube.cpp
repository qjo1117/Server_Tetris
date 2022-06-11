#include "pch.h"
#include "TexCube.h"
#include "Engine.h"
#include "Texture.h"

void TexCube::Init()
{
	Box::Init();

	Ref<Texture> texture = RESOURCES->LoadFromTexture(L"1.jpg");
	m_pTexture = texture;

	
}

void TexCube::Update()
{
	Box::Update();

	{
		float speed = 1.0f;
		Vec3 rot = GetTransform()->GetLocalRotation();
		rot.y += speed * DELTATIME;
		rot.z += speed * DELTATIME;
		GetTransform()->SetLocalRotation(rot);
	}
}

void TexCube::End()
{
	Box::End();


}

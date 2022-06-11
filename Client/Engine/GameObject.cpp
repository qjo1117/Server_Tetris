#include "pch.h"
#include "GameObject.h"
#include "Texture.h"

uint64 GameObject::Count = 0;

GameObject::GameObject() 
{
	m_strName = L"GameObject_" + std::to_wstring(GameObject::Count++);

}

GameObject::~GameObject()
{
	m_pTransform = nullptr;
	m_pMesh = nullptr;
}

void GameObject::Init()
{
	m_pTransform->SetObj(shared_from_this());
}

void GameObject::Awake()
{
	

}

void GameObject::Start()
{
	if (m_bEnable == false) {
		return;
	}
}

void GameObject::Update()
{
	if (m_bEnable == false) {
		return;
	}
}

void GameObject::Render()
{
	if (m_bMesh == false) {
		return;
	}

	DEVICE->SetMaterial((D3DMATERIAL9*)&m_material);
	DEVICE->SetTransform(D3DTS_WORLD, &GetTransform()->GetLocalToWorldMatrix());

	if (m_pTexture) {
		DEVICE->SetTexture(0, m_pTexture->GetTexture());
	}

	if (m_pMeshBuffer) {
		m_pMeshBuffer->DrawSubset(0);
	}
	else if (m_pMesh) {
		m_pMesh->Render();
	}

}

void GameObject::End()
{
	if (m_pMeshBuffer != nullptr) {
		m_pMeshBuffer->Release();
		m_pMeshBuffer = nullptr;
	}

	// REF RELEASE
	m_pMesh = nullptr;
	m_pTransform = nullptr;
}

void GameObject::FinalUpdate()
{
	m_pTransform->FinalUpdate();
}

void GameObject::SetName(const wstring& p_strName)
{
	m_strName = p_strName;
}

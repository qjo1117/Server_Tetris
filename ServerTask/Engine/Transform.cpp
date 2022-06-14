#include "pch.h"
#include "Transform.h"
#include "GameObject.h"

Transform::Transform()
{
}

Transform::~Transform()
{
	
}

void Transform::FinalUpdate()
{
	//if (_change == false) {
	//	return;
	//}
	//_change = false;


	m_matLocal = GetMatrixScale() * GetMatrixRotation() * GetMatrixTranslation();
	m_matWorld = m_matLocal;

	/* ----- �θ� ������� ���������� �̿����ش�. ----- */
	Ref<Transform> parent = GetParent();
	if (parent != nullptr) {
		m_matWorld *= GetParentWorldMatrix(parent);
	}
}

Matrix Transform::GetMatrixTranslation()
{
	return Utils::CreateTranslation(m_localPosition);
}

Matrix Transform::GetMatrixRotation()
{
	Matrix matRotation = Utils::CreateRotationX(m_localRotation.x);
	matRotation *= Utils::CreateRotationY(m_localRotation.y);
	matRotation *= Utils::CreateRotationZ(m_localRotation.z);
	return matRotation;
}

Matrix Transform::GetMatrixScale()
{
	return Utils::CreateScaling(m_localScale);
}

void Transform::SetObj(Ref<class GameObject> p_pObj)
{
	m_pOwner = p_pObj;
}

inline Ref<class GameObject> Transform::GetObj()
{
	return m_pOwner.lock();
}

void Transform::SetParent(Ref<Transform> p_pParent)
{
	if (p_pParent == nullptr) {
		return;
	}
	// TODO : ���� Parent�� �����ϸ� ����� �۾��� ������Ѵ�.
	m_pParent = p_pParent;
	p_pParent->m_vecChilds.push_back(GetObj()->GetTransform());
}

Matrix Transform::GetParentWorldMatrix(WRef<Transform> p_parent)
{
	// TODO : �Ƹ� ����ϴٰ� �ѹ��� �̻����� ������?

	if (p_parent.lock() == nullptr) {
		return Utils::Identity;
	}

	Matrix mat = p_parent.lock()->GetLocalMatrix();
	return mat * GetParentWorldMatrix(p_parent.lock()->GetParent());
}

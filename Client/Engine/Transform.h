#pragma once


/* ----------------------
		Transform
---------------------- */

class Transform
{
public:
	Transform();
	~Transform();

	void FinalUpdate();		// 시점을 맞춰줄려고 이름 이렇게 지음


public:
	/* ---------- Helper Function ---------- */
	const Matrix& GetLocalToWorldMatrix() { return m_matWorld; }
	const Matrix& GetLocalMatrix() { return m_matLocal; }
	Vec3 GetWorldPosition() { return Utils::Translation(m_matWorld); }

	Vec3 GetRight() { return Utils::Right(m_matWorld); }
	Vec3 GetUp() { return Utils::Up(m_matWorld); }
	Vec3 GetForward() { return Utils::Forward(m_matWorld); }

	void SetLocalMatrix(const Vec3& scale, const Vec3& rotation, const Vec3& position) { m_localScale = scale; m_localPosition = position; m_localRotation = rotation;  }
	void SetLocalPosition(const Vec3& position) { m_localPosition = position; }
	void SetLocalRotation(const Vec3& rotation) { m_localRotation = rotation; }
	void SetLocalScale(const Vec3& scale) { m_localScale = scale; }

	Vec3 GetLocalPosition() { return m_localPosition; }
	Vec3 GetLocalRotation() { return m_localRotation; }
	Vec3 GetLocalScale() { return m_localScale; }

	Matrix GetMatrixTranslation();
	Matrix GetMatrixRotation();
	Matrix GetMatrixScale();

	// 더 길어지게 하고 싶지않음
	void Translation(const Vec3& move) { m_localPosition += move; }

	void SetObj(Ref<class GameObject> p_pObj);
	inline Ref<class GameObject> GetObj();


	void SetParent(Ref<Transform> p_pParent);
	Ref<Transform> GetParent() { return m_pParent.lock(); }
	vector<WRef<Transform>>& GetChilds() { return m_vecChilds; }

	Matrix GetParentWorldMatrix(WRef<Transform> p_parent);

private:
	/* ----- Transform Variable ----- */
	Vec3 m_localPosition = Utils::Zero();
	Vec3 m_localRotation = Utils::Zero();
	Vec3 m_localScale = Utils::One();

	Matrix m_matLocal = Utils::Identity;
	Matrix m_matWorld = Utils::Identity;

	WRef<Transform>	m_pParent;
	vector<WRef<Transform>> m_vecChilds;

	WRef<class GameObject> m_pOwner;
};


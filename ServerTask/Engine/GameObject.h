#pragma once

#include "Engine.h"
#include "Transform.h"
#include "Mesh.h"

/* --------------------------------------------------
					GameObject
	머테리얼이나 컴포넌트같이 확장성은 버릴껍니다.
	dx9 과제,시험용도로 그짓하기에는 시간이....

	간단하게 구성할 생각이기 때문에 
	Commponent패턴은 갖다 던졌습니다.
	상속으로 해결해볼 생각입니다.
-------------------------------------------------- */

// 망할 업데이트및 정렬 순서 맞춰주기위한 짓
// 이럴꺼면 레이어를 맞춰주는게 나을거같기도하고...
enum class GameObject_TYPE : uint32
{
	NONE = 0,
	Light,
	Camera,
	GameObject,
	End,
};

struct Material
{
	D3DCOLORVALUE   diffuse = Utils::White();
	D3DCOLORVALUE   ambient = Utils::White();
	D3DCOLORVALUE   specular = Utils::Black();
	D3DCOLORVALUE   emissive = Utils::Black();
	float           power = 0.0f;   

	void SetSimpleColor(const Color& color, float power = 1.0f) {
		diffuse = color;
		ambient = color * power;
		specular = Utils::White();
	}
};

class GameObject : public enable_shared_from_this<GameObject>
{
public:
	GameObject();
	virtual ~GameObject();

public:
	/* ------- Update Function ------- */
	virtual void Init();
	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void Render();		// 생각해보니 이놈을 굳이? 오버라이딩?
	virtual void End();
	virtual void FinalUpdate();

public:
	/* ------- Helper Function ------- */
	Ref<Transform> GetTransform() { return m_pTransform; }

	void SetName(const wstring& p_strsName);
	const wstring& GetName() { return m_strName; }

	uint32 GetType() { return static_cast<uint32>(m_eType); }
	Material& GetMaterial() { return m_material; }

	void OnMesh() { m_bMesh = true; }		// 귀찮을때
	void OffMesh() { m_bMesh = false; }		// 쓸려고요
	bool GetIsMesh() { return m_bMesh; }	// 기본헬퍼
	void SetIsMesh(bool p_bMesh) { m_bMesh = p_bMesh; }

	void OnEnable() { m_bEnable = true; }		// 귀찮을때
	void OffEnable() { m_bEnable = false; }		// 쓸려고요
	bool GetIsEnable() { return m_bEnable; }	// 기본헬퍼
	void SetIsEnable(bool p_bEnable) { m_bEnable = p_bEnable; }

protected:
	/* ------- Draw ------- */
	LPD3DXMESH m_pMeshBuffer = nullptr;
	Material m_material = {};
	Ref<Mesh> m_pMesh = make_shared<Mesh>();
	bool m_bMesh = true;

	Ref<class Texture> m_pTexture = nullptr;

	bool m_bEnable = true;

	Ref<Transform> m_pTransform = make_shared<Transform>();
	wstring m_strName;
	GameObject_TYPE m_eType = GameObject_TYPE::GameObject;

private:
	//GENERATE_CLASS_TYPE_INFO(GameObject)

private:
	static uint64 Count;
};

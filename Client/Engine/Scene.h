#pragma once

class GameObject;

/*----------
	Scene
-----------*/
class Scene : public enable_shared_from_this<Scene>
{
public:
	/* ----- Constructor Function ----- */
	Scene();
	virtual ~Scene();

public:
	/* ----- External Function ----- */
	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void FinalUpdate();
	virtual void End();

	virtual void Render();

	Ref<class Camera> GetMainCamera();

	template<typename T>
	Ref<T> CreateGameObject();
public:
	/* ----- Helper Function ----- */
	void AddGameObject(Ref<GameObject> p_pGameObject);
	void RemoveGameObject(Ref<GameObject> p_pGameObject);
	Ref<GameObject> FindGameObject(const wstring& p_strName);

	const wstring& GetName() { return m_strName; }
	void SetName(const wstring& p_strName) { m_strName = p_strName; }

	vector<Ref<GameObject>>& GetGameObjects() { return m_vecGameObjects; }

protected:
	/* ----- Scene GameObject Mapping Variable ----- */
	vector<Ref<GameObject>> m_vecGameObjects;
	Ref<class Camera> m_pCamera = nullptr;

	wstring m_strName = L"BaseScene";
};

template<typename T>
Ref<T> Scene::CreateGameObject()
{
	Ref<T> obj = make_shared<T>();
	static_pointer_cast<T>(obj)->Init();
	AddGameObject(obj);
	return obj;
}
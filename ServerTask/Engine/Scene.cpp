#include "pch.h"
#include "Scene.h"
#include "Camera.h"

Scene::Scene()
{
	m_vecGameObjects.reserve(1000);


}

Scene::~Scene()
{
}

void Scene::Awake()
{
	std::sort(m_vecGameObjects.begin(), m_vecGameObjects.end(), [](Ref<GameObject>& a, Ref<GameObject>& b) { return a->GetType() < b->GetType(); });

	if (m_pCamera == nullptr) {
		for (Ref<GameObject>& obj : m_vecGameObjects) {
			if (obj->GetType() == static_cast<uint32>(GameObject_TYPE::Camera)) {
				m_pCamera = static_pointer_cast<Camera>(obj);
				break;
			}
		}
	} 

	for (Ref<GameObject>& obj : m_vecGameObjects) {
		obj->Awake();
	}
}

void Scene::Start()
{
	for (Ref<GameObject>& obj : m_vecGameObjects) {
		if (obj->GetIsEnable() == false) {
			continue;
		}
		obj->Start();
	}
}

void Scene::Update()
{
	for (Ref<GameObject>& obj : m_vecGameObjects) {
		if (obj->GetIsEnable() == false) {
			continue;
		}
		obj->Update();
	}
}

void Scene::FinalUpdate()
{
	for (Ref<GameObject>& obj : m_vecGameObjects) {
		if (obj->GetIsEnable() == false) {
			continue;
		}
		obj->FinalUpdate();
	}

}

void Scene::End()
{
	for (Ref<GameObject>& obj : m_vecGameObjects) {
		obj->End();
		obj = nullptr;
	}
	m_vecGameObjects.clear();

	if (m_pCamera) {
		m_pCamera->End();
		m_pCamera = nullptr;
	}
}

void Scene::Render()
{
	for (Ref<GameObject>& obj : m_vecGameObjects) {
		if (obj->GetIsEnable() == false) {
			continue;
		}
		obj->Render();
	}
}

Ref<class Camera> Scene::GetMainCamera()
{
	return m_pCamera;
}

void Scene::AddGameObject(Ref<GameObject> p_pGameObject)
{
	m_vecGameObjects.push_back(p_pGameObject);
}

void Scene::RemoveGameObject(Ref<GameObject> p_pGameObject)
{
	auto findGo = std::find(m_vecGameObjects.begin(), m_vecGameObjects.end(), p_pGameObject);
	if (findGo != m_vecGameObjects.end()) {
		m_vecGameObjects.erase(findGo);
	}
}

Ref<GameObject> Scene::FindGameObject(const wstring& p_strName)
{
	for (Ref<GameObject>& obj : m_vecGameObjects) {
		if (obj->GetName().compare(p_strName) == 0) {
			return obj;
		}
	}

	return nullptr;
}

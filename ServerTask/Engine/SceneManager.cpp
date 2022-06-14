#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Camera.h"
#include "Engine.h"

void SceneManager::Update()
{
	if (m_pCurrentScene == nullptr) {
		return;
	}
	m_pCurrentScene->Update();
	m_pCurrentScene->FinalUpdate();

}

void SceneManager::Render()
{

	if (m_pCurrentScene) {
		m_pCurrentScene->Render();
	}

}

void SceneManager::LoadScene(SCENE_TYPE p_eType)
{
	if (m_pCurrentScene == nullptr) {
		m_pCurrentScene = m_arrScene[static_cast<uint32>(p_eType)];
	}
	else {
		// TODO : 내용물 초기화
		m_pCurrentScene = m_arrScene[static_cast<uint32>(p_eType)];
	}

	m_eType = p_eType;
	m_pCurrentScene->Awake();
	m_pCurrentScene->Start();
}

void SceneManager::End()
{
	m_pCurrentScene->End();
	m_pCurrentScene = nullptr;
}

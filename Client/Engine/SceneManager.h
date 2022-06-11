#pragma once

class Scene;

enum class SCENE_TYPE : uint32
{
	TOOL,

	START,
	TEXTURE,
	MESH,
	SERVER,
	STAGE_0,
	STAGE_1,
	STAGE_2,

	END,
};

/*-----------------
	SceneManager
------------------*/
class SceneManager
{
private:
	/* ----- SingleTon ----- */
	DECLARE_SINGLE(SceneManager)

public:
	/* ----- External Function ----- */
	void Update();
	void Render();
	void LoadScene(SCENE_TYPE type);
	void End();

public:
	/* ----- Template Function ----- */
	template<typename T>
	void CreateScene(SCENE_TYPE type);


public:
	/* ----- Helper Function ----- */
	Ref<Scene>	GetCurrentScene() { return m_pCurrentScene; }
	SCENE_TYPE			GetType() { return m_eType; }
private:
	/* ----- Scene Variable ----- */
	Ref<Scene> m_pCurrentScene = nullptr;
	array<Ref<Scene>, static_cast<uint32>(SCENE_TYPE::END)> m_arrScene;
	SCENE_TYPE m_eType = SCENE_TYPE::END;
};


/* ----------------------------------------
	TODO :	Load할때 내용물 클리어하는 것
			Change할때 내용물 클리어하는 것
			★File에서 데이터를 로드★
---------------------------------------- */

template<typename T>
void SceneManager::CreateScene(SCENE_TYPE type)
{
	uint32 index = static_cast<uint32>(type);

	if (std::is_convertible_v<T, Scene> == false) {
		return;
	}

	Ref<T> scene = make_shared<T>();

	if (m_arrScene[index] == nullptr) {
		m_arrScene[index] = scene;
	}
}

#pragma once

#define RESOURCES GET_SINGLE(ResourcesManager)

enum class RESOURCE_TPYE
{
	NONE = -1,
	TEXTURE,
	END,
};


/*-------------------------
	 ResourcesManager
--------------------------*/
class ResourcesManager
{
	DECLARE_SINGLE(ResourcesManager);
public:
	void Init();
	void End();

	Ref<class Texture> LoadFromTexture(const wstring& p_path);

private:
	vector<Ref<class Texture>> m_vecReesources;
	//using KeyObjMap = std::map<wstring, Ref<GameObject>>;
	//array<KeyObjMap, RESOURCE_TPYE> _resources;
};


// 텍스처 사용법보고 대충 얼마나 할지 계산해보자

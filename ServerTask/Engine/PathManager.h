#pragma once

#define BASE_PATH_KEY               "BasePath"
#define TEXTURE_PATH_KEY            "Texture"
#define SCENE_PATH_KEY              "Scene"
#define FBX_PATH_KEY				"FBX"
#define SOUND_PATH_KEY				"Sound"



class PathManager
{
private:
	DECLARE_SINGLE(PathManager);
public:
	void Init();
	void Clear();

	bool CreatePath(const string& key, const wstring& path, const string& basePathKey = BASE_PATH_KEY);
	const wstring& FindPath(const string& key);
	const string& FindPathMultiByte(const string& key);

private:
	unordered_map<string, wstring> _paths;
	char _strPath[MAX_PATH] = {};
	
};


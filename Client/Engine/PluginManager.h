#pragma once



class IPlugin;

struct PluginInfo
{
	IPlugin* pPlugin = nullptr;
	HMODULE hDll = nullptr;
};

class PluginManager
{
	DECLARE_SINGLE(PluginManager);
public:
	void Init(class EditorManager& p_Editor, class Engine& p_Engine);
	void Update();
	void Render();
	void End();

	void LoadPlugins();  
	bool LoadPlugin(const wstring& p_fileName);


public:
	void Log(const string& log);
	class EditorManager* GetEditor();
	class Engine* GetEngine();

private:
	PRIVATE_PROPERTY(vector<Ref<PluginInfo>>, vecPlugins);

	// TODO : È®ÀÎ¿ë
	wstring m_strName = L"Plugin";
	class EditorManager* m_pEditor = nullptr;
	class Engine* m_pEngine = nullptr;
};



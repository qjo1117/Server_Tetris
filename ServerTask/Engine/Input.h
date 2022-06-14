#pragma once

struct KeyInfo
{
	string name;
	vector<uint32> vecKeys;
	bool bDown = false;
	bool bPress = false;
	bool bUp = false;
};



#define INPUT GET_SINGLE(Input)

/*-----------
	Input
------------*/
class Input
{
private:
	DECLARE_SINGLE(Input)

public:
	template<typename T>
	void AddKey(const T& data);

	template<typename T, class... Args>
	void AddKey(const T& data, const Args&... arg);

public:
	bool Init(HWND hWnd);
	void Update();
	void End();

	bool KeyDown(const string& strKey);
	bool KeyPress(const string& strKey);
	bool KeyUp(const string& strKey);

	void Clear();

	POINT GetMousePos() { return m_mousePos; }

public:
	/* --------- Editor ------------ */
	void __Editor__InputTool();

private:
	Ref<KeyInfo> FindKey(const string& strKey) const;

private:
	HWND m_hWnd;				// 마우스
	POINT m_mousePos = {};

	unordered_map<string, Ref<KeyInfo>> m_mapKeys;		// 등록 변수
	Ref<KeyInfo> m_pCreateKey;
};

template<typename T>
inline void Input::AddKey(const T& data)
{
	if (m_pCreateKey == nullptr) {
		m_pCreateKey = make_shared<KeyInfo>();
	}

	const char* pType = typeid(T).name();

	// 일반적인 등록키면 그냥 넣어준다.
	if (::strcmp(pType, "char") == 0 ||
		::strcmp(pType, "int") == 0) {
		m_pCreateKey->vecKeys.push_back((uint32)data);
	}
	else {			// 이름 등록
		m_pCreateKey->name = data;
		m_mapKeys.insert(make_pair(m_pCreateKey->name, m_pCreateKey));
	}
}

template<typename T, class... Args>
inline void Input::AddKey(const T& data, const Args&... arg)
{
	if (m_pCreateKey == nullptr) {
		m_pCreateKey = make_shared<KeyInfo>();
	}
	const char* pType = typeid(T).name();

	if (strcmp(pType, "char") == 0 ||
		strcmp(pType, "int") == 0) {
		m_pCreateKey->vecKeys.push_back((uint32)data);
	}
	else {
		m_pCreateKey->name = data;
		m_mapKeys.insert(make_pair(m_pCreateKey->name, m_pCreateKey));
	}

	AddKey(arg...);

	if (m_pCreateKey) {
		m_pCreateKey = nullptr;
	}
}
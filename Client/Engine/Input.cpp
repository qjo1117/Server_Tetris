#include "pch.h"
#include "Input.h"

bool Input::Init(HWND hWnd)
{
	/* ------ ¸ÊÇÎ ------ */
	m_hWnd = hWnd;

	/* ------ Key Register ------ */
	AddKey('W', "Forward");
	AddKey('S', "Back");
	AddKey('D', "Right");
	AddKey('A', "Left");

	//EDITOR->AddEditor([]() { INPUT->__Editor__InputTool(); });

    return true;
}

void Input::Update()
{
	HWND current_hWnd = GetFocus();
	if (current_hWnd != m_hWnd) {
		return;
	}


	for (auto item : m_mapKeys) {
		Ref<KeyInfo>& info = item.second;
		int32 iPushCount = 0;
		for (uint32 state : info->vecKeys) {
			if (GetAsyncKeyState(state) & 0x8000) {
				++iPushCount;
			}
		}

		if (iPushCount == info->vecKeys.size()) {
			if (info->bDown == false && info->bPress == false) {
				info->bDown = true;
			}
			else if (info->bDown && info->bPress == false) {
				info->bPress = true;
				info->bDown = false;
			}
		}
		else {
			if (info->bDown == false || info->bPress == false) {
				info->bUp = true;
				info->bDown = false;
				info->bPress = false;
			}
			else if (info->bUp) {
				info->bUp = false;
			}
		}
	}

	::GetCursorPos(&m_mousePos);
	::ScreenToClient(m_hWnd, &m_mousePos);
}

void Input::End()
{
	for (auto& key : m_mapKeys) {
		key.second = nullptr;
	}
	m_mapKeys.clear();
	m_pCreateKey = nullptr;
}

bool Input::KeyDown(const string& strKey)
{
	Ref<KeyInfo> pInfo = Input::FindKey(strKey);
	if (pInfo == nullptr) {
		return false;
	}

	return pInfo->bDown;
}

bool Input::KeyPress(const string& strKey)
{
	Ref<KeyInfo> pInfo = Input::FindKey(strKey);
	if (pInfo == nullptr) {
		return false;
	}

	return pInfo->bPress;
}

bool Input::KeyUp(const string& strKey)
{
	Ref<KeyInfo> pInfo = Input::FindKey(strKey);
	if (pInfo == nullptr) {
		return false;
	}

	return pInfo->bUp;
}

void Input::Clear()
{
	for (auto item : m_mapKeys) {
		item.second = nullptr;		// Ref - 1
	}

	m_mapKeys.clear();
}

Ref<KeyInfo> Input::FindKey(const string& strKey) const
{
	auto iter = m_mapKeys.find(strKey);
	if (iter == m_mapKeys.end()) {
		return nullptr;
	}

	return iter->second;
}

void Input::__Editor__InputTool()
{
	ImGui::Begin("Input Tool");

	for (auto& item : m_mapKeys) {
		string text = (item.first + " : " + item.second->name);
		ImGui::Text(text.c_str());
	}

	ImGui::End();
}
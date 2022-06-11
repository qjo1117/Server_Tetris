#include "pch.h"
#include "PathManager.h"

void PathManager::Init()
{
	wchar_t strPath[MAX_PATH] = {};

	::GetCurrentDirectoryW(MAX_PATH, strPath);

	// ���� ��ο��� / or \\ �� ���ö����� ����� �͵��� �����Ѵ�.
	for (int32 i = ::wcslen(strPath) - 1; i >= 0; --i) {
		if (strPath[i] == '/' || strPath[i] == '\\') {
			strPath[i + 1] = '\0';
			break;
		}
	}

	::wcscat_s(strPath, MAX_PATH, L"Resources\\");

	_paths.insert(make_pair<string, wstring>(BASE_PATH_KEY, strPath));

	assert(CreatePath(TEXTURE_PATH_KEY, L"Texture\\"));
	assert(CreatePath(FBX_PATH_KEY, L"Fbx\\"));

}

void PathManager::Clear()
{
	// ����� �Լ��� ���� �����
}

bool PathManager::CreatePath(const string& key, const wstring& path, const string& basePathKey)
{
	const wchar_t* basePath = FindPath(basePathKey).c_str();
	wstring wstrPath;

	if (basePath != L"") {
		wstrPath = basePath;
	}

	wstrPath += path;

	_paths.insert(make_pair(key, wstrPath));

	return true;
}

const wstring& PathManager::FindPath(const string& key)
{
	auto findIt = _paths.find(key);
	if (findIt == _paths.end()) {
		return L"";
	}

	return findIt->second;
}

const string& PathManager::FindPathMultiByte(const string& key)
{
	const wchar_t* path = FindPath(key).data();

	if (path == nullptr) {
		return nullptr;
	}

	::memset(_strPath, 0, sizeof(char) * MAX_PATH);
	::WideCharToMultiByte(CP_ACP, 0, path, -1, _strPath, lstrlenW(path), 0, 0);

	return _strPath;
}

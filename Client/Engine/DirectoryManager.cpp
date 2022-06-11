#include "pch.h"
#include "DirectoryManager.h"

void DirectoryManager::Init()
{

	m_path = fs::current_path().remove_filename();

	m_vecUpdateList.reserve(100);

	m_vecExtension.push_back(".h");
	m_vecExtension.push_back(".hpp");
	m_vecExtension.push_back(".cpp");
	m_vecExtension.push_back(".txt");
	
	m_vecExtension.push_back(".jpg");
	m_vecExtension.push_back(".png");
	m_vecExtension.push_back(".dds");
	
	m_vecExtension.push_back(".dll");
	m_vecExtension.push_back(".lib");


	CreateFileInfos();

	/* ------ Test ------ */
	vector<Ref<FileInfo>> vecFileInfos;
	FindFileInfo(m_pFileInfo, vecFileInfos, ".h");

}

void DirectoryManager::Update()
{

	// 갱신
	if (m_pFileInfo->Time != fs::last_write_time(m_pFileInfo->PathInfo)) {
		m_pFileInfo->Time = fs::last_write_time(m_pFileInfo->PathInfo);
	}

	if (CheckFileInfo(m_pFileInfo) == -1) {
		CreateFileInfos();
	}

}

void DirectoryManager::End()
{
	ClearAllFileInfos(m_pFileInfo);
}
void DirectoryManager::AddFileInfo(fs::path p_pathInfo, const string& p_strPath)
{
	vector<string> vecPath = Utils::Split(p_strPath, '\\');

	if (p_pathInfo.has_extension() == false) {
		// TODO : 폴더

		if (m_pFileInfo == nullptr) {
			m_pFileInfo = make_shared<FileInfo>();
			m_pFileInfo->PathInfo = p_pathInfo;
			m_pFileInfo->Parent = nullptr;
			m_pFileInfo->Name = vecPath[vecPath.size() - 1];
			m_pFileInfo->Type = FILE_TYPE::FOLDER;
			m_pFileInfo->Time = fs::last_write_time(p_pathInfo);
		}
		else {
			Ref<FileInfo> fileInfo = FindFileInfo(m_pFileInfo, vecPath);
			if (fileInfo == nullptr) {
				return;
			}
			Ref<FileInfo> tempInfo = make_shared<FileInfo>();
			tempInfo->PathInfo = p_pathInfo;
			tempInfo->Parent = fileInfo;
			tempInfo->Name = vecPath[vecPath.size() - 1];
			tempInfo->Type = FILE_TYPE::FOLDER;
			tempInfo->Time = fs::last_write_time(p_pathInfo);
			fileInfo->File.push_back(tempInfo);
		}
	}
	else {
		if (ExtensionCheck(p_strPath) == false) {
			return;
		}

		// TODO : 파일
		Ref<FileInfo> fileInfo = FindFileInfo(m_pFileInfo, vecPath);
		Ref<FileInfo> tempInfo = make_shared<FileInfo>();
		tempInfo->PathInfo = p_pathInfo;
		tempInfo->Parent = fileInfo;
		tempInfo->Name = vecPath[vecPath.size() - 1];
		tempInfo->Type = FILE_TYPE::FILE;
		tempInfo->Time = fs::last_write_time(p_pathInfo);
		fileInfo->File.push_back(tempInfo);
	}
}

Ref<FileInfo> DirectoryManager::FindFileInfo(Ref<FileInfo> p_info, const vector<string>& p_fileName, int32 index)
{
	if (p_fileName.size() - 1 == index) {
		return p_info;
	}

	for (Ref<FileInfo>& info : p_info->File) {
		if (info->Name == p_fileName[index]) {
			return FindFileInfo(info, p_fileName, ++index);
		}
	}
	return nullptr;
}

bool DirectoryManager::ExtensionCheck(const string& p_strPath)
{
	uint32 count = 0;
	for (string& extension : m_vecExtension) {
		if (p_strPath.find(extension) != string::npos) {
			return true;
		}
	}
	return false;
}

void DirectoryManager::CreateFileInfos()
{
	ClearAllFileInfos(m_pFileInfo);

	fs::path rootPath = fs::current_path().remove_filename();
	AddFileInfo(rootPath, "DX");
	for (auto& entry : fs::recursive_directory_iterator(rootPath)) {
		string strPath = entry.path().string();
		strPath = strPath.substr(rootPath.string().length(), strPath.size());
		AddFileInfo(entry.path(), strPath);
	}

}

void DirectoryManager::ClearAllFileInfos(Ref<FileInfo> p_info)
{
	if (p_info == nullptr) {
		return;
	}

	for (Ref<FileInfo>& info : p_info->File) {
		ClearAllFileInfos(info);
		if (info->Parent) {
			info->Parent = nullptr;
		}
		info = nullptr;

	}
	p_info->File.clear();
}

int32 DirectoryManager::CheckFileInfo(Ref<FileInfo> p_info)
{
	auto time = fs::last_write_time(p_info->PathInfo);
	if (p_info->Time != time) {
		m_vecUpdateList.push_back(p_info);
		return -1;
	}
	for (Ref<FileInfo>& info : p_info->File) {
		if (CheckFileInfo(info) == -1) {
			return -1;
		}
	}

	return 0;
}

void DirectoryManager::UpdateFileInfo(Ref<FileInfo> p_info)
{
	fs::path rootPath = p_info->PathInfo;
	if (rootPath.has_extension() == false) {
		return;
	}
	ClearAllFileInfos(p_info);


	auto iter = fs::directory_iterator(rootPath);
	for (auto& entry : iter) {
		string strPath = entry.path().string();
		strPath = strPath.substr(rootPath.string().length(), strPath.size());
		AddFileInfo(entry.path(), strPath);
	}
}

void DirectoryManager::FindFileInfo(Ref<FileInfo> root, vector<Ref<FileInfo>>& p_vecFileInfo, const string& extension)
{
	if (root == nullptr) {
		return;
	}

	for (Ref<FileInfo>& info : root->File) {
		if (info->PathInfo.extension().string() == extension) {
			p_vecFileInfo.push_back(info);
		}
		FindFileInfo(info, p_vecFileInfo, extension);
	}
}

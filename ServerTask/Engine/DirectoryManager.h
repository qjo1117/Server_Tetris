#pragma once


/*------------------------
	 DirectoryManager
------------------------ */

enum class FILE_TYPE
{
	NONE = -1,

	FOLDER,
	FILE,

	END
};

class FileInfo
{
public:
	PRIVATE_PROPERTY(string, Name) = "";
	PRIVATE_PROPERTY(FILE_TYPE, Type) = FILE_TYPE::NONE;
	PRIVATE_PROPERTY(fs::path, PathInfo);
	PRIVATE_PROPERTY(fs::file_time_type, Time);
	PRIVATE_PROPERTY(Ref<FileInfo>, Parent);
	PRIVATE_PROPERTY(vector<Ref<FileInfo>>, File);
};

class DirectoryManager
{
	DECLARE_SINGLE(DirectoryManager);
public:
	void Init();
	void Update();
	void End();

private:
	void AddFileInfo(fs::path p_pathInfo, const string& p_strPath);
	Ref<FileInfo> FindFileInfo(Ref<FileInfo> p_info, const vector<string>& p_fileName, int32 index = 0);

	bool ExtensionCheck(const string& p_strPath);

	void CreateFileInfos();
	void ClearAllFileInfos(Ref<FileInfo> p_info);

	int32 CheckFileInfo(Ref<FileInfo> p_info);
	void UpdateFileInfo(Ref<FileInfo> p_info);
public:
	void FindFileInfo(Ref<FileInfo> root, vector<Ref<FileInfo>>& p_vecFileInfo, const string& extension);

public:
	Ref<FileInfo>& GetFileInfo() { return m_pFileInfo; }

private:
	Ref<FileInfo> m_pFileInfo;

	fs::path m_path;
	fs::file_time_type m_time;

	vector<string>	m_vecExtension;

	uint32 m_iCount = 0;
	vector<Ref<FileInfo>> m_vecUpdateList;
};


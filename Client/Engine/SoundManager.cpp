#include "pch.h"
#include "SoundManager.h"
#include "PathManager.h"
#include "DirectoryManager.h"

bool SoundManager::Init(void)
{
	// fmod system 생성
	System_Create(&m_pSystem);

	// init(채널의 최대갯수, 초기화 플래그, 사운드 편집 여부)
	m_pSystem->init(static_cast<int32>(SOUND_CHANNEL::SC_MAX), FMOD_INIT_NORMAL, nullptr);

	return true;
}

void SoundManager::End()
{
	unordered_map<string, Ref<SoundInfo>>::iterator iter;
	for (iter = m_mapSound.begin(); iter != m_mapSound.end(); ++iter) {
		iter->second->pSound->release();
		iter->second = nullptr;
	}

	m_mapSound.clear();

	m_pSystem->close();
	m_pSystem->release();
}

bool SoundManager::LoadSound(const string& strKey, bool bLoop, const char* pFileName)
{
	if (FindSound(strKey)) {
		return true;
	}

	string temp = Utils::Wstr2Str(GET_SINGLE(PathManager)->FindPath(SOUND_PATH_KEY));

	const char* pPath = temp.c_str();
	string strPath;

	if (pPath) {
		strPath = pPath;
	}
	strPath += pFileName;

	FMOD_MODE	eMode = FMOD_LOOP_NORMAL;
	if (!bLoop) {
		eMode = DMDFO_DEFAULT;
	}

	Ref<SoundInfo>	pSound = make_shared<SoundInfo>();
	pSound->bLoop = bLoop;

	// createSound(경로, 루프여부, 사용안함, 객체 생성)
	m_pSystem->createSound(strPath.c_str(), eMode, nullptr, &pSound->pSound);
	m_mapSound.insert(make_pair(strKey, pSound));

	return false;
}

bool SoundManager::Play(const string& strKey)
{
	Ref<SoundInfo>	pSound = FindSound(strKey);

	if (pSound == nullptr) {
		return true;
	}

	// Fmod시스템을 갱신해서 한다.
	m_pSystem->update();

	SOUND_CHANNEL	eChannel = SOUND_CHANNEL::SC_BGM;
	if (pSound->bLoop == false) {
		eChannel = SOUND_CHANNEL::SC_EFFECT;
	}

	// playSound(실행시킬 사운드, nullptr, 일시정지 여부, 실행시킬 해당채널)
	m_pSystem->playSound(pSound->pSound, nullptr, false, &m_pChannel[static_cast<int32>(eChannel)]);

	return true;
}

bool SoundManager::Stop(SOUND_CHANNEL eChannel)
{
	m_pChannel[static_cast<int32>(eChannel)]->stop();

	return true;
}

bool SoundManager::Volume(SOUND_CHANNEL eChannel, float fVolum)
{
	m_pChannel[static_cast<int32>(eChannel)]->setVolume(fVolum);

	return true;
}

void SoundManager::SetSoundOffSet(SOUND_CHANNEL eChannel, float dTime)
{
	m_pChannel[static_cast<int32>(eChannel)]->setPosition(dTime, FMOD_TIMEUNIT_MS);
}

unsigned int SoundManager::GetSoundOffSet(SOUND_CHANNEL eChannel)
{
	unsigned int tTemp = 0;
	m_pChannel[static_cast<int32>(eChannel)]->getPosition(&tTemp, FMOD_TIMEUNIT_MS);
	return tTemp;
}

unsigned int SoundManager::GetLength(SOUND_CHANNEL eChannel)
{
	FMOD::Sound* pSound = nullptr;
	m_pChannel[static_cast<int32>(eChannel)]->getCurrentSound(&pSound);

	unsigned int iLength = 0;
	pSound->getLength(&iLength, FMOD_TIMEUNIT_MS);

	return iLength;
}


Ref<SoundInfo> SoundManager::FindSound(const string& strKey)
{
	unordered_map<string, Ref<SoundInfo>>::iterator iter = m_mapSound.find(strKey);

	if (iter == m_mapSound.end()) {
		return nullptr;
	}

	return iter->second;
}

void SoundManager::LoadAllSound()
{
	vector<Ref<FileInfo>> vecFileInfo;

	GET_SINGLE(DirectoryManager)->FindFileInfo(GET_SINGLE(DirectoryManager)->GetFileInfo(), OUT vecFileInfo, ".mp3");

	for (Ref<FileInfo>& fileInfo : vecFileInfo) {
		vector<string> str = Utils::Split(fileInfo->Name, '.');
		LoadSound(str[0], true, fileInfo->Name.c_str());
	}
}

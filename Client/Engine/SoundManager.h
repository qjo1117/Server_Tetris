#pragma once

#include "Fmod/fmod.hpp"


// Sound Channel
enum class SOUND_CHANNEL
{
	SC_BGM,
	SC_EFFECT,
	SC_MAX
};

// Sound
struct SoundInfo
{
	// Fmod의 Sound Class 이다.
	FMOD::Sound* pSound;
	bool	bLoop;
	unsigned int		iLength;
	SOUND_CHANNEL	eChannel;
};

/* -----------------
	SoundManager
----------------- */
class SoundManager
{
	DECLARE_SINGLE(SoundManager);
public:
	bool Init(void);
	void End();
	// 중복이 있을경우 true 없을경우 false
	bool LoadSound(const string& strKey, bool bLoop, const char* pFileName);
	void LoadAllSound();

	bool Play(const string& strKey);
	bool Stop(SOUND_CHANNEL eChannel);
	bool Volume(SOUND_CHANNEL eChannel, float fVolum);

	void SetSoundOffSet(SOUND_CHANNEL eChannel, float dTime);
	unsigned int GetSoundOffSet(SOUND_CHANNEL eChannel);

	unsigned int GetLength(SOUND_CHANNEL eChannel);

	array<FMOD::Channel*, static_cast<uint16>(SOUND_CHANNEL::SC_MAX)>& GetChannels() { return m_pChannel; }
	unordered_map<string, Ref<SoundInfo>>& GetMapSound() { return m_mapSound; }
private:
	Ref<SoundInfo> FindSound(const string& strKey);


private:
	// Fmod에서 지원하는 시스템 인터페이스
	FMOD::System* m_pSystem = nullptr;
	array<FMOD::Channel*, static_cast<uint16>(SOUND_CHANNEL::SC_MAX)> m_pChannel = { nullptr };
	unordered_map<string, Ref<SoundInfo>>	m_mapSound;
	
};


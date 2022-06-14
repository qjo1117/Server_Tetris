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
	// Fmod�� Sound Class �̴�.
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
	// �ߺ��� ������� true ������� false
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
	// Fmod���� �����ϴ� �ý��� �������̽�
	FMOD::System* m_pSystem = nullptr;
	array<FMOD::Channel*, static_cast<uint16>(SOUND_CHANNEL::SC_MAX)> m_pChannel = { nullptr };
	unordered_map<string, Ref<SoundInfo>>	m_mapSound;
	
};


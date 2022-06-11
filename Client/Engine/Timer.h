#pragma once

#define DELTATIME GET_SINGLE(Timer)->GetDeltaTime()

enum class TIMER_STATE : uint16
{
	NONE,
	STOP,
	FAST,
	SLOW
};


#define TIMER GET_SINGLE(Timer)

/*----------
	Timer
-----------*/
class Timer
{
	DECLARE_SINGLE(Timer)

public:
	/* ----- External Function ----- */
	void Init(HWND hWnd);
	void Update();

	/* ----- Helper Function ----- */
	uint32	GetFps() { return m_iFps; }
	float	GetDeltaTime() { return m_fDeltaTime; }

	void	SetTimeScale(float timeScale) { m_fTimeScale = timeScale; }

	/* ----- Time State Function ----- */
	void	Stop() { m_eState = TIMER_STATE::STOP; }
	void	Run() { m_eState = TIMER_STATE::NONE; }
	void	Slow() { m_eState = TIMER_STATE::SLOW; }
	void	Fast() { m_eState = TIMER_STATE::FAST; }

public:
	void ShowFpsWindowTitle();

private:
	/* ----- Frequency Variable ----- */
	uint64		m_iFrequency = 0;
	uint64		m_iPrevCount = 0;
	float		m_fDeltaTime = 0.0f;

private:
	/* ----- Visiable Frame Variable ----- */
	uint32		m_iFrameCount = 0;
	float		m_fFrameTime = 0.0f;
	uint32		m_iFps = 0;

private:
	/* ----- 시간이 변할때 쓰는 변수 ----- */
	float		m_fTimeScale = 2.0f;
	TIMER_STATE	m_eState = TIMER_STATE::NONE;

private:
	/* ----- Mapping ----- */
	HWND		m_hWnd = nullptr;
};


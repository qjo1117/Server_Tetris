#include "pch.h"
#include "Timer.h"

void Timer::Init(HWND hWnd)
{
	m_hWnd = hWnd;

	/* ----- 하드웨어의 갱신 빈도수를 구한다 ----- */
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_iFrequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_iPrevCount)); // CPU 클럭
}

void Timer::Update()
{
	/* ----- DeltaTime, Frame를 구해준다. ----- */
	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

	/* ----- DeltaTime ----- */
	m_fDeltaTime = (currentCount - m_iPrevCount) / static_cast<float>(m_iFrequency);
	m_iPrevCount = currentCount;

	m_iFrameCount++;
	m_fFrameTime += m_fDeltaTime;

	/* ----- Frame ----- */
	if (m_fFrameTime > 1.f) {
		m_iFps = static_cast<uint32>(m_iFrameCount / m_fFrameTime);

		m_fFrameTime = 0.f;
		m_iFrameCount = 0;

		//ShowFps();
	}

	/* ----- State ----- */
	switch (m_eState) {
	case TIMER_STATE::STOP:
		m_fDeltaTime = 0.0f;
		break;
	case TIMER_STATE::FAST:
		m_fDeltaTime *= m_fTimeScale;
		break;
	case TIMER_STATE::SLOW:
		m_fDeltaTime /= m_fTimeScale;
		break;
	}
}

void Timer::ShowFpsWindowTitle()
{
	uint32 fps = m_iFps;

	WCHAR text[20] = L"";
	::wsprintf(text, L"FPS : %d", fps);

	::SetWindowTextW(m_hWnd, text);
}

/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-08-12
 *
 * <파일 내용>
 * 시간을 다루는 클래스입니다.
 *
 ====================================================================================*/
#include "PCH.h"
#include "RXTimer.h"

namespace RX
{

	RXTimer::RXTimer()
	{
		m_rDeltaSecond = 0.0f;
		m_dwStartTime  = 0;
		m_dwEndTime    = 0;
		
		m_highStartTime.QuadPart = 0;
		m_highEndTime.QuadPart   = 0;

		// 고해상도 타이머의 주파수를 얻는다.
		// 주파수는 초당 진동수를 말하고 단위는 Hz(헤르츠)입니다.
		// 예를 들어 60Hz면 초당 60번 진동한다는 뜻입니다.
		// 주기는 1사이클(Cycle)에 걸리는 시간을 말합니다.
		// 사이클은 진동 한 번을 의미합니다. 따라서 주기는 = (1 / 주파수)입니다.
		::QueryPerformanceFrequency(&m_highFrequency);
	}

	RXTimer::~RXTimer()
	{

	}

	FLOAT RXTimer::CalcDeltaSecond()
	{
		m_dwEndTime = ::timeGetTime();

		// 밀리세컨드 단위, 1000이 1초
		m_rDeltaSecond = ((m_dwEndTime - m_dwStartTime) * 0.001f);

		m_dwStartTime = m_dwEndTime;
		return m_rDeltaSecond;
	}

	// 프레임 간격 시간보다 작다면 FPS에 따른 고정 간격 시간을 반환합니다.
	FLOAT RXTimer::CalcFixedDeltaSecondByFPS(FPS_TYPE type)
	{
		FLOAT rBaseSecond = 0.0f;
		(type == FPS_TYPE::SIXTY) ?
			(rBaseSecond = FIXED_DELTASECOND_60FPS) : (rBaseSecond = FIXED_DELTASECOND_30FPS);

		m_dwEndTime = ::timeGetTime();

		// 밀리세컨드 단위, 1000이 1초
		m_rDeltaSecond = ((m_dwEndTime - m_dwStartTime) * 0.001f);

		if (m_rDeltaSecond < rBaseSecond) // 프레임 간격 시간보다 작을 때
		{
			// 남은 프레임 시간 간격만큼 CPU를 휴식합니다.
			// 단순히 보여지는 숫자가 아니라 실제로 딜레이를 겁니다.
			// 즉, 프로그램 속도에 영향을 줍니다.
			FLOAT rSleepSecond = (rBaseSecond - m_rDeltaSecond) * CLOCKS_PER_SEC;
			::SleepEx(static_cast<DWORD>(rSleepSecond), TRUE);

			// FPS에 따른 고정 프레임 시간 간격을 대입합니다.
			// 이미 위에서 남은 프레임 시간 간격만큼 CPU가 휴식했으므로 실제 시간과도 맞아떨어집니다.
			m_rDeltaSecond = rBaseSecond;
		}

		m_dwStartTime = m_dwEndTime;
		return m_rDeltaSecond;
	}

	// 고해상도 타이머를 이용한 델타세컨드 계산 방법입니다.
	// 참고 코드 : http://www.tipssoft.com/bulletin/board.php?bo_table=FAQ&wr_id=735
	FLOAT RXTimer::CalcHigResoultionDeltaSecond()
	{
		// 현재 시점의 CPU 클럭수를 알아냅니다.
		QueryPerformanceCounter(&m_highEndTime);

		// 밀리세컨드 단위로 델타세컨드를 구합니다.
		// (현재 시점의 CPU 클럭수 - 이전 시점의 CPU 클럭수)에
		// 주파수를 원하는 단위만큼 나눈값으로 나눠줍니다. 밀리세컨드 기준은 1000입니다.
		m_rDeltaSecond = 
			static_cast<FLOAT>(m_highEndTime.QuadPart - m_highStartTime.QuadPart) /
			                   (m_highFrequency.QuadPart / 1000);

		// 초단위 기준으로 값을 출력합니다.
		//FLOAT rDeltaMilliSec = m_rDeltaSecond * 0.001f;
		//FLOAT rDeltaMicroSec = m_rDeltaSecond * 0.000001f;
		//RXLOG("MilliSec(%f) MicroSec(%f)", rDeltaMilliSec, rDeltaMicroSec);

		// 밀리세컨드를 초단위로 바꿔야 합니다.
		m_rDeltaSecond *= 0.001f;

		m_highStartTime = m_highEndTime;
		return m_rDeltaSecond;
	}

} // namespace RX end
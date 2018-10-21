/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-10-21
 *
 * <파일 내용>
 * 프로그램의 프레임을 관리하는 매니저입니다.
 * 단순히 FPS를 측정하는 것 말고도 FPS에 따른 딜레이도 줄 수 있습니다.
 * 기본 설정은 수직동기 기준의 (가변 프레임 + 프레임 스키핑)입니다.
 *
 ====================================================================================*/
#include "PCH.h"
#include "RXProgramFPSMgr.h"

PHOENIX_SINGLETON_INIT(RX::RXProgramFPSMgr);

namespace RX
{

	RXProgramFPSMgr::RXProgramFPSMgr()
	{
		m_frameSec = 0.0f;
		m_frameCnt = 0;
		m_FPS      = 0;
	}

	void RXProgramFPSMgr::InitFrame()
	{
		m_frameSec = 0.0f;
		m_frameCnt = 0;
	}

	void RXProgramFPSMgr::UpdateFrame()
	{
		m_frameSec += m_frameTimer.CalcDeltaSecond();
		++m_frameCnt;

		// 1초가 넘어가면 프레임 체크
		if (m_frameSec >= ONE_SECOND)
		{
			// FPS = 프레임 카운트 / 1초
			// 좀 더 정밀한 측정을 위해 (프레임 카운트 / 경과 시간)을 이용합니다.
			m_FPS = static_cast<INT32>(m_frameCnt / m_frameSec);
			RXDEBUGLOG("현재 프레임 카운트 %d, FPS : %d", m_frameCnt, m_FPS);
			InitFrame();
		}
	}

} // namespace RX end
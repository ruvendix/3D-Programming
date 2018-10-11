/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-08-12
 *
 * <파일 내용>
 * 프레임을 다루는 클래스입니다.
 * 현재는 가변 프레임용으로 사용되고 있습니다.
 *
 ====================================================================================*/
#include "PCH.h"
#include "RXFrame.h"

namespace RX
{

	RXFrame::RXFrame()
	{
		m_frameCount = 0;
		m_FPS        = 0;
	}

	RXFrame::~RXFrame()
	{

	}

	void RXFrame::InitFrame()
	{
		m_frameTime.SubstituteStartTimeWithEndTime();
		m_frameCount = 0;
	}

	void RXFrame::UpdateFrame()
	{
		// 프레임 카운트가 0일 때만 시작 시간을 측정합니다.
		if (m_frameCount == 0)
		{
			m_frameTime.MeasureStartTime();
		}

		m_frameTime.MeasureEndTime();
		++m_frameCount;

		// 1초가 넘어가면 프레임 체크
		if (m_frameTime.CalcTimeInterval() >= CLOCKS_PER_SEC)
		{
			// FPS = 프레임 카운트 / 1초
			m_FPS = m_frameCount * CLOCKS_PER_SEC / m_frameTime.getIntervalTime();
			//RXDEBUGLOG("현재 프레임 카운트 %d, FPS : %d", m_frameCount, m_FPS);
			InitFrame();
		}
	}

} // namespace RX end
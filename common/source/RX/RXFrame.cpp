/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <�ۼ� ��¥>
 * 2018-08-12
 *
 * <���� ����>
 * �������� �ٷ�� Ŭ�����Դϴ�.
 * ����� ���� �����ӿ����� ���ǰ� �ֽ��ϴ�.
 *
 ====================================================================================*/
#include "PCH.h"
#include "RXFrame.h"

namespace RX
{

	RXFrame::RXFrame()
	{
		m_frameSec = 0.0f;
		m_frameCnt = 0;
		m_FPS      = 0;
	}

	RXFrame::~RXFrame()
	{

	}

	void RXFrame::InitFrame()
	{
		m_frameTime.SubstituteStartTimeWithEndTime();
		m_frameSec = 0.0f;
		m_frameCnt = 0;
	}

	void RXFrame::UpdateFrame()
	{
		// ������ ī��Ʈ�� 0�� ���� ���� �ð��� �����մϴ�.
		if (m_frameCnt == 0)
		{
			m_frameTime.MeasureStartTime();
		}

		m_frameTime.MeasureEndTime();
		m_frameSecond += m_frameTime.CalcDeltaSecond();
		++m_frameCnt;

		// 1�ʰ� �Ѿ�� ������ üũ
		if (m_frameSecond >= ONE_SECOND)
		{
			// FPS = ������ ī��Ʈ / 1��
			m_FPS = static_cast<INT32>(m_frameCnt / m_frameSecond);
			RXDEBUGLOG("���� ������ ī��Ʈ %d, FPS : %d", m_frameCnt, m_FPS);
			InitFrame();
		}
	}

} // namespace RX end
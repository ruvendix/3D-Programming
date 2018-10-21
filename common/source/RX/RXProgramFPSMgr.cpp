/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <�ۼ� ��¥>
 * 2018-10-21
 *
 * <���� ����>
 * ���α׷��� �������� �����ϴ� �Ŵ����Դϴ�.
 * �ܼ��� FPS�� �����ϴ� �� ���� FPS�� ���� �����̵� �� �� �ֽ��ϴ�.
 * �⺻ ������ �������� ������ (���� ������ + ������ ��Ű��)�Դϴ�.
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

		// 1�ʰ� �Ѿ�� ������ üũ
		if (m_frameSec >= ONE_SECOND)
		{
			// FPS = ������ ī��Ʈ / 1��
			// �� �� ������ ������ ���� (������ ī��Ʈ / ��� �ð�)�� �̿��մϴ�.
			m_FPS = static_cast<INT32>(m_frameCnt / m_frameSec);
			RXDEBUGLOG("���� ������ ī��Ʈ %d, FPS : %d", m_frameCnt, m_FPS);
			InitFrame();
		}
	}

} // namespace RX end
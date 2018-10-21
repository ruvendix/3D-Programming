/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <�ۼ� ��¥>
 * 2018-08-12
 *
 * <���� ����>
 * �ð��� �ٷ�� Ŭ�����Դϴ�.
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

		// ���ػ� Ÿ�̸��� ���ļ��� ��´�.
		// ���ļ��� �ʴ� �������� ���ϰ� ������ Hz(�츣��)�Դϴ�.
		// ���� ��� 60Hz�� �ʴ� 60�� �����Ѵٴ� ���Դϴ�.
		// �ֱ�� 1����Ŭ(Cycle)�� �ɸ��� �ð��� ���մϴ�.
		// ����Ŭ�� ���� �� ���� �ǹ��մϴ�. ���� �ֱ�� = (1 / ���ļ�)�Դϴ�.
		::QueryPerformanceFrequency(&m_highFrequency);
	}

	RXTimer::~RXTimer()
	{

	}

	FLOAT RXTimer::CalcDeltaSecond()
	{
		m_dwEndTime = ::timeGetTime();

		// �и������� ����, 1000�� 1��
		m_rDeltaSecond = ((m_dwEndTime - m_dwStartTime) * 0.001f);

		m_dwStartTime = m_dwEndTime;
		return m_rDeltaSecond;
	}

	// ������ ���� �ð����� �۴ٸ� FPS�� ���� ���� ���� �ð��� ��ȯ�մϴ�.
	FLOAT RXTimer::CalcFixedDeltaSecondByFPS(FPS_TYPE type)
	{
		FLOAT rBaseSecond = 0.0f;
		(type == FPS_TYPE::SIXTY) ?
			(rBaseSecond = FIXED_DELTASECOND_60FPS) : (rBaseSecond = FIXED_DELTASECOND_30FPS);

		m_dwEndTime = ::timeGetTime();

		// �и������� ����, 1000�� 1��
		m_rDeltaSecond = ((m_dwEndTime - m_dwStartTime) * 0.001f);

		if (m_rDeltaSecond < rBaseSecond) // ������ ���� �ð����� ���� ��
		{
			// ���� ������ �ð� ���ݸ�ŭ CPU�� �޽��մϴ�.
			// �ܼ��� �������� ���ڰ� �ƴ϶� ������ �����̸� �̴ϴ�.
			// ��, ���α׷� �ӵ��� ������ �ݴϴ�.
			FLOAT rSleepSecond = (rBaseSecond - m_rDeltaSecond) * CLOCKS_PER_SEC;
			::SleepEx(static_cast<DWORD>(rSleepSecond), TRUE);

			// FPS�� ���� ���� ������ �ð� ������ �����մϴ�.
			// �̹� ������ ���� ������ �ð� ���ݸ�ŭ CPU�� �޽������Ƿ� ���� �ð����� �¾ƶ������ϴ�.
			m_rDeltaSecond = rBaseSecond;
		}

		m_dwStartTime = m_dwEndTime;
		return m_rDeltaSecond;
	}

	// ���ػ� Ÿ�̸Ӹ� �̿��� ��Ÿ������ ��� ����Դϴ�.
	// ���� �ڵ� : http://www.tipssoft.com/bulletin/board.php?bo_table=FAQ&wr_id=735
	FLOAT RXTimer::CalcHigResoultionDeltaSecond()
	{
		// ���� ������ CPU Ŭ������ �˾Ƴ��ϴ�.
		QueryPerformanceCounter(&m_highEndTime);

		// �и������� ������ ��Ÿ�����带 ���մϴ�.
		// (���� ������ CPU Ŭ���� - ���� ������ CPU Ŭ����)��
		// ���ļ��� ���ϴ� ������ŭ ���������� �����ݴϴ�. �и������� ������ 1000�Դϴ�.
		m_rDeltaSecond = 
			static_cast<FLOAT>(m_highEndTime.QuadPart - m_highStartTime.QuadPart) /
			                   (m_highFrequency.QuadPart / 1000);

		// �ʴ��� �������� ���� ����մϴ�.
		//FLOAT rDeltaMilliSec = m_rDeltaSecond * 0.001f;
		//FLOAT rDeltaMicroSec = m_rDeltaSecond * 0.000001f;
		//RXLOG("MilliSec(%f) MicroSec(%f)", rDeltaMilliSec, rDeltaMicroSec);

		// �и������带 �ʴ����� �ٲ�� �մϴ�.
		m_rDeltaSecond *= 0.001f;

		m_highStartTime = m_highEndTime;
		return m_rDeltaSecond;
	}

} // namespace RX end
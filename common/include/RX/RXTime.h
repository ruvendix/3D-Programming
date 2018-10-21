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
#ifndef RXTIME_H_
#define RXTIME_H_

#include "../common.h"

namespace RX
{

	class DLL_DEFINE RXTime
	{
	public:
		RXTime();
		~RXTime();

		void MeasureStartTime()
		{
			m_dwStartTime = ::timeGetTime();
		}

		void MeasureEndTime()
		{
			m_dwEndTime = ::timeGetTime();
		}

		void SubstituteStartTimeWithEndTime()
		{
			m_dwStartTime = m_dwEndTime;
		}

		DWORD CalcDeltaSecond()
		{
			// �и������� ����, 1000�� 1��
			FLOAT rDeltaSec = ((m_dwEndTime - m_dwStartTime) * 0.0001f);

			m_dwEndTime = m_dwStartTime;
			return rDeltaSec;
		}

		// ====================================================================================
		// Getter
		DWORD getStartTime() const noexcept
		{
			return m_dwStartTime;
		}

		DWORD getEndTime() const noexcept
		{
			return m_dwEndTime;
		}

	private:
		// ====================================================================================
		// �⺻ ����
		DWORD    m_dwStartTime;
		DWORD    m_dwEndTime;
	};

} // namespace RX end

#endif
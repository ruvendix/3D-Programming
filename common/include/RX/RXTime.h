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
			// 밀리세컨드 단위, 1000이 1초
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
		// 기본 정보
		DWORD    m_dwStartTime;
		DWORD    m_dwEndTime;
	};

} // namespace RX end

#endif
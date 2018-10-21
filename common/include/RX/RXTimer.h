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
#ifndef RXTIMER_H_
#define RXTIMER_H_

#include "../common.h"

namespace RX
{

	class DLL_DEFINE RXTimer
	{
	public:
		RXTimer();
		~RXTimer();

		FLOAT CalcDeltaSecond();
		FLOAT CalcFixedDeltaSecondByFPS(FPS_TYPE type);
		FLOAT CalcHigResoultionDeltaSecond();

		// ====================================================================================
		// Getter
		FLOAT getDeltaSecond() const noexcept
		{
			return m_rDeltaSecond;
		}

	private:
		// ====================================================================================
		// 기본 정보
		FLOAT    m_rDeltaSecond;
		DWORD    m_dwStartTime;
		DWORD    m_dwEndTime;

		// ====================================================================================
		// 고해상도 타이머입니다. LARGE_INTEGER는 8바이트 자료형입니다.
		// 고해상도 타이머의 주파수(초당 진동수)와 CPU의 클럭수를 이용합니다.
		LARGE_INTEGER m_highStartTime; // 측정을 시작할 CPU의 클럭수
		LARGE_INTEGER m_highEndTime;   // 측정을 완료할 CPU의 클럭수
		LARGE_INTEGER m_highFrequency; // CPU의 주파수
	};

} // namespace RX end

#endif
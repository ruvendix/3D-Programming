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

		FLOAT CalcDeltaSeconds();
		FLOAT CalcFixedDeltaSecondsByFPS(FPS_TYPE type);
		FLOAT CalcHigResoultionDeltaSeconds();

		// ====================================================================================
		// Getter
		FLOAT getDeltaSeconds() const noexcept
		{
			return m_rDeltaSeconds;
		}

		DWORD getPrevFrameTime() const noexcept
		{
			return m_dwPrevFrameTime;
		}

		DWORD getCurrentFrameTime() const noexcept
		{
			return m_dwCurrentFrameTime;
		}

		LONGLONG getHighPrevTick() const noexcept
		{
			return m_highPrevTick.QuadPart;
		}

		LONGLONG getHighCureentTick() const noexcept
		{
			return m_highCurrentTick.QuadPart;
		}

	private:
		bool  m_bInit;              // 타이머 첫 작동 여부입니다.
		FLOAT m_rDeltaSeconds;       // 프레임 간의 시간 차이입니다. 단위는 초입니다.
		DWORD m_dwPrevFrameTime;    // 이전 프레임 시간입니다.
		DWORD m_dwCurrentFrameTime; // 현재 프레임 시간입니다.

		// 고해상도 타이머입니다. LARGE_INTEGER는 8바이트 자료형입니다.
		// 고해상도 타이머의 주파수(초당 진동수)와 CPU의 클럭수를 이용합니다.
		LARGE_INTEGER m_highPrevTick;    // 이전 프레임에서 CPU의 클럭수
		LARGE_INTEGER m_highCurrentTick; // 현재 프레임에서 CPU의 클럭수
		LARGE_INTEGER m_highFrequency;   // CPU의 주파수
	};

} // namespace RX end

#endif
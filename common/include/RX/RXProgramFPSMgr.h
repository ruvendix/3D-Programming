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
#ifndef RXPROGRAMFPSMGR_H_
#define RXPROGRAMFPSMGR_H_

#include "common.h"
#include "RXTimer.h"

namespace RX
{

	class DLL_DEFINE RXProgramFPSMgr
	{
		PHOENIX_SINGLETON_CTOR(RXProgramFPSMgr);
	public:
		RXProgramFPSMgr();

		// 전체 프레임을 업데이트합니다.
		void UpdateFrame();

		// FPS를 문자열로 변환합니다.
		const WCHAR* ConvertFPSToString();

		// ====================================================================================
		// Getter
		FLOAT getTimeScale() const noexcept
		{
			return m_rTimeScale;
		}

		const RXTimer* getTimer() const noexcept
		{
			return &m_frameTimer;
		}

		// ====================================================================================
		// Setter
		void setTimeScale(FLOAT rTimeScale)
		{
			m_rTimeScale = rTimeScale;
		}

	private:
		INT32   m_frameCnt;   // 프레임 개수입니다.
		FLOAT   m_rFrameSec;  // 프레임 누적 시간입니다.
		FLOAT   m_rTimeScale; // 타임스케일입니다. (값이 크면 빨라지고, 작으면 느려집니다)
		DWORD   m_FPS;        // 초당 프레임 개수입니다. (Frame Per Second)
		RXTimer m_frameTimer; // 프레임을 측정하기 위한 타이머입니다.
		WCHAR   m_szFPS[DEFAULT_STRING_LENGTH]; // 프레임을 문자열로 저장하기 위한 것입니다.
	};

} // namespace RX end

#endif
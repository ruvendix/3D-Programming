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

#include "../common.h"
#include "RXTimer.h"

namespace RX
{

	class DLL_DEFINE RXProgramFPSMgr
	{
		PHOENIX_SINGLETON_CTOR(RXProgramFPSMgr);
	public:
		RXProgramFPSMgr();

		void InitFrame();
		void UpdateFrame();
		
		// ====================================================================================
		// Getter
		DWORD getFPS() const
		{
			return m_FPS;
		}

	private:
		// ====================================================================================
		// 기본 정보
		INT32      m_frameCnt;
		FLOAT      m_frameSec;
		DWORD      m_FPS;
		RXTimer    m_frameTimer;
	};

} // namespace RX end

#endif
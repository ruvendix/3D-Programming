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
#ifndef RXFRAME_H_
#define RXFRAME_H_

#include "../common.h"
#include "RXTime.h"

namespace RX
{

	class DLL_DEFINE RXFrame
	{
	public:
		RXFrame();
		virtual ~RXFrame();

		void InitFrame();
		void UpdateFrame();

		// getter
		DWORD getFPS() const
		{
			return m_FPS;
		}

	private:
		// ====================================================================================
		// 기본 정보
		INT32     m_frameCount;
		INT32     m_framePerSecond;
		DWORD     m_FPS;
		RXTime    m_frameTime;
	};

} // namespace RX end

#endif
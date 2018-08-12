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
#include "stdafx.h"
#include "RXTime.h"

namespace RX
{

	RXTime::RXTime()
	{
		m_dwStartTime = 0;
		m_dwEndTime = 0;
		m_dwIntervalTime = 0;
	}

	RXTime::~RXTime()
	{

	}

} // namespace RX end
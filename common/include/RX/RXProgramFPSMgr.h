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
		// �⺻ ����
		INT32      m_frameCnt;
		FLOAT      m_frameSec;
		DWORD      m_FPS;
		RXTimer    m_frameTimer;
	};

} // namespace RX end

#endif
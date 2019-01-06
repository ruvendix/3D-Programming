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

#include "common.h"
#include "RXTimer.h"

namespace RX
{

	class DLL_DEFINE RXProgramFPSMgr
	{
		PHOENIX_SINGLETON_CTOR(RXProgramFPSMgr);
	public:
		RXProgramFPSMgr();

		// ��ü �������� ������Ʈ�մϴ�.
		void UpdateFrame();

		// FPS�� ���ڿ��� ��ȯ�մϴ�.
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
		INT32   m_frameCnt;   // ������ �����Դϴ�.
		FLOAT   m_rFrameSec;  // ������ ���� �ð��Դϴ�.
		FLOAT   m_rTimeScale; // Ÿ�ӽ������Դϴ�. (���� ũ�� ��������, ������ �������ϴ�)
		DWORD   m_FPS;        // �ʴ� ������ �����Դϴ�. (Frame Per Second)
		RXTimer m_frameTimer; // �������� �����ϱ� ���� Ÿ�̸��Դϴ�.
		WCHAR   m_szFPS[DEFAULT_STRING_LENGTH]; // �������� ���ڿ��� �����ϱ� ���� ���Դϴ�.
	};

} // namespace RX end

#endif
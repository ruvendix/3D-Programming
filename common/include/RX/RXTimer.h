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
		bool  m_bInit;              // Ÿ�̸� ù �۵� �����Դϴ�.
		FLOAT m_rDeltaSeconds;       // ������ ���� �ð� �����Դϴ�. ������ ���Դϴ�.
		DWORD m_dwPrevFrameTime;    // ���� ������ �ð��Դϴ�.
		DWORD m_dwCurrentFrameTime; // ���� ������ �ð��Դϴ�.

		// ���ػ� Ÿ�̸��Դϴ�. LARGE_INTEGER�� 8����Ʈ �ڷ����Դϴ�.
		// ���ػ� Ÿ�̸��� ���ļ�(�ʴ� ������)�� CPU�� Ŭ������ �̿��մϴ�.
		LARGE_INTEGER m_highPrevTick;    // ���� �����ӿ��� CPU�� Ŭ����
		LARGE_INTEGER m_highCurrentTick; // ���� �����ӿ��� CPU�� Ŭ����
		LARGE_INTEGER m_highFrequency;   // CPU�� ���ļ�
	};

} // namespace RX end

#endif
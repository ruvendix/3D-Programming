/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <�ۼ� ��¥>
 * 2018-08-12
 *
 * <���� ����>
 * _tWinMain()�� �ھ� ���� ��ƾ�Դϴ�.
 * �ܺ� ������Ʈ���� ����ϸ� ���ϰ� ���α׷����� �� �� �ֽ��ϴ�.
 * ���� Ŭ���̵� �����ϹǷ� �����ο� ������ �����մϴ�.
 *
 ====================================================================================*/
#ifndef MAIN_SUB_H_
#define MAIN_SUB_H_

#include "common.h"

namespace RX
{

	class DLL_DEFINE MainSub
	{
	public:
		MainSub();
		virtual ~MainSub();

		HRESULT InitInstance();
		HRESULT CreateProgramWindow();
		HRESULT RunMainRoutine(HINSTANCE hInst, HWND* phMainWnd, INT32 iconID = 0);
		
		virtual HRESULT InitMain();
		virtual HRESULT DriveMain();
		virtual HRESULT Release();

		// ====================================================================================
		// getter
		ROUTINE_STATE getRoutineState() const noexcept
		{
			return m_routineState;
		}

		INT32 getMessageCode() const noexcept
		{
			return m_msgCode;
		}

		// ====================================================================================
		// setter
		void setWndProc(WNDPROC wndProc)
		{
			m_wndProc = wndProc;
		}

		void setSubFunc(SubFunc subFunc)
		{
			m_subFunc = subFunc;
		}

	protected:
		// ====================================================================================
		// �⺻ ����
		HWND             m_hMainWnd;
		HINSTANCE        m_hInst;
		WNDPROC          m_wndProc;
		ROUTINE_STATE    m_routineState;
		INT32            m_msgCode;
		SubFunc          m_subFunc;

		// ====================================================================================
		// ���ҽ� ����
		INT32            m_iconID;
	};

} // namespace RX end

#endif
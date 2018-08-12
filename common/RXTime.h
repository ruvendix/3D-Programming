/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-08-12
 *
 * <파일 내용>
 * _tWinMain()의 코어 서브 루틴입니다.
 * 외부 프로젝트에서 사용하면 편하게 프로그래밍을 할 수 있습니다.
 * 서브 클래싱도 지원하므로 자유로운 수정도 가능합니다.
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
		// 기본 정보
		HWND             m_hMainWnd;
		HINSTANCE        m_hInst;
		WNDPROC          m_wndProc;
		ROUTINE_STATE    m_routineState;
		INT32            m_msgCode;
		SubFunc          m_subFunc;

		// ====================================================================================
		// 리소스 정보
		INT32            m_iconID;
	};

} // namespace RX end

#endif
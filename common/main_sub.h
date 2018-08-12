/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-08-11
 *
 * <파일 내용>
 * 공동으로 사용되는 전역 함수 모음입니다.
 * 함수 선언부만 있으며, 구현부는 각 프로젝트에 포함시켜야 합니다.
 * 매크로에 영향을 받지 않습니다.
 * DLL로 변경되었으므로 DLL 선언부와 구현부로 나눠집니다.
 * 절대로 프로젝트와 전역 변수가 겹치면 안 됩니다. 겹치면 정상 작동이 보장되지 않습니다.
 *
 ====================================================================================*/
#ifndef RXMAINSUB_H_
#define RXMAINSUB_H_

#include "RXCmn.h"

namespace RX
{

	class RXMainSub : public RXNecessity
	{
	public:
		RXMainSub();
		explicit RXMainSub(const HINSTANCE hInst);
		virtual ~RXMainSub();

		virtual HRESULT InitMain(HINSTANCE hInst);
		HRESULT         InitWndClass();
		HRESULT         InitInstance();
		virtual void    InitGlobalVariables();

		virtual HRESULT DriveMain();
		virtual HRESULT Release() override;
		virtual void    RunMainRoutine();

		// getter

		// setter
		void setWndProc(WNDPROC wndProc)
		{
			m_wndProc = wndProc;
		}

	protected:
		// 기본 정보
		HWND         m_hMainWnd;
		HINSTANCE    m_hInst;
		WNDPROC      m_wndProc;
		DWORD        m_dwStyle;
		WCHAR        m_szWndClass[DEFAULT_STRING_LENGTH];
		WCHAR        m_szProgram[DEFAULT_STRING_LENGTH];
	};

} // namespace RX end
/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <�ۼ� ��¥>
 * 2018-08-11
 *
 * <���� ����>
 * �������� ���Ǵ� ���� �Լ� �����Դϴ�.
 * �Լ� ����θ� ������, �����δ� �� ������Ʈ�� ���Խ��Ѿ� �մϴ�.
 * ��ũ�ο� ������ ���� �ʽ��ϴ�.
 * DLL�� ����Ǿ����Ƿ� DLL ����ο� �����η� �������ϴ�.
 * ����� ������Ʈ�� ���� ������ ��ġ�� �� �˴ϴ�. ��ġ�� ���� �۵��� ������� �ʽ��ϴ�.
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
		// �⺻ ����
		HWND         m_hMainWnd;
		HINSTANCE    m_hInst;
		WNDPROC      m_wndProc;
		DWORD        m_dwStyle;
		WCHAR        m_szWndClass[DEFAULT_STRING_LENGTH];
		WCHAR        m_szProgram[DEFAULT_STRING_LENGTH];
	};

} // namespace RX end
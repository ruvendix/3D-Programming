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
#ifndef RXMAIN_H_
#define RXMAIN_H_

#include "common.h"

namespace RX
{

	class DLL_DEFINE RXMain
	{
	public:
		RXMain();
		virtual ~RXMain();

		HRESULT InitInstance();
		HRESULT CreateProgramWindow();

		virtual HRESULT RunMainRoutine(HINSTANCE hInst, INT32 iconID = 0);
		virtual HRESULT InitMain();
		virtual HRESULT DriveMain();
		virtual HRESULT Update();
		virtual HRESULT Render();
		virtual HRESULT Release();
		virtual HRESULT ResizeResolution(INT32 clientWidth, INT32 clientHeight);
		virtual HRESULT ToggleFullScreenMode(bool bFullScreen = false);

		void ChangeProgramTitle(const TCHAR* szTitle);

		// ���ϴ� Ŭ���̾�Ʈ �ػ󵵸� ������
		// ������ �������� ����ؼ� �������ݴϴ�.
		void AdjustProgramRange(INT32 width, INT32 height);

		bool IsFullScreen() const noexcept
		{
			return (m_bFullScreen == true);
		}

		// ====================================================================================
		// Getter
		ROUTINE_STATE getRoutineState() const noexcept
		{
			return m_routineState;
		}

		INT32 getMessageCode() const noexcept
		{
			return m_msgCode;
		}

		HINSTANCE getProgramInstanceHandle() const noexcept
		{
			return m_hInst;
		}

		HWND getMainWindowHandle() const noexcept
		{
			return m_hMainWnd;
		}

		HMONITOR getMainMonitorHandle() const noexcept
		{
			return m_hMainMonitor;
		}

		RECT getClientRect() const noexcept
		{
			return m_rtClient;
		}

		RECT getWindowRect() const noexcept
		{
			return m_rtWindow;
		}

		INT32 getClientWidth() const noexcept
		{
			return (m_rtClient.right - m_rtClient.left);
		}

		INT32 getClientHeight() const noexcept
		{
			return (m_rtClient.bottom - m_rtClient.top);
		}

		// ====================================================================================
		// Setter
		void setWndProc(WNDPROC wndProc)
		{
			::SetWindowLongPtr(m_hMainWnd, GWLP_WNDPROC, reinterpret_cast<LONG>(wndProc));
		}

		void setSubFunc(SubFunc subFunc, SUBFUNC_TYPE type)
		{
			m_subFunc[static_cast<INT32>(type)].subFunc = subFunc;
		}

		void setFullScreen(bool bFullScreen)
		{
			m_bFullScreen = bFullScreen;
		}

	protected:
		// ====================================================================================
		// �⺻ ����
		bool             m_bFullScreen;
		HWND             m_hMainWnd;
		HMONITOR         m_hMainMonitor;
		HINSTANCE        m_hInst;
		ROUTINE_STATE    m_routineState;
		INT32            m_msgCode;
		RECT             m_rtClient;
		RECT             m_rtWindow;
		SubFuncInfo      m_subFunc[SubFuncInfo::MAX_SUBFUNC];

		// ====================================================================================
		// ���ҽ� ����
		INT32            m_iconID;
	};

} // namespace RX end

#endif
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

		// 원하는 클라이언트 해상도를 넣으면
		// 윈도우 영역까지 계산해서 적용해줍니다.
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
		bool          m_bFullScreen;  // 전체 화면 여부입니다.
		HWND          m_hMainWnd;     // 프로그램 주 창 핸들입니다.
		HMONITOR      m_hMainMonitor; // 주 모니터입니다.
		HINSTANCE     m_hInst;        // 프로그램 인스턴스 핸들입니다.
		ROUTINE_STATE m_routineState; // 프로그램 루틴 상태입니다.
		INT32         m_msgCode;      // 메시지 핸들러의 코드값입니다.
		INT32         m_iconID;       // 프로그램 아이콘 아이디입니다.
		RECT          m_rtClient;     // 프로그램 클라이언트 영역입니다.
		RECT          m_rtWindow;     // 프로그램 전체 영역입니다.
		SubFuncInfo   m_subFunc[SubFuncInfo::MAX_SUBFUNC]; // 콜백 서브 루틴입니다.
	};

} // namespace RX end

#endif
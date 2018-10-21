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
#include "PCH.h"
#include "RXMain.h"
#include "RXRendererDX9.h"
#include "RXWindowProcedure.h"

extern RX::RXMain* g_pMain    = nullptr;
extern HWND        g_hMainWnd = nullptr;

HRESULT CALLBACK DefaultSubInit();
HRESULT CALLBACK DefaultSubUpdate();
HRESULT CALLBACK DefaultSubRender();
HRESULT CALLBACK DefaultSubRelease();
HRESULT CALLBACK DefaultSubLostDevice();
HRESULT CALLBACK DefaultSubResetDevice();

namespace
{
	SubFunc subFuncTable[SubFuncInfo::MAX_SUBFUNC] =
	{
		DefaultSubInit, DefaultSubUpdate, DefaultSubRender,
		DefaultSubRelease, DefaultSubLostDevice, DefaultSubResetDevice
	};
}

namespace RX
{

	RXMain::RXMain()
	{
		g_pMain = this;

		m_bFullScreen               = false;
		m_hMainWnd                  = nullptr;
		m_hMainMonitor              = nullptr;
		m_hInst                     = nullptr;
		m_routineState              = ROUTINE_STATE::NORMAL;
		m_msgCode                   = 0;

		::SetRect(&m_rtClient, 0, 0, 0, 0);
		::SetRect(&m_rtWindow, 0, 0, 0, 0);

		for (INT32 i = 0; i < SubFuncInfo::MAX_SUBFUNC; ++i)
		{
			m_subFunc[i].Reset();
		}

		m_iconID = 0;
	}

	RXMain::~RXMain()
	{

	}

	HRESULT RXMain::InitMain()
	{
#if defined(DEBUG) || defined(_DEBUG)
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

		if (FAILED(InitInstance()))
		{
			return E_FAIL;
		}

		if (FAILED(CreateProgramWindow()))
		{
			return E_FAIL;
		}

		for (INT32 i = 0; i < SubFuncInfo::MAX_SUBFUNC; ++i)
		{
			// subFunc가 nullptr이 아니면 이미 등록된 상태입니다.
			if (m_subFunc[i].subFunc == nullptr)
			{
				m_subFunc[i].subFunc = subFuncTable[i];
			}
		}

		return S_OK;
	}

	HRESULT RXMain::InitInstance()
	{
		WNDCLASSEX wcex;
		::ZeroMemory(&wcex, sizeof(wcex));

		wcex.cbSize      = sizeof(wcex);
		wcex.style       = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = DefaultWndProc;
		wcex.cbClsExtra  = 0;
		wcex.cbWndExtra  = 0;
		wcex.hInstance   = m_hInst;
		
		if (m_iconID != 0)
		{
			wcex.hIcon = ::LoadIcon(m_hInst, MAKEINTRESOURCE(m_iconID));
		}
		else
		{
			wcex.hIcon = ::LoadIcon(nullptr, 0);
		}

		wcex.hCursor       = ::LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = static_cast<HBRUSH>(::GetStockObject(GRAY_BRUSH));
		wcex.lpszMenuName  = SZ_NULL;
		wcex.lpszClassName = SZ_WINDOW_CLASS;
		wcex.hIconSm       = wcex.hIcon;

		if (::RegisterClassEx(&wcex) == 0)
		{
			RXERRLOG_RETURN_EFAIL("프로그램 초기화 실패했습니다!");
		}
		
		RXLOG("프로그램 초기화 성공했습니다!");
		return S_OK;
	}

	HRESULT RXMain::CreateProgramWindow()
	{
		DWORD dwStyle;
		if (m_bFullScreen)
		{
			dwStyle = WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP;
		}
		else
		{
			dwStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
		}

		// 클라이언트 영역의 크기를 조정해줍니다.
		// 프레임 윈도우를 포함해서 순수하게 클라이언트 영역의 크기만 계산합니다.
		// 즉, 기존 창의 크기보다 더 커집니다.
		AdjustProgramRange(DEFAULT_CLIENT_WIDTH, DEFAULT_CLIENT_HEIGHT);

		INT32 fullClientWidth  = m_rtWindow.right - m_rtWindow.left;
		INT32 fullClientHeight = m_rtWindow.bottom - m_rtWindow.top;

		// 현재 모니터 해상도에 설정된 값을 가져옵니다.
		// 현재 설정된 해상도가 1920 X 1080이라면
		// screenWidth에는 1920이 screenHeight에는 1080이 대입됩니다.
		INT32 screenWidth  = ::GetSystemMetrics(SM_CXSCREEN);
		INT32 screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

		// 프로그램 창을 생성합니다.
		// 창의 위치는 모니터 해상도 기준으로 정가운데입니다.
		// 예를 들어 1024 X 768의 클라이언트 영역과 1920 X 1080의 모니터 영역이 있을 때
		// X 좌표는 ((1920 - 1024) / 2), Y 좌표는 ((1080 - 768) / 2)이 됩니다.
		// 조정된 클라이언트 영역의 크기를 포함해서 프로그램 창을 생성해야 하므로
		// rtClient.right - rtClient.left로 설정해야 합니다.
		m_hMainWnd = ::CreateWindow(SZ_WINDOW_CLASS, SZ_PROGRAM_TITLE, dwStyle,
			(screenWidth - fullClientWidth) / 2,
			(screenHeight - fullClientHeight) / 2,
			fullClientWidth, fullClientHeight,
			::GetDesktopWindow(), // 바탕화면을 부모 창으로 설정합니다.
			nullptr, m_hInst, nullptr);

		NULLCHK_RETURN_EFAIL(m_hMainWnd, "프로그램 창 생성 실패했습니다!");
		::ShowWindow(m_hMainWnd, SW_NORMAL);
		::UpdateWindow(m_hMainWnd);
		::ShowCursor(TRUE);
		g_hMainWnd = m_hMainWnd;

		m_hMainMonitor = FindCurrentMonitorHandle(m_hMainWnd);
		NULLCHK_RETURN_EFAIL(m_hMainMonitor, "주모니터가 없습니다!")

		RXLOG("프로그램 창 생성 성공했습니다!");
		return S_OK;
	}

	HRESULT RXMain::DriveMain()
	{
		MSG msg;
		::ZeroMemory(&msg, sizeof(msg));

		bool m_bDriveFailure = false;

		for ( ; ; )
		{
			// 외부에서도 별개로 종료 컨트롤이 가능합니다.
			if (m_routineState == ROUTINE_STATE::EXIT)
			{
				m_msgCode = 0;
				break;
			}

			if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					m_routineState = ROUTINE_STATE::EXIT;
					m_msgCode = msg.wParam;
					break;
				}

				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			else
			{
				Update();
				Render();

				if (m_routineState == ROUTINE_STATE::FAILURE)
				{
					m_msgCode = 0;
					m_bDriveFailure = true;
					break;
				}
			}
		}

		if (m_bDriveFailure)
		{
			return E_FAIL;
		}

		return S_OK;
	}

	HRESULT RXMain::Update()
	{
		if (FAILED(m_subFunc[static_cast<INT32>(SUBFUNC_TYPE::UPDATE)].subFunc()))
		{
			RXERRLOG_RETURN_EFAIL("서브 업데이트 실패했습니다!");
		}

		return S_OK;
	}

	HRESULT RXMain::Render()
	{
		if (FAILED(m_subFunc[static_cast<INT32>(SUBFUNC_TYPE::RENDER)].subFunc()))
		{
			RXERRLOG_RETURN_EFAIL("서브 렌더 실패했습니다!");
		}

		return S_OK;
	}

	HRESULT RXMain::Release()
	{
		if (FAILED(m_subFunc[static_cast<INT32>(SUBFUNC_TYPE::RELEASE)].subFunc()))
		{
			RXERRLOG_RETURN_EFAIL("서브 릴리즈 실패했습니다!");
		}
		::UnregisterClass(SZ_WINDOW_CLASS, m_hInst);
		return S_OK;
	}

	HRESULT RXMain::ResizeResolution(INT32 clientWidth, INT32 clientHeight)
	{
		AdjustProgramRange(clientWidth, clientHeight);

		// 현재 모니터 해상도에 설정된 값을 가져옵니다.
		// 현재 설정된 해상도가 1920 X 1080이라면
		// screenWidth에는 1920이 screenHeight에는 1080이 대입됩니다.
		INT32 screenWidth      = GetSystemMetrics(SM_CXSCREEN);
		INT32 screenHeight     = GetSystemMetrics(SM_CYSCREEN);

		INT32 fullClientWidth  = m_rtWindow.right - m_rtWindow.left;
		INT32 fullClientHeight = m_rtWindow.bottom - m_rtWindow.top;

		SetWindowLongPtr(m_hMainWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
		SetWindowPos(m_hMainWnd, HWND_TOP,
			(screenWidth - (m_rtWindow.right - m_rtWindow.left)) / 2,
			(screenHeight - (m_rtWindow.bottom - m_rtWindow.top)) / 2,
			m_rtWindow.right, m_rtWindow.bottom, SWP_NOZORDER | SWP_SHOWWINDOW);
		
		return S_OK;
	}

	void RXMain::ChangeProgramTitle(const TCHAR* szTitle)
	{
		::SetWindowText(m_hMainWnd, szTitle);
	}

	void RXMain::AdjustProgramRange(INT32 width, INT32 height)
	{
		RECT rtClient = { 0, 0, width, height };
		::CopyRect(&m_rtClient, &rtClient);

		// 원하는 클라이언트 영역을 넘겨주면 프로그램 윈도우 등을 포함해서
		// 전체 영역을 재조정합니다. 즉, 기존 영역보다 더 커집니다.
		// 재조정된 영역은 클라이언트 영역이 아닌 윈도우 영역이므로 주의해야 합니다.
		::AdjustWindowRect(&rtClient, WS_OVERLAPPEDWINDOW | WS_VISIBLE, FALSE);

		::CopyRect(&m_rtWindow, &rtClient);
	}

	HRESULT RXMain::RunMainRoutine(HINSTANCE hInst, INT32 iconID)
	{
#if defined(_UNICODE) || defined(UNICODE)
		// 기본 로케일을 한국어로 설정합니다.
		_wsetlocale(LC_ALL, L"kor");
#endif

		m_hInst = hInst;

		if (iconID != 0)
		{
			m_iconID = iconID;
		}

		if (FAILED(InitMain()))
		{
			Release();
			m_routineState = ROUTINE_STATE::FAILURE;
			m_msgCode      = 0;
			RXERRLOG_RETURN_EFAIL("프로그램 비정상 종료되었습니다!");
		}

		if (FAILED(m_subFunc[static_cast<INT32>(SUBFUNC_TYPE::INIT)].subFunc()))
		{
			RXERRLOG_RETURN_EFAIL("서브 초기화 실패했습니다!");
		}

		if (FAILED(DriveMain()))
		{
			Release();
			m_routineState = ROUTINE_STATE::FAILURE;
			m_msgCode = 0;
			RXERRLOG_RETURN_EFAIL("프로그램 비정상 종료되었습니다!");
		}

		Release();
		RXLOG("프로그램 정상 종료되었습니다!");
		return S_OK;
	}

	HRESULT RXMain::ToggleFullScreenMode(bool bFullScreen)
	{
		if (bFullScreen) // 최대화 키를 위한 변수입니다.
		{
			m_bFullScreen = bFullScreen;
		}
		else
		{
			// 전체 화면이면 창 화면으로, 창 화면이면 전체 화면으로 전환합니다.
			(m_bFullScreen == true) ? (m_bFullScreen = false) : (m_bFullScreen = true);
		}

		INT32 screenWidth  = GetSystemMetrics(SM_CXSCREEN);
		INT32 screenHeight = GetSystemMetrics(SM_CYSCREEN);

		if (m_bFullScreen) // 전체 화면 모드
		{
			SetWindowLongPtr(m_hMainWnd, GWL_STYLE, WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP);
			SetWindowPos(m_hMainWnd, HWND_TOP, 0, 0,
				screenWidth, screenHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

			RXLOG("전체 화면 모드로 전환되었습니다!");
		}
		else // 창 화면 모드
		{
			// 창 화면에서만 클라이언트 영역과 윈도우 영역을 재조정합니다.
			// 전체 화면일 때는 영역 크기를 재조정할 필요가 없습니다.
			AdjustProgramRange(DEFAULT_CLIENT_WIDTH, DEFAULT_CLIENT_HEIGHT);

			SetWindowLongPtr(m_hMainWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
			SetWindowPos(m_hMainWnd, HWND_TOP,
				(screenWidth - (m_rtWindow.right - m_rtWindow.left)) / 2,
				(screenHeight - (m_rtWindow.bottom - m_rtWindow.top)) / 2,
				m_rtWindow.right, m_rtWindow.bottom, SWP_NOZORDER | SWP_SHOWWINDOW);

			RXLOG("창 화면 모드로 전환되었습니다!");
		}

		return S_OK;
	}

} // namespace RX end

HRESULT CALLBACK DefaultSubInit()
{
	return S_OK;
}

HRESULT CALLBACK DefaultSubUpdate()
{
	return S_OK;
}

HRESULT CALLBACK DefaultSubRender()
{
	return S_OK;
}

HRESULT CALLBACK DefaultSubRelease()
{
	return S_OK;
}

HRESULT CALLBACK DefaultSubLostDevice()
{
	return S_OK;
}

HRESULT CALLBACK DefaultSubResetDevice()
{
	return S_OK;
}
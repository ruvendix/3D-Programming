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

HRESULT CALLBACK DefaultSubInit();
HRESULT CALLBACK DefaultSubUpdate();
HRESULT CALLBACK DefaultSubRender();
HRESULT CALLBACK DefaultSubRelease();
HRESULT CALLBACK DefaultSubLostDevice();
HRESULT CALLBACK DefaultSubResetDevice();

// 메시지 핸들러입니다.
void OnCreate();
void OnClose();
void OnDestroy();
void OnKeyEscape();
void OnChangeSize();
void OnMouseLButtonDown(LPARAM lParam);
void OnMouseRButtonDown(LPARAM lParam);
void OnMouseMove();
void OnGetMinMaxInfo(LPARAM lParam);
void OnMaximize();
void OnAltEnter(WPARAM wParam, LPARAM lParam);

namespace
{
	RX::RXMain* g_pThis = nullptr;
	SubFunc subFuncTable[SubFuncInfo::MAX_SUBFUNC] =
	{
		DefaultSubInit, DefaultSubUpdate, DefaultSubRender,
		DefaultSubRelease, DefaultSubLostDevice, DefaultSubResetDevice
	};
}

LRESULT CALLBACK DefaultWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
	{
		OnCreate();
		break;
	}
	case WM_CLOSE:
	{
		OnClose();
		break;
	}
	case WM_DESTROY:
	{
		OnDestroy();
		break;
	}
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_ESCAPE:
		{
			OnKeyEscape();
			break;
		}
		}

		break;
	}
	case WM_SIZE:
	{
		OnChangeSize();
		break;
	}
	case WM_LBUTTONDOWN:
	{
		OnMouseLButtonDown(lParam);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		OnMouseRButtonDown(lParam);
		break;
	}
	case WM_MOUSEMOVE:
	{
		OnMouseMove();
		break;
	}
	case WM_GETMINMAXINFO:
	{
		OnGetMinMaxInfo(lParam);
		break;
	}
	case WM_SYSCOMMAND:
	{
		switch (wParam)
		{
		case SC_MAXIMIZE:
		{
			OnMaximize();
			break;
		}
		}

		break;
	}
	}

	// Alt + Enter를 위한 처리입니다.
	switch (msg)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		OnAltEnter(wParam, lParam);
		break;
	}
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

namespace RX
{

	RXMain::RXMain()
	{
		g_pThis = this;

		m_bFullScreen               = false;
		m_hMainWnd                  = nullptr;
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
			if (m_subFunc[i].func == nullptr)
			{
				m_subFunc[i].func = subFuncTable[i];
			}
		}

		return S_OK;
	}

	HRESULT RXMain::InitInstance()
	{
		WNDCLASSEX wcex;
		::ZeroMemory(&wcex, sizeof(wcex));

		wcex.cbSize        = sizeof(wcex);
		wcex.style         = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc   = DefaultWndProc;
		wcex.cbClsExtra    = 0;
		wcex.cbWndExtra    = 0;
		wcex.hInstance     = m_hInst;
		
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
		wcex.lpszMenuName  = SZ_NONE;
		wcex.lpszClassName = SZ_WINDOW_CLASS;
		wcex.hIconSm       = wcex.hIcon;

		if (::RegisterClassEx(&wcex) == 0)
		{
			RXERRLOG_RETURN_EFAIL("프로그램 초기화 실패했습니다!");
		}
		
		RXLOG(false, "프로그램 초기화 성공했습니다!");
		return S_OK;
	}

	HRESULT RXMain::CreateProgramWindow()
	{
		// 클라이언트 영역의 크기를 설정해줍니다.
		RECT rtClient = { 0, 0, DEFAULT_CLIENT_WIDTH, DEFAULT_CLIENT_HEIGHT };
		::CopyRect(&m_rtClient, &rtClient);

		DWORD dwStyle;
		if (m_bFullScreen)
		{
			dwStyle = /*WS_EX_TOPMOST |*/ WS_VISIBLE | WS_POPUP;
		}
		else
		{
			dwStyle = WS_OVERLAPPEDWINDOW;
		}

		// 클라이언트 영역의 크기를 조정해줍니다.
		// 프레임 윈도우를 포함해서 순수하게 클라이언트 영역의 크기만 계산합니다.
		// 즉, 기존 창의 크기보다 더 커집니다.
		::AdjustWindowRect(&rtClient, dwStyle, FALSE);
		::CopyRect(&m_rtWindow, &rtClient);

		INT fullClientWidth  = rtClient.right - rtClient.left;
		INT fullClientHeight = rtClient.bottom - rtClient.top;

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

		NULLCHK_RETURN_EFAIL(m_hMainWnd, "프로그램 창 생성 실패!");
		::ShowWindow(m_hMainWnd, SW_NORMAL);
		::UpdateWindow(m_hMainWnd);
		::ShowCursor(TRUE);

		RXLOG(false, "프로그램 창 생성 성공했습니다!");
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
		if (FAILED(m_subFunc[static_cast<INT32>(SUBFUNC_TYPE::UPDATE)].func()))
		{
			RXERRLOG_RETURN_EFAIL("서브 업데이트 실패했습니다!");
		}

		return S_OK;
	}

	HRESULT RXMain::Render()
	{
		if (FAILED(m_subFunc[static_cast<INT32>(SUBFUNC_TYPE::RENDER)].func()))
		{
			RXERRLOG_RETURN_EFAIL("서브 렌더 실패했습니다!");
		}

		return S_OK;
	}

	HRESULT RXMain::Release()
	{
		if (FAILED(m_subFunc[static_cast<INT32>(SUBFUNC_TYPE::RELEASE)].func()))
		{
			RXERRLOG_RETURN_EFAIL("서브 릴리즈 실패했습니다!");
		}
		::UnregisterClass(SZ_WINDOW_CLASS, m_hInst);
		return S_OK;
	}

	HRESULT RXMain::ResizeResolution(INT32 clientWidth, INT32 clientHeight)
	{
		INT32 screenWidth  = GetSystemMetrics(SM_CXSCREEN);
		INT32 screenHeight = GetSystemMetrics(SM_CYSCREEN);

		RECT rtClient = { 0, 0, clientWidth, clientHeight };
		::CopyRect(&m_rtClient, &rtClient);
		::AdjustWindowRect(&rtClient, WS_OVERLAPPEDWINDOW, FALSE);
		::CopyRect(&m_rtWindow, &rtClient);

		SetWindowLongPtr(m_hMainWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowPos(m_hMainWnd, HWND_TOP,
			(screenWidth - (rtClient.right - rtClient.left)) / 2,
			(screenHeight - (rtClient.bottom - rtClient.top)) / 2,
			rtClient.right, rtClient.bottom, SWP_NOZORDER | SWP_SHOWWINDOW);
		
		return S_OK;
	}

	void RXMain::ChangeProgramTitle(const TCHAR* szTitle)
	{
		::SetWindowText(m_hMainWnd, szTitle);
	}

	void RXMain::AdjustClientRect()
	{
		RECT rtClient;
		GetClientRect(m_hMainWnd, &rtClient);
		::SetRect(&m_rtClient, 0, 0,
			rtClient.right - rtClient.left, rtClient.bottom - rtClient.top);
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

		if (FAILED(m_subFunc[static_cast<INT32>(SUBFUNC_TYPE::INIT)].func()))
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
		RXLOG(false, "프로그램 정상 종료되었습니다!");
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
			SetWindowLongPtr(m_hMainWnd, GWL_STYLE, /*WS_EX_TOPMOST |*/ WS_VISIBLE | WS_POPUP);
			SetWindowPos(m_hMainWnd, HWND_TOP, 0, 0,
				screenWidth, screenHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

			RXLOG(false, "전체 화면 모드로 전환되었습니다!");
		}
		else // 창 화면 모드
		{
			// AdjustWindowRect()가 호출되면 WM_GETMINMAXINFO가 발생하므로
			// 전체 화면 -> 창 화면일 때는 클라이언트 영역 조절할 때 주의해야 합니다.
			// * 무한루프가 되므로 코드 주석 처리
			//::AdjustWindowRect(&rtClient, WS_OVERLAPPEDWINDOW, false);

			SetWindowLongPtr(m_hMainWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
			SetWindowPos(m_hMainWnd, HWND_TOP,
				(screenWidth - (m_rtWindow.right - m_rtWindow.left)) / 2,
				(screenHeight - (m_rtWindow.bottom - m_rtWindow.top)) / 2,
				m_rtWindow.right, m_rtWindow.bottom, SWP_NOZORDER | SWP_SHOWWINDOW);

			RXLOG(false, "창 화면 모드로 전환되었습니다!");
		}

		AdjustClientRect();
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

void OnCreate()
{
	RXDEBUGLOG("프로그램 창을 생성했습니다.");
}

void OnClose()
{
	::DestroyWindow(g_pThis->getMainWindowHandle());
	RXDEBUGLOG("닫기 버튼을 클릭했습니다.");
}

void OnDestroy()
{
	::PostQuitMessage(0);
	RXDEBUGLOG("프로그램 창을 제거했습니다.");
}

void OnKeyEscape()
{
	::SendMessage(g_pThis->getMainWindowHandle(), WM_CLOSE, 0, 0); // 동기화 함수
	RXDEBUGLOG("ESC 키를 눌렀습니다.");
}

// 창 크기 변경이 끝났을 때 처리됩니다.
void OnChangeSize()
{
	g_pThis->AdjustClientRect();
}

void OnMouseLButtonDown(LPARAM lParam)
{
	//MSGBOX("마우스 왼쪽 버튼 클릭!");
	//g_pThis->ResizeResolution(1024, 768);
}

void OnMouseRButtonDown(LPARAM lParam)
{
	//MSGBOX("마우스 오른쪽 버튼 클릭!");
	//g_pThis->ResizeResolution(800, 600);
}

void OnMouseMove()
{
	return;
}

// 창 크기 변경이 시작될 때 항상 처리됩니다.
// 창 화면 -> 전체 화면일 때는 처리되지 않습니다.
void OnGetMinMaxInfo(LPARAM lParam)
{
	MINMAXINFO* pMinMax = reinterpret_cast<MINMAXINFO*>(lParam);
	NULLCHK_RETURN_NOCOMENT(pMinMax);

	HANDLE hMainWnd = g_pThis->getMainWindowHandle();
	NULL_RETURN(hMainWnd);

	// 전체 윈도우 영역에 적용되므로 GetClientRect()를 사용해야 하지만 
	// GetClientRect()가 아니라 미리 계산해둔 전체 윈도우 영역을 이용합니다.
	// 이유는 GetClientRect()의 갱신 시점 때문입니다.
	RECT rtWindow;
	::CopyRect(&rtWindow, &g_pThis->getWindowRect());

	// 창 크기 변경 불가능합니다.
	pMinMax->ptMinTrackSize.x = rtWindow.right - rtWindow.left;
	pMinMax->ptMinTrackSize.y = rtWindow.bottom - rtWindow.top;
	pMinMax->ptMaxTrackSize.x = rtWindow.right - rtWindow.left;
	pMinMax->ptMaxTrackSize.y = rtWindow.bottom - rtWindow.top;
}

void OnMaximize()
{
	g_pThis->ToggleFullScreenMode(true);
	RXDEBUGLOG("최대화를 클릭했습니다. 창 화면 -> 전체 화면");
}

void OnAltEnter(WPARAM wParam, LPARAM lParam)
{
	// 왼쪽 Alt + Enter입니다.
	// 오른쪽 Alt는 WM_KEYDOWN으로 들어옵니다.
	if (wParam == VK_RETURN)
	{
		if ((HIWORD(lParam) & KF_ALTDOWN)) // Alt를 눌렀는지 비트 플래그로 확인합니다.
		{
			g_pThis->ToggleFullScreenMode();
			RXDEBUGLOG("왼쪽 Alt + Enter를 눌렀습니다. 전체 화면 <-> 창 화면");
		}
	}
}
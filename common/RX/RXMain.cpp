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
#include "stdafx.h"
#include "RXMain.h"

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

namespace
{
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
		RXDEBUGLOG("프로그램 창을 생성했습니다.");
		return 0;
	}
	case WM_CLOSE:
	{
		::DestroyWindow(hWnd);
		RXDEBUGLOG("닫기 버튼을 클릭했습니다.");
		return 0;
	}
	case WM_DESTROY:
	{
		::PostQuitMessage(0);
		RXDEBUGLOG("프로그램 창을 제거했습니다.");
		return 0;
	}
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_ESCAPE:
		{
			::SendMessage(hWnd, WM_CLOSE, 0, 0); // 동기화 함수
			RXDEBUGLOG("ESC 키를 눌렀습니다.");
			break;
		}
		}

		return 0;
	}
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

namespace RX
{

	RXMain::RXMain()
	{
		m_hMainWnd     = nullptr;
		m_hInst        = nullptr;
		m_wndProc      = DefaultWndProc;
		m_routineState = ROUTINE_STATE::NORMAL;
		m_msgCode      = 0;

		for (INT32 i = 0; i < SubFuncInfo::MAX_SUBFUNC; ++i)
		{
			m_arrSubFunc[i].Reset();
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
			if (m_arrSubFunc[i].func == nullptr)
			{
				m_arrSubFunc[i].func = subFuncTable[i];
			}
		}

		if (FAILED(m_arrSubFunc[static_cast<INT32>(SUBFUNC_TYPE::INIT)].func()))
		{
			RXERRLOG_EFAIL_RETURN("서브 초기화 실패!");
		}

		return S_OK;
	}

	HRESULT RXMain::InitInstance()
	{
		WNDCLASSEX wcex;
		::ZeroMemory(&wcex, sizeof(wcex));

		wcex.cbSize        = sizeof(wcex);
		wcex.style         = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc   = m_wndProc;
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
			RXERRLOG_EFAIL_RETURN("프로그램 초기화 실패!");
		}
		
		RXLOG(false, "프로그램 초기화 성공!");
		return S_OK;
	}

	HRESULT RXMain::CreateProgramWindow()
	{
		// 클라이언트 영역의 크기를 설정해줍니다.
		RECT clientRt;
		::SetRect(&clientRt, 0, 0, DEFAULT_CLIENT_WIDTH, DEFAULT_CLIENT_HEIGHT);

		// 클라이언트 영역의 크기를 조정해줍니다.
		// 프레임 윈도우를 제외하고 순수하게 클라이언트 영역의 크기만 계산합니다.
		// 즉, g_defaultClientWidth와 g_defaultClientHeight로 설정한 값이 적용됩니다.
		::AdjustWindowRect(&clientRt, WS_OVERLAPPEDWINDOW | WS_VISIBLE, FALSE);

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
		// g_defaultClientWidth가 아니라 clientRt.right - clientRt.left로 설정해야 합니다.
		m_hMainWnd = ::CreateWindow(SZ_WINDOW_CLASS, SZ_PROGRAM_TITLE,
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			(screenWidth - (clientRt.right - clientRt.left)) / 2,
			(screenHeight - (clientRt.bottom - clientRt.top)) / 2,
			clientRt.right - clientRt.left,
			clientRt.bottom - clientRt.top,
			::GetDesktopWindow(), // 바탕화면을 부모 창으로 설정합니다.
			nullptr, m_hInst, nullptr);

		NULLCHK_EFAIL_RETURN(m_hMainWnd, "프로그램 창 생성 실패!");
		::ShowWindow(m_hMainWnd, SW_NORMAL);
		::UpdateWindow(m_hMainWnd);
		::ShowCursor(TRUE);

		RXLOG(false, "프로그램 창 생성 성공!");
		return S_OK;
	}

	HRESULT RXMain::DriveMain()
	{
		MSG msg;
		::ZeroMemory(&msg, sizeof(msg));

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
			}
		}

		return S_OK;
	}

	HRESULT RXMain::Update()
	{
		if (FAILED(m_arrSubFunc[static_cast<INT32>(SUBFUNC_TYPE::UPDATE)].func()))
		{
			RXERRLOG_EFAIL_RETURN("서브 업데이트 실패!");
		}

		return S_OK;
	}

	HRESULT RXMain::Release()
	{
		if (FAILED(m_arrSubFunc[static_cast<INT32>(SUBFUNC_TYPE::RELEASE)].func()))
		{
			RXERRLOG_EFAIL_RETURN("서브 릴리즈 실패!");
		}
		::UnregisterClass(SZ_WINDOW_CLASS, m_hInst);
		return S_OK;
	}

	void RXMain::ChangeProgramTitle(const TCHAR* szTitle)
	{
		::SetWindowText(m_hMainWnd, szTitle);
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
			RXERRLOG_EFAIL_RETURN("프로그램 비정상 종료!");
		}

		if (FAILED(DriveMain()))
		{
			Release();
			m_routineState = ROUTINE_STATE::FAILURE;
			m_msgCode = 0;
			RXERRLOG_EFAIL_RETURN("프로그램 비정상 종료!");
		}

		Release();
		RXLOG(false, "프로그램 정상 종료!");
		return S_OK;
	}

} // namespace RX end
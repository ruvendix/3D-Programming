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
			// subFunc�� nullptr�� �ƴϸ� �̹� ��ϵ� �����Դϴ�.
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
			RXERRLOG_RETURN_EFAIL("���α׷� �ʱ�ȭ �����߽��ϴ�!");
		}
		
		RXLOG("���α׷� �ʱ�ȭ �����߽��ϴ�!");
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

		// Ŭ���̾�Ʈ ������ ũ�⸦ �������ݴϴ�.
		// ������ �����츦 �����ؼ� �����ϰ� Ŭ���̾�Ʈ ������ ũ�⸸ ����մϴ�.
		// ��, ���� â�� ũ�⺸�� �� Ŀ���ϴ�.
		AdjustProgramRange(DEFAULT_CLIENT_WIDTH, DEFAULT_CLIENT_HEIGHT);

		INT32 fullClientWidth  = m_rtWindow.right - m_rtWindow.left;
		INT32 fullClientHeight = m_rtWindow.bottom - m_rtWindow.top;

		// ���� ����� �ػ󵵿� ������ ���� �����ɴϴ�.
		// ���� ������ �ػ󵵰� 1920 X 1080�̶��
		// screenWidth���� 1920�� screenHeight���� 1080�� ���Ե˴ϴ�.
		INT32 screenWidth  = ::GetSystemMetrics(SM_CXSCREEN);
		INT32 screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

		// ���α׷� â�� �����մϴ�.
		// â�� ��ġ�� ����� �ػ� �������� ������Դϴ�.
		// ���� ��� 1024 X 768�� Ŭ���̾�Ʈ ������ 1920 X 1080�� ����� ������ ���� ��
		// X ��ǥ�� ((1920 - 1024) / 2), Y ��ǥ�� ((1080 - 768) / 2)�� �˴ϴ�.
		// ������ Ŭ���̾�Ʈ ������ ũ�⸦ �����ؼ� ���α׷� â�� �����ؾ� �ϹǷ�
		// rtClient.right - rtClient.left�� �����ؾ� �մϴ�.
		m_hMainWnd = ::CreateWindow(SZ_WINDOW_CLASS, SZ_PROGRAM_TITLE, dwStyle,
			(screenWidth - fullClientWidth) / 2,
			(screenHeight - fullClientHeight) / 2,
			fullClientWidth, fullClientHeight,
			::GetDesktopWindow(), // ����ȭ���� �θ� â���� �����մϴ�.
			nullptr, m_hInst, nullptr);

		NULLCHK_RETURN_EFAIL(m_hMainWnd, "���α׷� â ���� �����߽��ϴ�!");
		::ShowWindow(m_hMainWnd, SW_NORMAL);
		::UpdateWindow(m_hMainWnd);
		::ShowCursor(TRUE);
		g_hMainWnd = m_hMainWnd;

		m_hMainMonitor = FindCurrentMonitorHandle(m_hMainWnd);
		NULLCHK_RETURN_EFAIL(m_hMainMonitor, "�ָ���Ͱ� �����ϴ�!")

		RXLOG("���α׷� â ���� �����߽��ϴ�!");
		return S_OK;
	}

	HRESULT RXMain::DriveMain()
	{
		MSG msg;
		::ZeroMemory(&msg, sizeof(msg));

		bool m_bDriveFailure = false;

		for ( ; ; )
		{
			// �ܺο����� ������ ���� ��Ʈ���� �����մϴ�.
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
			RXERRLOG_RETURN_EFAIL("���� ������Ʈ �����߽��ϴ�!");
		}

		return S_OK;
	}

	HRESULT RXMain::Render()
	{
		if (FAILED(m_subFunc[static_cast<INT32>(SUBFUNC_TYPE::RENDER)].subFunc()))
		{
			RXERRLOG_RETURN_EFAIL("���� ���� �����߽��ϴ�!");
		}

		return S_OK;
	}

	HRESULT RXMain::Release()
	{
		if (FAILED(m_subFunc[static_cast<INT32>(SUBFUNC_TYPE::RELEASE)].subFunc()))
		{
			RXERRLOG_RETURN_EFAIL("���� ������ �����߽��ϴ�!");
		}
		::UnregisterClass(SZ_WINDOW_CLASS, m_hInst);
		return S_OK;
	}

	HRESULT RXMain::ResizeResolution(INT32 clientWidth, INT32 clientHeight)
	{
		AdjustProgramRange(clientWidth, clientHeight);

		// ���� ����� �ػ󵵿� ������ ���� �����ɴϴ�.
		// ���� ������ �ػ󵵰� 1920 X 1080�̶��
		// screenWidth���� 1920�� screenHeight���� 1080�� ���Ե˴ϴ�.
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

		// ���ϴ� Ŭ���̾�Ʈ ������ �Ѱ��ָ� ���α׷� ������ ���� �����ؼ�
		// ��ü ������ �������մϴ�. ��, ���� �������� �� Ŀ���ϴ�.
		// �������� ������ Ŭ���̾�Ʈ ������ �ƴ� ������ �����̹Ƿ� �����ؾ� �մϴ�.
		::AdjustWindowRect(&rtClient, WS_OVERLAPPEDWINDOW | WS_VISIBLE, FALSE);

		::CopyRect(&m_rtWindow, &rtClient);
	}

	HRESULT RXMain::RunMainRoutine(HINSTANCE hInst, INT32 iconID)
	{
#if defined(_UNICODE) || defined(UNICODE)
		// �⺻ �������� �ѱ���� �����մϴ�.
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
			RXERRLOG_RETURN_EFAIL("���α׷� ������ ����Ǿ����ϴ�!");
		}

		if (FAILED(m_subFunc[static_cast<INT32>(SUBFUNC_TYPE::INIT)].subFunc()))
		{
			RXERRLOG_RETURN_EFAIL("���� �ʱ�ȭ �����߽��ϴ�!");
		}

		if (FAILED(DriveMain()))
		{
			Release();
			m_routineState = ROUTINE_STATE::FAILURE;
			m_msgCode = 0;
			RXERRLOG_RETURN_EFAIL("���α׷� ������ ����Ǿ����ϴ�!");
		}

		Release();
		RXLOG("���α׷� ���� ����Ǿ����ϴ�!");
		return S_OK;
	}

	HRESULT RXMain::ToggleFullScreenMode(bool bFullScreen)
	{
		if (bFullScreen) // �ִ�ȭ Ű�� ���� �����Դϴ�.
		{
			m_bFullScreen = bFullScreen;
		}
		else
		{
			// ��ü ȭ���̸� â ȭ������, â ȭ���̸� ��ü ȭ������ ��ȯ�մϴ�.
			(m_bFullScreen == true) ? (m_bFullScreen = false) : (m_bFullScreen = true);
		}

		INT32 screenWidth  = GetSystemMetrics(SM_CXSCREEN);
		INT32 screenHeight = GetSystemMetrics(SM_CYSCREEN);

		if (m_bFullScreen) // ��ü ȭ�� ���
		{
			SetWindowLongPtr(m_hMainWnd, GWL_STYLE, WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP);
			SetWindowPos(m_hMainWnd, HWND_TOP, 0, 0,
				screenWidth, screenHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

			RXLOG("��ü ȭ�� ���� ��ȯ�Ǿ����ϴ�!");
		}
		else // â ȭ�� ���
		{
			// â ȭ�鿡���� Ŭ���̾�Ʈ ������ ������ ������ �������մϴ�.
			// ��ü ȭ���� ���� ���� ũ�⸦ �������� �ʿ䰡 �����ϴ�.
			AdjustProgramRange(DEFAULT_CLIENT_WIDTH, DEFAULT_CLIENT_HEIGHT);

			SetWindowLongPtr(m_hMainWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
			SetWindowPos(m_hMainWnd, HWND_TOP,
				(screenWidth - (m_rtWindow.right - m_rtWindow.left)) / 2,
				(screenHeight - (m_rtWindow.bottom - m_rtWindow.top)) / 2,
				m_rtWindow.right, m_rtWindow.bottom, SWP_NOZORDER | SWP_SHOWWINDOW);

			RXLOG("â ȭ�� ���� ��ȯ�Ǿ����ϴ�!");
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
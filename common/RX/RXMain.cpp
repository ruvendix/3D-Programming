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
		RXDEBUGLOG("���α׷� â�� �����߽��ϴ�.");
		return 0;
	}
	case WM_CLOSE:
	{
		::DestroyWindow(hWnd);
		RXDEBUGLOG("�ݱ� ��ư�� Ŭ���߽��ϴ�.");
		return 0;
	}
	case WM_DESTROY:
	{
		::PostQuitMessage(0);
		RXDEBUGLOG("���α׷� â�� �����߽��ϴ�.");
		return 0;
	}
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_ESCAPE:
		{
			::SendMessage(hWnd, WM_CLOSE, 0, 0); // ����ȭ �Լ�
			RXDEBUGLOG("ESC Ű�� �������ϴ�.");
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
			// subFunc�� nullptr�� �ƴϸ� �̹� ��ϵ� �����Դϴ�.
			if (m_arrSubFunc[i].func == nullptr)
			{
				m_arrSubFunc[i].func = subFuncTable[i];
			}
		}

		if (FAILED(m_arrSubFunc[static_cast<INT32>(SUBFUNC_TYPE::INIT)].func()))
		{
			RXERRLOG_EFAIL_RETURN("���� �ʱ�ȭ ����!");
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
			RXERRLOG_EFAIL_RETURN("���α׷� �ʱ�ȭ ����!");
		}
		
		RXLOG(false, "���α׷� �ʱ�ȭ ����!");
		return S_OK;
	}

	HRESULT RXMain::CreateProgramWindow()
	{
		// Ŭ���̾�Ʈ ������ ũ�⸦ �������ݴϴ�.
		RECT clientRt;
		::SetRect(&clientRt, 0, 0, DEFAULT_CLIENT_WIDTH, DEFAULT_CLIENT_HEIGHT);

		// Ŭ���̾�Ʈ ������ ũ�⸦ �������ݴϴ�.
		// ������ �����츦 �����ϰ� �����ϰ� Ŭ���̾�Ʈ ������ ũ�⸸ ����մϴ�.
		// ��, g_defaultClientWidth�� g_defaultClientHeight�� ������ ���� ����˴ϴ�.
		::AdjustWindowRect(&clientRt, WS_OVERLAPPEDWINDOW | WS_VISIBLE, FALSE);

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
		// g_defaultClientWidth�� �ƴ϶� clientRt.right - clientRt.left�� �����ؾ� �մϴ�.
		m_hMainWnd = ::CreateWindow(SZ_WINDOW_CLASS, SZ_PROGRAM_TITLE,
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			(screenWidth - (clientRt.right - clientRt.left)) / 2,
			(screenHeight - (clientRt.bottom - clientRt.top)) / 2,
			clientRt.right - clientRt.left,
			clientRt.bottom - clientRt.top,
			::GetDesktopWindow(), // ����ȭ���� �θ� â���� �����մϴ�.
			nullptr, m_hInst, nullptr);

		NULLCHK_EFAIL_RETURN(m_hMainWnd, "���α׷� â ���� ����!");
		::ShowWindow(m_hMainWnd, SW_NORMAL);
		::UpdateWindow(m_hMainWnd);
		::ShowCursor(TRUE);

		RXLOG(false, "���α׷� â ���� ����!");
		return S_OK;
	}

	HRESULT RXMain::DriveMain()
	{
		MSG msg;
		::ZeroMemory(&msg, sizeof(msg));

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
			}
		}

		return S_OK;
	}

	HRESULT RXMain::Update()
	{
		if (FAILED(m_arrSubFunc[static_cast<INT32>(SUBFUNC_TYPE::UPDATE)].func()))
		{
			RXERRLOG_EFAIL_RETURN("���� ������Ʈ ����!");
		}

		return S_OK;
	}

	HRESULT RXMain::Release()
	{
		if (FAILED(m_arrSubFunc[static_cast<INT32>(SUBFUNC_TYPE::RELEASE)].func()))
		{
			RXERRLOG_EFAIL_RETURN("���� ������ ����!");
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
			RXERRLOG_EFAIL_RETURN("���α׷� ������ ����!");
		}

		if (FAILED(DriveMain()))
		{
			Release();
			m_routineState = ROUTINE_STATE::FAILURE;
			m_msgCode = 0;
			RXERRLOG_EFAIL_RETURN("���α׷� ������ ����!");
		}

		Release();
		RXLOG(false, "���α׷� ���� ����!");
		return S_OK;
	}

} // namespace RX end
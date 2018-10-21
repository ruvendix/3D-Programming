/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <�ۼ� ��¥>
 * 2018-08-21
 *
 * <���� ����>
 * ������ ���ν���(�޽��� ���ν���)���� ���ǵǾ��ֽ��ϴ�.
 *
 ====================================================================================*/
#include "PCH.h"
#include "RXWindowProcedure.h"

void BaseWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
	}
}

LRESULT CALLBACK DefaultWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	BaseWndProc(hWnd, msg, wParam, lParam);

	switch (msg)
	{
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

	// Alt + Enter�� ���� ó���Դϴ�.
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

LRESULT CALLBACK DefaultWndProcDX9(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	BaseWndProc(hWnd, msg, wParam, lParam);

	switch (msg)
	{
	case WM_SYSCOMMAND:
	{
		switch (wParam)
		{
		case SC_MAXIMIZE:
		{
			OnMaximizeDX9();
			break;
		}
		}

		break;
	}
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_F12:
		{
			OnKeyF12DX9();
			break;
		}
		}

		break;
	}
	}

	// Alt + Enter�� ���� ó���Դϴ�.
	switch (msg)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		OnAltEnterDX9(wParam, lParam);
		break;
	}
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

void OnCreate()
{
	RXDEBUGLOG("���α׷� â�� �����߽��ϴ�.");
}

void OnClose()
{
	::DestroyWindow(g_hMainWnd);
	RXDEBUGLOG("�ݱ� ��ư�� Ŭ���߽��ϴ�.");
}

void OnDestroy()
{
	::PostQuitMessage(0);
	RXDEBUGLOG("���α׷� â�� �����߽��ϴ�.");
}

void OnKeyEscape()
{
	::SendMessage(g_hMainWnd, WM_CLOSE, 0, 0); // ����ȭ �Լ�
	RXDEBUGLOG("ESC Ű�� �������ϴ�.");
}

void OnMouseLButtonDown(LPARAM lParam)
{
	//MSGBOX("���콺 ���� ��ư Ŭ��!");
	//g_pThis->ResizeResolution(1024, 768);
}

void OnMouseRButtonDown(LPARAM lParam)
{
	//MSGBOX("���콺 ������ ��ư Ŭ��!");
	//g_pThis->ResizeResolution(800, 600);
}

void OnMouseMove()
{
	return;
}

// â ũ�� ������ ���۵� �� �׻� ó���˴ϴ�.
// (â ȭ�� -> ��ü ȭ��)�� ���� ó������ �ʽ��ϴ�.
//
// ����� AdjustWindowRect()�� ȣ��Ǹ� WM_GETMINMAXINFO�� �߻��ϹǷ�
// (��ü ȭ�� -> â ȭ��)�� ���� Ŭ���̾�Ʈ ���� ������ �� �����ؾ� �մϴ�.
// ��, ���ѷ����� �ǹǷ� �����ؾ� �մϴ�.
void OnGetMinMaxInfo(LPARAM lParam)
{
	MINMAXINFO* pMinMax = reinterpret_cast<MINMAXINFO*>(lParam);
	NULLCHK_RETURN_NOCOMENT(pMinMax);

	// ��ü ������ ������ ����ǹǷ� GetClientRect()�� ����ؾ� ������ 
	// GetClientRect()�� �ƴ϶� �̸� ����ص� ��ü ������ ������ �̿��մϴ�.
	// ������ GetClientRect()�� ���� ���� �����Դϴ�.
	RECT rtWindow;
	::CopyRect(&rtWindow, &g_pMain->getWindowRect());

	// â ũ�� ���� �Ұ����մϴ�.
	pMinMax->ptMinTrackSize.x = rtWindow.right - rtWindow.left;
	pMinMax->ptMinTrackSize.y = rtWindow.bottom - rtWindow.top;
	pMinMax->ptMaxTrackSize.x = rtWindow.right - rtWindow.left;
	pMinMax->ptMaxTrackSize.y = rtWindow.bottom - rtWindow.top;
}

void OnMaximize()
{
	g_pMain->ToggleFullScreenMode(true);
	RXDEBUGLOG("�ִ�ȭ�� Ŭ���߽��ϴ�. â ȭ�� -> ��ü ȭ��");
}

void OnAltEnter(WPARAM wParam, LPARAM lParam)
{
	// ���� Alt + Enter�Դϴ�.
	// ������ Alt�� WM_KEYDOWN���� ���ɴϴ�.
	if (wParam == VK_RETURN)
	{
		if ((HIWORD(lParam) & KF_ALTDOWN)) // Alt�� �������� ��Ʈ �÷��׷� Ȯ���մϴ�.
		{
			g_pMain->ToggleFullScreenMode();
			RXDEBUGLOG("���� Alt + Enter�� �������ϴ�. ��ü ȭ�� <-> â ȭ��");
		}
	}
}

void OnKeyF12DX9()
{
	RX::RXRendererDX9::Instance()->SaveBackBufferToFile("RXTest");
}

void OnMaximizeDX9()
{
	g_pMainDX9->ToggleFullScreenMode(true);
	RXDEBUGLOG("�ִ�ȭ�� Ŭ���߽��ϴ�. â ȭ�� -> ��ü ȭ��");
}

void OnAltEnterDX9(WPARAM wParam, LPARAM lParam)
{
	// ���� Alt + Enter�Դϴ�.
	// ������ Alt�� WM_KEYDOWN���� ���ɴϴ�.
	if (wParam == VK_RETURN)
	{
		if ((HIWORD(lParam) & KF_ALTDOWN)) // Alt�� �������� ��Ʈ �÷��׷� Ȯ���մϴ�.
		{
			g_pMainDX9->ToggleFullScreenMode();
			RXDEBUGLOG("���� Alt + Enter�� �������ϴ�. ��ü ȭ�� <-> â ȭ��");
		}
	}
}
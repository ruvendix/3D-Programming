/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-08-21
 *
 * <파일 내용>
 * 윈도우 프로시저(메시지 프로시저)들이 정의되어있습니다.
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

	// Alt + Enter를 위한 처리입니다.
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
	RXDEBUGLOG("프로그램 창을 생성했습니다.");
}

void OnClose()
{
	::DestroyWindow(g_hMainWnd);
	RXDEBUGLOG("닫기 버튼을 클릭했습니다.");
}

void OnDestroy()
{
	::PostQuitMessage(0);
	RXDEBUGLOG("프로그램 창을 제거했습니다.");
}

void OnKeyEscape()
{
	::SendMessage(g_hMainWnd, WM_CLOSE, 0, 0); // 동기화 함수
	RXDEBUGLOG("ESC 키를 눌렀습니다.");
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
// (창 화면 -> 전체 화면)일 때는 처리되지 않습니다.
//
// 참고로 AdjustWindowRect()가 호출되면 WM_GETMINMAXINFO가 발생하므로
// (전체 화면 -> 창 화면)일 때는 클라이언트 영역 조절할 때 주의해야 합니다.
// 즉, 무한루프가 되므로 주의해야 합니다.
void OnGetMinMaxInfo(LPARAM lParam)
{
	MINMAXINFO* pMinMax = reinterpret_cast<MINMAXINFO*>(lParam);
	NULLCHK_RETURN_NOCOMENT(pMinMax);

	// 전체 윈도우 영역에 적용되므로 GetClientRect()를 사용해야 하지만 
	// GetClientRect()가 아니라 미리 계산해둔 전체 윈도우 영역을 이용합니다.
	// 이유는 GetClientRect()의 갱신 시점 때문입니다.
	RECT rtWindow;
	::CopyRect(&rtWindow, &g_pMain->getWindowRect());

	// 창 크기 변경 불가능합니다.
	pMinMax->ptMinTrackSize.x = rtWindow.right - rtWindow.left;
	pMinMax->ptMinTrackSize.y = rtWindow.bottom - rtWindow.top;
	pMinMax->ptMaxTrackSize.x = rtWindow.right - rtWindow.left;
	pMinMax->ptMaxTrackSize.y = rtWindow.bottom - rtWindow.top;
}

void OnMaximize()
{
	g_pMain->ToggleFullScreenMode(true);
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
			g_pMain->ToggleFullScreenMode();
			RXDEBUGLOG("왼쪽 Alt + Enter를 눌렀습니다. 전체 화면 <-> 창 화면");
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
	RXDEBUGLOG("최대화를 클릭했습니다. 창 화면 -> 전체 화면");
}

void OnAltEnterDX9(WPARAM wParam, LPARAM lParam)
{
	// 왼쪽 Alt + Enter입니다.
	// 오른쪽 Alt는 WM_KEYDOWN으로 들어옵니다.
	if (wParam == VK_RETURN)
	{
		if ((HIWORD(lParam) & KF_ALTDOWN)) // Alt를 눌렀는지 비트 플래그로 확인합니다.
		{
			g_pMainDX9->ToggleFullScreenMode();
			RXDEBUGLOG("왼쪽 Alt + Enter를 눌렀습니다. 전체 화면 <-> 창 화면");
		}
	}
}
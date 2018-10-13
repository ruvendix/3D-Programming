#include "../../common/include/common.h"


// ====================================================================================
// <���� ���� ������Դϴ�>
//
HINSTANCE g_hInst    = nullptr;
HWND      g_hMainWnd = nullptr;


// ====================================================================================
// <�Լ� ������Դϴ�>
//
// ������ �޽����� ó�����ִ� ������ ���ν����Դϴ�.
LRESULT CALLBACK WndProc(HWND hWnd, UINT32 msg, WPARAM wParam, LPARAM lParam);

// ������ Ŭ������ �ʱ�ȭ���ִ� �Լ��Դϴ�.
HRESULT InitInstance();

// ���α׷� â�� �������ִ� �Լ��Դϴ�.
HRESULT CreateProgramWindow();


// ====================================================================================
// <Win32 API�� WinMain()�� �������Դϴ�>
INT32 APIENTRY _tWinMain(HINSTANCE hInstance,
	                     HINSTANCE hPrevInstance,
	                     LPTSTR    szCmdLine,
	                     INT32     cmdShow)
{
	// ������� �ʴ� �Ű������� ǥ�����ݴϴ�.
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(szCmdLine);
	UNREFERENCED_PARAMETER(cmdShow);

	// �ν��Ͻ� �ڵ��� ���� �������� ������ݴϴ�.
	g_hInst = hInstance;

	InitInstance();
	CreateProgramWindow();

	MSG msg;
	::ZeroMemory(&msg, sizeof(msg));

	// �޽��� �����Դϴ�.
	bool bDrive = true;
	while (bDrive)
	{
		// ������ �޽����� ���� ������ ����� �ʿ䰡 �����Ƿ�
		// PeekMessage()�� ����մϴ�.
		if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDrive = false;
			}
			else
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
	}

	return msg.wParam; // ���� �����Դϴ�.
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT32 msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
	{
		OutputDebugString(L"���α׷� â�� �����߽��ϴ�.");
		return 0;
	}
	case WM_CLOSE:
	{
		::DestroyWindow(hWnd);
		OutputDebugString(L"�ݱ� ��ư�� Ŭ���߽��ϴ�.");
		return 0;
	}
	case WM_DESTROY:
	{
		::PostQuitMessage(0);
		OutputDebugString(L"���α׷� â�� �����߽��ϴ�.");
		return 0;
	}
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

HRESULT InitInstance()
{
	WNDCLASSEX wcex;
	::ZeroMemory(&wcex, sizeof(wcex));

	wcex.cbSize        = sizeof(wcex);
	wcex.style         = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc   = WndProc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = 0;
	wcex.hInstance     = g_hInst;
	wcex.hIcon         = ::LoadIcon(nullptr, IDI_APPLICATION);
	wcex.hCursor       = ::LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = static_cast<HBRUSH>(::GetStockObject(GRAY_BRUSH));
	wcex.lpszMenuName  = L"None";
	wcex.lpszClassName = SZ_WINDOW_CLASS;
	wcex.hIconSm       = wcex.hIcon;

	::RegisterClassEx(&wcex);

	return S_OK;
}

HRESULT CreateProgramWindow()
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
	g_hMainWnd = ::CreateWindow(SZ_WINDOW_CLASS, SZ_PROGRAM_TITLE,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		(screenWidth - (clientRt.right - clientRt.left)) / 2,
		(screenHeight - (clientRt.bottom - clientRt.top)) / 2,
		clientRt.right - clientRt.left,
		clientRt.bottom - clientRt.top,
		::GetDesktopWindow(), // ����ȭ���� �θ� â���� �����մϴ�.
		nullptr, g_hInst, nullptr);

	::ShowWindow(g_hMainWnd, SW_NORMAL);
	::UpdateWindow(g_hMainWnd);
	::ShowCursor(TRUE);

	return S_OK;
}
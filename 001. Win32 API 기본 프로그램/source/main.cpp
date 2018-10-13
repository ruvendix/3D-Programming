#include "../../common/include/common.h"


// ====================================================================================
// <전역 변수 선언부입니다>
//
HINSTANCE g_hInst    = nullptr;
HWND      g_hMainWnd = nullptr;


// ====================================================================================
// <함수 선언부입니다>
//
// 윈도우 메시지를 처리해주는 윈도우 프로시저입니다.
LRESULT CALLBACK WndProc(HWND hWnd, UINT32 msg, WPARAM wParam, LPARAM lParam);

// 윈도우 클래스를 초기화해주는 함수입니다.
HRESULT InitInstance();

// 프로그램 창을 생성해주는 함수입니다.
HRESULT CreateProgramWindow();


// ====================================================================================
// <Win32 API는 WinMain()이 진입점입니다>
INT32 APIENTRY _tWinMain(HINSTANCE hInstance,
	                     HINSTANCE hPrevInstance,
	                     LPTSTR    szCmdLine,
	                     INT32     cmdShow)
{
	// 사용하지 않는 매개변수를 표시해줍니다.
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(szCmdLine);
	UNREFERENCED_PARAMETER(cmdShow);

	// 인스턴스 핸들을 전역 변수에도 등록해줍니다.
	g_hInst = hInstance;

	InitInstance();
	CreateProgramWindow();

	MSG msg;
	::ZeroMemory(&msg, sizeof(msg));

	// 메시지 루프입니다.
	bool bDrive = true;
	while (bDrive)
	{
		// 윈도우 메시지를 받을 때까지 대기할 필요가 없으므로
		// PeekMessage()를 사용합니다.
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

	return msg.wParam; // 정상 종료입니다.
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT32 msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
	{
		OutputDebugString(L"프로그램 창을 생성했습니다.");
		return 0;
	}
	case WM_CLOSE:
	{
		::DestroyWindow(hWnd);
		OutputDebugString(L"닫기 버튼을 클릭했습니다.");
		return 0;
	}
	case WM_DESTROY:
	{
		::PostQuitMessage(0);
		OutputDebugString(L"프로그램 창을 제거했습니다.");
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
	g_hMainWnd = ::CreateWindow(SZ_WINDOW_CLASS, SZ_PROGRAM_TITLE,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		(screenWidth - (clientRt.right - clientRt.left)) / 2,
		(screenHeight - (clientRt.bottom - clientRt.top)) / 2,
		clientRt.right - clientRt.left,
		clientRt.bottom - clientRt.top,
		::GetDesktopWindow(), // 바탕화면을 부모 창으로 설정합니다.
		nullptr, g_hInst, nullptr);

	::ShowWindow(g_hMainWnd, SW_NORMAL);
	::UpdateWindow(g_hMainWnd);
	::ShowCursor(TRUE);

	return S_OK;
}
//////////////////////////////////////////////////////////////////////
// <DirectX9의 LIB 연결입니다>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "DxErr.lib")
//
// Window Kit의 버전이 높아서 링크되지 않는 함수들을
// 링크시켜주기 위해 사용합니다. 예를 들면 "DxErr.h"가 있습니다.
#pragma comment(lib, "legacy_stdio_definitions.lib")

#include "base_project.h"

//////////////////////////////////////////////////////////////////////
// <전역 변수 선언부입니다>
//
// DirectX9 객체입니다.
// 가상 디바이스 생성, 그래픽 카드 호환성 확인 등의 작업을 처리해줍니다.
IDirect3D9* g_pD3D9 = nullptr;
//
// DirectX9 가상 디바이스입니다.
// 시스템, 렌더링, 리소스 관리, 셰이더 등 다양한 작업을 처리해줍니다.
IDirect3DDevice9* g_pD3DDevice9 = nullptr;

//////////////////////////////////////////////////////////////////////
// <함수 선언부입니다>
//
// 윈도우 메시지를 처리해주는 윈도우 프로시저입니다.
LRESULT CALLBACK WndProc(HWND hWnd, UINT32 msg, WPARAM wParam, LPARAM lParam);
//
// 윈도우 클래스를 초기화해주는 함수입니다.
HRESULT InitInstance(); 
//
// 프로그램 창을 생성해주는 함수입니다.
HRESULT CreateProgramWindow();
//
// DirectX9을 초기화해주는 함수입니다.
HRESULT InitDirectX9();

//////////////////////////////////////////////////////////////////////
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

	// 프로그램 초기화입니다.
	if (FAILED(InitInstance()))
	{
		RXERRLOG(false, "프로그램 비정상 종료!");
		::exit(1);
	}
	
	// 프로그램 창을 생성합니다.
	if (FAILED(CreateProgramWindow()))
	{
		RXERRLOG(false, "프로그램 비정상 종료!");
		::exit(1);
	}

	// DirectX9을 초기화합니다.
	if (FAILED(InitDirectX9()))
	{
		RXERRLOG(false, "프로그램 비정상 종료!");
		::exit(1);
	}

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
		else // 여기에 DirectX9 처리가 들어갑니다.
		{
			// 후면 버퍼를 설정한 색상으로 초기화해줍니다.
			g_pD3DDevice9->Clear(
				0,       // 두 번째 인자와 관련이 있는 값인데 nullptr일 때는 0으로 설정해줍니다.
				nullptr, // 초기화해줄 영역인데 nullptr로 하면 후면 버퍼 전체를 초기화해줍니다.
				D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, // 초기화 플래그인데 일반적으로는 이렇게 사용합니다.
				D3DCOLOR_ARGB(255, 0, 0, 255),      // 초기화할 때 채울 색상을 설정해줍니다.
				1.0f,                               // 깊이 버퍼를 초기화해줄 값을 설정해줍니다.
				0);                                 // 스텐실 버퍼를 초기화해줄 값을 설정해줍니다.

			g_pD3DDevice9->BeginScene(); // 렌더링 시작 루틴입니다.
			
			// 여기에 렌더링 작업이 들어갑니다.
			
			g_pD3DDevice9->EndScene(); // 렌더링 종료 루틴입니다.

			// 페이지 플리핑을 해주는 함수입니다.
			// D3DSWAPEFFECT_COPY로 설정한 게 아니라면 세 번째를 제외한 인자에는 nullptr을 넣어줍니다.
			// 세 번째 인자는 타겟 프로그램 창인데 nullptr로 설정하면 D3DPP에서 설정했던
			// 타겟 프로그램 창으로 설정됩니다.
			g_pD3DDevice9->Present(nullptr, nullptr, nullptr, nullptr);
		}
	}

	// 인터페이스를 해제해줍니다.
	// 초기화했던 순서와 반대로 해제해줘야 합니다.
	SAFE_RELEASE(g_pD3DDevice9);
	SAFE_RELEASE(g_pD3D9);
	RXLOG(false, "DirectX9 안전하게 전부 해제!");

	RXLOG(false, "프로그램 정상 종료!");
	return msg.wParam; // 정상 종료입니다.
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT32 msg, WPARAM wParam, LPARAM lParam)
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
	wcex.hIcon         = ::LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_RUVENDIX_ICO));
	wcex.hCursor       = ::LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = static_cast<HBRUSH>(::GetStockObject(GRAY_BRUSH));
	wcex.lpszMenuName  = SZ_NONE;
	wcex.lpszClassName = SZ_WINDOW_CLASS;
	wcex.hIconSm       = wcex.hIcon;

	if (::RegisterClassEx(&wcex) == 0)
	{
		ERRMSG_EFAIL_RETURNBOX("프로그램 초기화 실패!");
	}
	
	RXLOG(false, "프로그램 초기화 성공!");
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

	NULLCHK_EFAIL_RETURN(g_hMainWnd, "프로그램 창 생성 실패!");
	::ShowWindow(g_hMainWnd, SW_NORMAL);
	::UpdateWindow(g_hMainWnd);
	::ShowCursor(TRUE);

	RXLOG(false, "프로그램 창 생성 성공!");
	return S_OK;
}

HRESULT InitDirectX9()
{
	// DirectX9 객체를 생성합니다.
	// 인터페이스 포인터에 인스턴화된 객체를 반환해줍니다.
	g_pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	NULLCHK_EFAIL_RETURN(g_pD3D9, "DirectX9 객체 생성 실패!");
	RXLOG(false, "DirectX9 객체 생성 성공!");

	//////////////////////////////////////////////////////////////////////
	// 가상 디바이스 생성을 위한 정보를 설정해줍니다.
	// 일반적으로 Present Parameters를 줄여서 PP라고 선언합니다.
	D3DPRESENT_PARAMETERS D3DPP;
	::ZeroMemory(&D3DPP, sizeof(D3DPP));

	// 후면 버퍼 정보를 설정합니다.
	// 순서대로 가로, 세로, 형식, 개수입니다.
	// 형식에 A는 Alpha를 의미합니다.
	// 개수에는 0을 넣어도 1로 인식되지만 가독성을 위해 1로 설정합니다.
	D3DPP.BackBufferWidth  = DEFAULT_CLIENT_WIDTH;
	D3DPP.BackBufferHeight = DEFAULT_CLIENT_HEIGHT;
	D3DPP.BackBufferFormat = D3DFMT_A8R8G8B8;
	D3DPP.BackBufferCount  = 1;

	// 깊이 스텐실 버퍼 정보를 설정합니다.
	// D3DFMT_D24S8은 깊이 버퍼 24비트, 스텐실 버퍼 8비트를 뜻합니다.
	D3DPP.EnableAutoDepthStencil = true;
	D3DPP.AutoDepthStencilFormat = D3DFMT_D24S8;

	// 페이지 플리핑을 할 때의 효과를 설정합니다.
	// 초반에는 D3DSWAPEFFECT_DISCARD가 가장 무난합니다.
	D3DPP.SwapEffect = D3DSWAPEFFECT_DISCARD;

	// 특수한 플래그를 설정합니다.
	// 딱히 사용할 플래그가 없으니 0으로 설정합니다.
	D3DPP.Flags = 0;

	// 프로그램 창 정보를 설정합니다.
	// 프로그램 창 핸들과 창 모드 여부를 설정해주면 됩니다.
	// 나중에 전체 화면도 설정하게 되지만 초반에는 창 모드만 사용합니다.
	D3DPP.hDeviceWindow = g_hMainWnd;
	D3DPP.Windowed      = true;
	
	// 멀티 샘플링 정보를 설정합니다.
	// MSAA(MultiSample AntiAliasing)라고도 합니다.
	// 초반에는 사용할 일이 없으므로 0으로 설정합니다.
	D3DPP.MultiSampleType    = D3DMULTISAMPLE_NONE;
	D3DPP.MultiSampleQuality = 0;

	// 모니터 정보를 설정합니다.
	// 전체 화면일 때는 재생률을 설정해줘야 합니다.
	// 창 모드를 사용할 때는 D3DPRESENT_RATE_DEFAULT로 설정하면 됩니다.
	// 페이지 플리핑 간격도 설정해줘야 하는데 D3DPRESENT_INTERVAL_DEFAULT를 사용합니다.
	D3DPP.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	D3DPP.PresentationInterval       = D3DPRESENT_INTERVAL_DEFAULT;
	//////////////////////////////////////////////////////////////////////

	// 가상 디바이스 생성을 위한 정보를 설정했으므로 가상 디바이스를 생성해줍니다.
	g_hResult = g_pD3D9->CreateDevice(
		D3DADAPTER_DEFAULT,                  // 어댑터를 뜻하는데 모니터 개수라고 생각하면 됩니다.
		D3DDEVTYPE_HAL,                      // HAL Device를 사용하겠다는 것입니다.
		D3DPP.hDeviceWindow,                 // 가상 디바이스의 타겟 프로그램 창을 의미합니다.
		D3DCREATE_HARDWARE_VERTEXPROCESSING, // 정점 처리를 그래픽 카드에게 맡긴다는 뜻입니다.
		&D3DPP,                              // 가상 디바이스 생성을 위한 정보를 넘겨줍니다.
		&g_pD3DDevice9);                     // 가상 디바이스의 객체 포인터를 받을 인터페이스 포인터입니다.
	
	DXERR_HANDLER();
	NULLCHK_EFAIL_RETURN(g_pD3DDevice9, "DirectX9 가상 디바이스 생성 실패!");
	
	RXLOG(false, "DirectX9 가상 디바이스 생성 성공!");
	return S_OK;
}
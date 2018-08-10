//////////////////////////////////////////////////////////////////////
// <DirectX9�� LIB �����Դϴ�>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "DxErr.lib")
//
// Window Kit�� ������ ���Ƽ� ��ũ���� �ʴ� �Լ�����
// ��ũ�����ֱ� ���� ����մϴ�. ���� ��� "DxErr.h"�� �ֽ��ϴ�.
#pragma comment(lib, "legacy_stdio_definitions.lib")

#include "base_project.h"

//////////////////////////////////////////////////////////////////////
// <���� ���� ������Դϴ�>
//
// DirectX9 ��ü�Դϴ�.
// ���� ����̽� ����, �׷��� ī�� ȣȯ�� Ȯ�� ���� �۾��� ó�����ݴϴ�.
IDirect3D9* g_pD3D9 = nullptr;
//
// DirectX9 ���� ����̽��Դϴ�.
// �ý���, ������, ���ҽ� ����, ���̴� �� �پ��� �۾��� ó�����ݴϴ�.
IDirect3DDevice9* g_pD3DDevice9 = nullptr;

//////////////////////////////////////////////////////////////////////
// <�Լ� ������Դϴ�>
//
// ������ �޽����� ó�����ִ� ������ ���ν����Դϴ�.
LRESULT CALLBACK WndProc(HWND hWnd, UINT32 msg, WPARAM wParam, LPARAM lParam);
//
// ������ Ŭ������ �ʱ�ȭ���ִ� �Լ��Դϴ�.
HRESULT InitInstance(); 
//
// ���α׷� â�� �������ִ� �Լ��Դϴ�.
HRESULT CreateProgramWindow();
//
// DirectX9�� �ʱ�ȭ���ִ� �Լ��Դϴ�.
HRESULT InitDirectX9();

//////////////////////////////////////////////////////////////////////
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

	// ���α׷� �ʱ�ȭ�Դϴ�.
	if (FAILED(InitInstance()))
	{
		RXERRLOG(false, "���α׷� ������ ����!");
		::exit(1);
	}
	
	// ���α׷� â�� �����մϴ�.
	if (FAILED(CreateProgramWindow()))
	{
		RXERRLOG(false, "���α׷� ������ ����!");
		::exit(1);
	}

	// DirectX9�� �ʱ�ȭ�մϴ�.
	if (FAILED(InitDirectX9()))
	{
		RXERRLOG(false, "���α׷� ������ ����!");
		::exit(1);
	}

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
		else // ���⿡ DirectX9 ó���� ���ϴ�.
		{
			// �ĸ� ���۸� ������ �������� �ʱ�ȭ���ݴϴ�.
			g_pD3DDevice9->Clear(
				0,       // �� ��° ���ڿ� ������ �ִ� ���ε� nullptr�� ���� 0���� �������ݴϴ�.
				nullptr, // �ʱ�ȭ���� �����ε� nullptr�� �ϸ� �ĸ� ���� ��ü�� �ʱ�ȭ���ݴϴ�.
				D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, // �ʱ�ȭ �÷����ε� �Ϲ������δ� �̷��� ����մϴ�.
				D3DCOLOR_ARGB(255, 0, 0, 255),      // �ʱ�ȭ�� �� ä�� ������ �������ݴϴ�.
				1.0f,                               // ���� ���۸� �ʱ�ȭ���� ���� �������ݴϴ�.
				0);                                 // ���ٽ� ���۸� �ʱ�ȭ���� ���� �������ݴϴ�.

			g_pD3DDevice9->BeginScene(); // ������ ���� ��ƾ�Դϴ�.
			
			// ���⿡ ������ �۾��� ���ϴ�.
			
			g_pD3DDevice9->EndScene(); // ������ ���� ��ƾ�Դϴ�.

			// ������ �ø����� ���ִ� �Լ��Դϴ�.
			// D3DSWAPEFFECT_COPY�� ������ �� �ƴ϶�� �� ��°�� ������ ���ڿ��� nullptr�� �־��ݴϴ�.
			// �� ��° ���ڴ� Ÿ�� ���α׷� â�ε� nullptr�� �����ϸ� D3DPP���� �����ߴ�
			// Ÿ�� ���α׷� â���� �����˴ϴ�.
			g_pD3DDevice9->Present(nullptr, nullptr, nullptr, nullptr);
		}
	}

	// �������̽��� �������ݴϴ�.
	// �ʱ�ȭ�ߴ� ������ �ݴ�� ��������� �մϴ�.
	SAFE_RELEASE(g_pD3DDevice9);
	SAFE_RELEASE(g_pD3D9);
	RXLOG(false, "DirectX9 �����ϰ� ���� ����!");

	RXLOG(false, "���α׷� ���� ����!");
	return msg.wParam; // ���� �����Դϴ�.
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT32 msg, WPARAM wParam, LPARAM lParam)
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
		ERRMSG_EFAIL_RETURNBOX("���α׷� �ʱ�ȭ ����!");
	}
	
	RXLOG(false, "���α׷� �ʱ�ȭ ����!");
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

	NULLCHK_EFAIL_RETURN(g_hMainWnd, "���α׷� â ���� ����!");
	::ShowWindow(g_hMainWnd, SW_NORMAL);
	::UpdateWindow(g_hMainWnd);
	::ShowCursor(TRUE);

	RXLOG(false, "���α׷� â ���� ����!");
	return S_OK;
}

HRESULT InitDirectX9()
{
	// DirectX9 ��ü�� �����մϴ�.
	// �������̽� �����Ϳ� �ν���ȭ�� ��ü�� ��ȯ���ݴϴ�.
	g_pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	NULLCHK_EFAIL_RETURN(g_pD3D9, "DirectX9 ��ü ���� ����!");
	RXLOG(false, "DirectX9 ��ü ���� ����!");

	//////////////////////////////////////////////////////////////////////
	// ���� ����̽� ������ ���� ������ �������ݴϴ�.
	// �Ϲ������� Present Parameters�� �ٿ��� PP��� �����մϴ�.
	D3DPRESENT_PARAMETERS D3DPP;
	::ZeroMemory(&D3DPP, sizeof(D3DPP));

	// �ĸ� ���� ������ �����մϴ�.
	// ������� ����, ����, ����, �����Դϴ�.
	// ���Ŀ� A�� Alpha�� �ǹ��մϴ�.
	// �������� 0�� �־ 1�� �νĵ����� �������� ���� 1�� �����մϴ�.
	D3DPP.BackBufferWidth  = DEFAULT_CLIENT_WIDTH;
	D3DPP.BackBufferHeight = DEFAULT_CLIENT_HEIGHT;
	D3DPP.BackBufferFormat = D3DFMT_A8R8G8B8;
	D3DPP.BackBufferCount  = 1;

	// ���� ���ٽ� ���� ������ �����մϴ�.
	// D3DFMT_D24S8�� ���� ���� 24��Ʈ, ���ٽ� ���� 8��Ʈ�� ���մϴ�.
	D3DPP.EnableAutoDepthStencil = true;
	D3DPP.AutoDepthStencilFormat = D3DFMT_D24S8;

	// ������ �ø����� �� ���� ȿ���� �����մϴ�.
	// �ʹݿ��� D3DSWAPEFFECT_DISCARD�� ���� �����մϴ�.
	D3DPP.SwapEffect = D3DSWAPEFFECT_DISCARD;

	// Ư���� �÷��׸� �����մϴ�.
	// ���� ����� �÷��װ� ������ 0���� �����մϴ�.
	D3DPP.Flags = 0;

	// ���α׷� â ������ �����մϴ�.
	// ���α׷� â �ڵ�� â ��� ���θ� �������ָ� �˴ϴ�.
	// ���߿� ��ü ȭ�鵵 �����ϰ� ������ �ʹݿ��� â ��常 ����մϴ�.
	D3DPP.hDeviceWindow = g_hMainWnd;
	D3DPP.Windowed      = true;
	
	// ��Ƽ ���ø� ������ �����մϴ�.
	// MSAA(MultiSample AntiAliasing)��� �մϴ�.
	// �ʹݿ��� ����� ���� �����Ƿ� 0���� �����մϴ�.
	D3DPP.MultiSampleType    = D3DMULTISAMPLE_NONE;
	D3DPP.MultiSampleQuality = 0;

	// ����� ������ �����մϴ�.
	// ��ü ȭ���� ���� ������� ��������� �մϴ�.
	// â ��带 ����� ���� D3DPRESENT_RATE_DEFAULT�� �����ϸ� �˴ϴ�.
	// ������ �ø��� ���ݵ� ��������� �ϴµ� D3DPRESENT_INTERVAL_DEFAULT�� ����մϴ�.
	D3DPP.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	D3DPP.PresentationInterval       = D3DPRESENT_INTERVAL_DEFAULT;
	//////////////////////////////////////////////////////////////////////

	// ���� ����̽� ������ ���� ������ ���������Ƿ� ���� ����̽��� �������ݴϴ�.
	g_hResult = g_pD3D9->CreateDevice(
		D3DADAPTER_DEFAULT,                  // ����͸� ���ϴµ� ����� ������� �����ϸ� �˴ϴ�.
		D3DDEVTYPE_HAL,                      // HAL Device�� ����ϰڴٴ� ���Դϴ�.
		D3DPP.hDeviceWindow,                 // ���� ����̽��� Ÿ�� ���α׷� â�� �ǹ��մϴ�.
		D3DCREATE_HARDWARE_VERTEXPROCESSING, // ���� ó���� �׷��� ī�忡�� �ñ�ٴ� ���Դϴ�.
		&D3DPP,                              // ���� ����̽� ������ ���� ������ �Ѱ��ݴϴ�.
		&g_pD3DDevice9);                     // ���� ����̽��� ��ü �����͸� ���� �������̽� �������Դϴ�.
	
	DXERR_HANDLER();
	NULLCHK_EFAIL_RETURN(g_pD3DDevice9, "DirectX9 ���� ����̽� ���� ����!");
	
	RXLOG(false, "DirectX9 ���� ����̽� ���� ����!");
	return S_OK;
}
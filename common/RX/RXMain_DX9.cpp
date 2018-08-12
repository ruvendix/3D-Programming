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
 * DirectX9 전용 코어 서브 루틴입니다.
 *
 ====================================================================================*/
#include "stdafx.h"
#include "RXMain_DX9.h"

namespace
{
	RX::RXMain_DX9* g_pThis = nullptr;
}

LRESULT CALLBACK DefaultWndProcDX9(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
		case VK_UP:
		{
			g_pThis->ChangeScreenMode(false);
			break;
		}
		}

		return 0;
	}
	case WM_SYSCOMMAND:
	{
		switch (wParam)
		{
		case SC_MAXIMIZE:
		{
			g_pThis->ChangeScreenMode(true);
			break;
		}
		}
	}
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

namespace RX
{

	RXMain_DX9::RXMain_DX9()
	{
		g_pThis = this;

		m_bLostDevice = false;
		m_bMSAA       = false;
		m_pD3D9       = nullptr;
		m_pD3DDevice9 = nullptr;
		m_dwBehavior  = 0;
	}

	RXMain_DX9::~RXMain_DX9()
	{

	}

	HRESULT RXMain_DX9::InitMain()
	{
		setWndProc(DefaultWndProcDX9);
		RXMain::InitMain();

		if (FAILED(InitD3D9()))
		{			
			RXERRLOG_EFAIL_RETURN("D3D9 초기화 실패!");
		}

		return S_OK;
	}

	HRESULT RXMain_DX9::InitD3D9()
	{
		// DirectX9 객체를 생성합니다.
		// 인터페이스 포인터에 인스턴화된 객체를 반환해줍니다.
		m_pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
		NULLCHK_EFAIL_RETURN(m_pD3D9, "DirectX9 객체 생성 실패!");
		RXLOG(false, "DirectX9 객체 생성 성공!");
		
		// ====================================================================================
		// 가상 디바이스 생성을 위한 정보를 설정해줍니다.
		// 일반적으로 Present Parameters를 줄여서 PP라고 선언합니다.
		D3DPRESENT_PARAMETERS D3DPP;
		::ZeroMemory(&D3DPP, sizeof(D3DPP));

		// ====================================================================================
		// 가상 디바이스 생성이 가능한지 검증합니다.
		VerifyDevice(&D3DPP);

		// ====================================================================================
		// 검증이 끝나면 나머지 정보를 채워줍니다.
		//
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
		D3DPP.hDeviceWindow = m_hMainWnd;
		D3DPP.Windowed      = !m_bFullScreen;
		
		// ====================================================================================

		HRESULT hDXResult = S_OK;

		// 가상 디바이스 생성을 위한 정보를 설정했으므로 가상 디바이스를 생성해줍니다.
		hDXResult = m_pD3D9->CreateDevice(
			D3DADAPTER_DEFAULT,     // 어댑터를 뜻하는데 모니터 개수라고 생각하면 됩니다.
			D3DDEVTYPE_HAL,         // HAL Device를 사용하겠다는 것입니다.
			D3DPP.hDeviceWindow,    // 가상 디바이스의 타겟 프로그램 창을 의미합니다.
			m_dwBehavior,           // 정점 처리를 그래픽 카드에게 맡긴다는 뜻입니다.
			&D3DPP,                 // 가상 디바이스 생성을 위한 정보를 넘겨줍니다.
			&m_pD3DDevice9);        // 가상 디바이스의 객체 포인터를 받을 인터페이스 포인터입니다.
	
		DXERR_HANDLER(hDXResult);
		NULLCHK_EFAIL_RETURN(m_pD3DDevice9, "DirectX9 가상 디바이스 생성 실패!");

		RXLOG(false, "DirectX9 가상 디바이스 생성 성공!");
		return S_OK;
	}

	HRESULT RXMain_DX9::DriveMain()
	{
		MSG msg;
		::ZeroMemory(&msg, sizeof(msg));

		bool m_bDriveFailure = false;

		DWORD nextGameTime    = ::timeGetTime();
		INT32 updateCallCount = 0;
		FLOAT rRenderingInterpolation = 0.0f;

		for ( ; ; )
		{
			// 외부에서도 별개로 종료 컨트롤이 가능합니다.
			if (m_routineState == ROUTINE_STATE::EXIT)
			{
				m_msgCode = 0;
				break;
			}

			if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
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
				updateCallCount = 0;

				// 업데이트 루틴은 FPS에 영향을 받습니다.
				// 25 정도로 해주면 좋습니다.
				// 이쪽은 하드웨어에 따라 성능 차이가 거의 없습니다.
				while ( (::timeGetTime() > nextGameTime) &&
					    (updateCallCount < MAX_FRAMESKIP) )
				{
					if (m_bLostDevice == false)
					{
						Update();
					}

					nextGameTime += FRAME_SKIP_TIME;
					++updateCallCount;

					//RXDEBUGLOG("업데이트 호출 수 : %d", updateCallCount);
				}

				// 이쪽은 하드웨어에 따라 성능 차이가 심합니다.
				// 렌더링을 수행하는 부분입니다.
				// 인터폴레이션은 0.0 ~ 1.0 사이로 옵니다.
				rRenderingInterpolation =
					float(::timeGetTime() + FRAME_SKIP_TIME - nextGameTime) / float(FRAME_SKIP_TIME);
				//RXDEBUGLOG("렌더링 보간값 : %.2f", rRenderingInterpolation);				
				Render(rRenderingInterpolation);

				// 렌더링을 완료하면 프레임 카운트가 1증가 됩니다.
				// 렌더링은 많은 연산이 필요하므로 프레임 기준이 됩니다.
				m_frame.UpdateFrame();

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

	HRESULT RXMain_DX9::Release()
	{
		m_arrSubFunc[static_cast<INT32>(SUBFUNC_TYPE::RELEASE)].func();

		SAFE_RELEASE(m_pD3DDevice9);
		SAFE_RELEASE(m_pD3D9);

		RXMain::Release();
		return S_OK;
	}

	// 가상 디바이스를 생성하기 전에 검증해주는 함수입니다.
	// 검증하는 순서는 다음과 같습니다.
	//
	// 1. 그래픽 카드의 출력 능력 검증(모니터 호환)
	// 2. 하드웨어 가속이 가능한지 검증
	// 3. MSAA 검증
	// 4. 정점 처리 방식 검증(셰이더)
	HRESULT RXMain_DX9::VerifyDevice(D3DPRESENT_PARAMETERS* pD3DPP)
	{
		// ====================================================================================
		// 그래픽 카드의 출력 능력을 검증합니다.
		D3DDISPLAYMODE mainDisplayMode;
		::ZeroMemory(&mainDisplayMode, sizeof(mainDisplayMode));

		// 그래픽 카드의 출력 정보를 가져옵니다.
		if (FAILED(m_pD3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mainDisplayMode)))
		{
			RXERRLOG_EFAIL_RETURN("그래픽 카드의 출력 정보 획득 실패!");
		}

		// 후면 버퍼의 개수를 설정합니다.
		// 개수에는 0을 넣어도 1로 인식되지만 가독성을 위해 1로 설정합니다.
		pD3DPP->BackBufferCount = 1;

		// 후면 버퍼 정보를 설정합니다.
		// 순서대로 가로, 세로입니다.
		// 전체 화면일 때는 디스플레이 모드 정보로 설정해줍니다.
		if (m_bFullScreen == false)
		{
			pD3DPP->BackBufferWidth  = DEFAULT_CLIENT_WIDTH;
			pD3DPP->BackBufferHeight = DEFAULT_CLIENT_HEIGHT;
		}

		// 후면 버퍼의 형식을 설정해줍니다.
		pD3DPP->BackBufferFormat = mainDisplayMode.Format;

		// 후면 버퍼의 주사율을 설정합니다.
		// 창 모드를 사용할 때는 D3DPRESENT_RATE_DEFAULT로 설정하면 됩니다.
		if (m_bFullScreen == false)
		{
			pD3DPP->FullScreen_RefreshRateInHz = D3DPRESENT_INTERVAL_DEFAULT;
		}

		// 후면 버퍼의 페이지 플리핑 간격을 설정합니다.
		pD3DPP->PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

		// 전체 화면으로 전환될 때의 처리입니다.
		if (m_bFullScreen)
		{
			bool bResult = false;

			// 전체 화면으로 전환될 때 사용 가능한 모드 개수를 가져옵니다.
			// 모드란 그래픽 카드가 지원하는 해상도를 말합니다.
			// 예를 들면 현재 모니터 해상도는 1920 X 1080이지만
			// 그래픽 카드는 800 X 600도 지원합니다. 이런 게 모드입니다.
			INT32 adapterCount = m_pD3D9->GetAdapterModeCount(D3DADAPTER_DEFAULT, mainDisplayMode.Format);
			for (INT32 i = 0; i < adapterCount; ++i)
			{
				D3DDISPLAYMODE subDisplayMode;
				::ZeroMemory(&subDisplayMode, sizeof(subDisplayMode));

				// 원하는 해상도 또는 형식과 호환이 되는지 검증합니다.
				HRESULT hDXResult = m_pD3D9->EnumAdapterModes(D3DADAPTER_DEFAULT,
					mainDisplayMode.Format, i, &subDisplayMode);
				DXERR_HANDLER(hDXResult);
	
				// 현재 해상도와 전체 화면 해상도를 비교합니다.
				if ( (subDisplayMode.Width == mainDisplayMode.Width) &&
					 (subDisplayMode.Height == mainDisplayMode.Height) &&
					 (subDisplayMode.RefreshRate >= mainDisplayMode.RefreshRate) )
				{
					pD3DPP->BackBufferWidth            = subDisplayMode.Width;
					pD3DPP->BackBufferHeight           = subDisplayMode.Height;
					pD3DPP->FullScreen_RefreshRateInHz = subDisplayMode.RefreshRate;
					bResult = true;
					break;
				}
			}

			if (bResult == false)
			{
				RXERRLOG_EFAIL_RETURN("전체 화면일 때 현재 그래픽 카드가 해상도 또는 형식을 지원하지 않음!");
			}
		}

		// ====================================================================================
		// 하드웨어 가속이 가능한지 검증합니다.
		if (FAILED(m_pD3D9->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
			pD3DPP->BackBufferFormat, pD3DPP->BackBufferFormat, !m_bFullScreen)))
		{
			RXERRLOG_EFAIL_RETURN("하드웨어 가속을 지원하지 않음!");
		}

		// ====================================================================================
		// MSAA(Multisample AntiAliasing)을 검증합니다.
		// AA는 여러 기법이 있지만 가장 기본적인 건 MSAA입니다.
		// 2, 4, 8, 16만 검증합니다.
		if (m_bMSAA)
		{
			DWORD dwMSAAQuality = 0;
			INT32 MSAATable[4] =
			{
				D3DMULTISAMPLE_16_SAMPLES,
				D3DMULTISAMPLE_8_SAMPLES,
				D3DMULTISAMPLE_4_SAMPLES,
				D3DMULTISAMPLE_2_SAMPLES,
			};

			bool bResult = false;
			for (INT32 i = 0; i < 4; ++i)
			{
				D3DMULTISAMPLE_TYPE type = static_cast<D3DMULTISAMPLE_TYPE>(MSAATable[i]);

				if (SUCCEEDED(m_pD3D9->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
					D3DDEVTYPE_HAL, mainDisplayMode.Format, !m_bFullScreen, type, &dwMSAAQuality)))
				{
					pD3DPP->MultiSampleType = type;
					pD3DPP->MultiSampleQuality = dwMSAAQuality - 1;
					bResult = true;
					break;
				}
			}

			if (bResult == false)
			{
				pD3DPP->MultiSampleType    = D3DMULTISAMPLE_NONE;
				pD3DPP->MultiSampleQuality = 0;
				RXERRLOG_EFAIL_RETURN("MSAA를 지원하지 않으므로 0으로 설정함!");
			}
		}
		else
		{
			// 멀티 샘플링 정보를 설정합니다.
			// MSAA(MultiSample AntiAliasing)라고도 합니다.
			// 초반에는 사용할 일이 없으므로 0으로 설정합니다.
			pD3DPP->MultiSampleType    = D3DMULTISAMPLE_NONE;
			pD3DPP->MultiSampleQuality = 0;
		}

		// ====================================================================================
		// 정점 처리 방식을 검증합니다.
		D3DCAPS9 D3DCaps9;
		::ZeroMemory(&D3DCaps9, sizeof(D3DCaps9));

		// 그래픽 카드의 능력 정보를 가져옵니다.
		m_pD3D9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &D3DCaps9);

		// 그래픽 카드가 TL, 정점 셰이더 1.0, 픽셀 셰이더 1.0을 지원하지 않으면
		// 정점 처리 방식을 소프트웨어 정점 처리로 설정합니다.
		if ( (D3DCaps9.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) ||
			 (D3DCaps9.VertexShaderVersion < D3DVS_VERSION(1, 0)) ||
			 (D3DCaps9.PixelShaderVersion < D3DPS_VERSION(1, 0)))
		{
			m_dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		}
		else
		{
			m_dwBehavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		}

		return S_OK;
	}

	void RXMain_DX9::ChangeScreenMode(bool bFullScreen)
	{
		m_bFullScreen = bFullScreen;

		OnLostDevice();
		OnResetDevice();

		// 뭔가 좀 이상하지만...
		// 리셋 디바이스 먼저 해야 하므로 이렇게 사용합니다.
		RXMain::ChangeScreenMode(m_bFullScreen);
	}

	HRESULT RXMain_DX9::Update()
	{
		RXMain::Update();
		return S_OK;
	}

	HRESULT RXMain_DX9::Render(FLOAT rInterpolation)
	{
		// 가상 디바이스가 존재할 때만 렌더링합니다.
		if (m_bLostDevice == false)
		{
			BeginRender();

			if (FAILED(m_arrSubFunc[static_cast<INT32>(SUBFUNC_TYPE::RENDER)].func()))
			{
				RXERRLOG_EFAIL_RETURN("서브 렌더 실패!");
			}

			EndRender();
		}

		NULLCHK(m_pD3DDevice9);
		HRESULT hDXReuslt = m_pD3DDevice9->Present(nullptr, nullptr, nullptr, nullptr);

		// 로스트 디바이스는 자주 발생할 수 있으므로 따로 에러 핸들러를 하지 않습니다.
		if (hDXReuslt == D3DERR_DEVICELOST)
		{
			RXERRLOG("가상 디바이스 손실 발생!");
			m_bLostDevice = true;
			Sleep(100); // 가상 디바이스가 손실되었다면 잠깐 CPU를 풀어줍니다.
		}

		// 가상 디바이스가 손실되었다면 처리해줍니다.
		// 위에서 잠깐의 시간을 주기 위해 m_bLostDevice를 이용합니다.
		if (m_bLostDevice)
		{
			// 가상 디바이스를 계속 사용할 수 있는지 확인합니다.
			if (FAILED(hDXReuslt = m_pD3DDevice9->TestCooperativeLevel()))
			{
				// 가상 디바이스가 완전히 손실되어서 리셋할 수 없는 상황입니다.
				// 이럴 경우에는 프로그램을 재실행해야 합니다.
				if (hDXReuslt == D3DERR_DEVICELOST)
				{
					RXERRLOG_EFAIL_RETURN("가상 디바이스가 완전히 손실되었습니다!");
				}
				// 가상 디바이스를 리셋할 수 있는 상황입니다.
				// 가상 디바이스를 리셋하려면 가상 디바이스와 관련된
				// 모든 객체들의 상태를 로스트 상태로 만들어야 합니다.
				else if (hDXReuslt == D3DERR_DEVICENOTRESET)
				{
					OnLostDevice();
					OnResetDevice();
				}
			}
			else
			{
				// 가상 디바이스를 사용할 수 있다면
				// 가상 디바이스 손실 상태를 초기화해줍니다.
				m_bLostDevice = false;
			}
		}

		return S_OK;
	}

	HRESULT RXMain_DX9::BeginRender()
	{
		m_pD3DDevice9->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, DXCOLOR_BLUE, 1.0f, 0);
		m_pD3DDevice9->BeginScene();
		return S_OK;
	}

	HRESULT RXMain_DX9::EndRender()
	{
		m_pD3DDevice9->EndScene();
		return S_OK;
	}

	HRESULT RXMain_DX9::OnLostDevice()
	{
		if (FAILED(m_arrSubFunc[static_cast<INT32>(SUBFUNC_TYPE::LOSTDEVICE)].func()))
		{
			RXERRLOG_EFAIL_RETURN("서브 로스트 디바이스 실패!");
		}

		RXLOG(false, "로스트 디바이스!");
		return S_OK;
	}

	HRESULT RXMain_DX9::OnResetDevice()
	{
		D3DPRESENT_PARAMETERS D3DPP;
		::ZeroMemory(&D3DPP, sizeof(D3DPP));

		VerifyDevice(&D3DPP);

		D3DPP.EnableAutoDepthStencil = true;
		D3DPP.AutoDepthStencilFormat = D3DFMT_D24S8;
		D3DPP.SwapEffect             = D3DSWAPEFFECT_DISCARD;
		D3DPP.Flags                  = 0;
		D3DPP.hDeviceWindow          = m_hMainWnd;
		D3DPP.Windowed               = !m_bFullScreen;

		m_pD3DDevice9->Reset(&D3DPP);

		if (FAILED(m_arrSubFunc[static_cast<INT32>(SUBFUNC_TYPE::RESETDEVICE)].func()))
		{
			RXERRLOG_EFAIL_RETURN("서브 리셋 디바이스 실패!");
		}

		Sleep(100);

		RXLOG(false, "리셋 디바이스!");
		return S_OK;
	}

} // namespace RX end
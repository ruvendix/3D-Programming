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

namespace RX
{

	RXMain_DX9::RXMain_DX9()
	{
		m_bLostDevice = false;
		m_pD3D9       = nullptr;
		m_pD3DDevice9 = nullptr;
	}

	RXMain_DX9::~RXMain_DX9()
	{

	}

	HRESULT RXMain_DX9::InitMain()
	{
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
		D3DPP.hDeviceWindow = m_hMainWnd;
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
		// ====================================================================================

		HRESULT hDXResult = S_OK;

		// 가상 디바이스 생성을 위한 정보를 설정했으므로 가상 디바이스를 생성해줍니다.
		hDXResult = m_pD3D9->CreateDevice(
			D3DADAPTER_DEFAULT,                  // 어댑터를 뜻하는데 모니터 개수라고 생각하면 됩니다.
			D3DDEVTYPE_HAL,                      // HAL Device를 사용하겠다는 것입니다.
			D3DPP.hDeviceWindow,                 // 가상 디바이스의 타겟 프로그램 창을 의미합니다.
			D3DCREATE_HARDWARE_VERTEXPROCESSING, // 정점 처리를 그래픽 카드에게 맡긴다는 뜻입니다.
			&D3DPP,                              // 가상 디바이스 생성을 위한 정보를 넘겨줍니다.
			&m_pD3DDevice9);                     // 가상 디바이스의 객체 포인터를 받을 인터페이스 포인터입니다.
		
		DXERR_HANDLER(hDXResult);
		NULLCHK_EFAIL_RETURN(m_pD3DDevice9, "DirectX9 가상 디바이스 생성 실패!");

		RXLOG(false, "DirectX9 가상 디바이스 생성 성공!");
		return S_OK;
	}

	HRESULT RXMain_DX9::DriveMain()
	{
		MSG msg;
		::ZeroMemory(&msg, sizeof(msg));

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
			}
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
		return S_OK;
	}

	HRESULT RXMain_DX9::OnResetDevice()
	{
		return S_OK;
	}

} // namespace RX end
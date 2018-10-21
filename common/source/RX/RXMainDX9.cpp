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
#include "PCH.h"
#include "RXMainDX9.h"

extern RX::RXMain_DX9* g_pMainDX9 = nullptr;

// 메시지 핸들러입니다.
void OnKeyF12(); // 스크린샷을 찍습니다.

LRESULT CALLBACK DefaultWndDX9Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	DefaultWndProc(hWnd, msg, wParam, lParam);

	switch (msg)
	{
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_F12:
		{
			OnKeyF12();
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
		g_pMainDX9 = this;
	}

	RXMain_DX9::~RXMain_DX9()
	{
		
	}

	HRESULT RXMain_DX9::InitMain()
	{
		RXMain::InitMain();
		setWndProc(DefaultWndDX9Proc);

		if (FAILED(InitD3D9()))
		{			
			RXERRLOG_RETURN_EFAIL("D3D9 초기화 실패!");
		}

		return S_OK;
	}

	HRESULT RXMain_DX9::InitD3D9()
	{
		HRESULT reuslt = RXRendererDX9::Instance()->CreateDevice();
		RXRendererDX9::Instance()->DefaultRenderState();

		// 투영행렬은 한번만 초기화해도 됩니다.
		RXRendererDX9::Instance()->DefaultProjectionMatrix();
		return reuslt;
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
				// (1 / UPDATE_FRAME_PER_SECOND) 간격이 존재하므로
				// 아무리 빠른 하드웨어라 해도 초당 25로 고정되어있습니다.
				// 반면에 아무리 느린 하드웨어라 해도 업데이트를 MAX_FRAMESKIP만큼 한 뒤에
				// 렌더링하므로 어느 정도 게임 속도는 보장됩니다.
				while ( (::timeGetTime() > nextGameTime) &&
					    (updateCallCount < MAX_FRAMESKIP) )
				{
					if (RXRendererDX9::Instance()->IsLostDevice() == false)
					{
						Update();
					}

					nextGameTime += FRAME_SKIP_TIME; // 1000 / 25 = 40 => 0.04초의 간격
					++updateCallCount;

					//RXDEBUGLOG("업데이트 호출 수 : %d", updateCallCount);
				}

				// 이쪽은 하드웨어에 따라 성능 차이가 심합니다.
				// 렌더링을 수행하는 부분입니다.
				// 인터폴레이션은 0.0 ~ 1.0 사이로 옵니다.
				rRenderingInterpolation =
					float(::timeGetTime() + FRAME_SKIP_TIME - nextGameTime) / float(FRAME_SKIP_TIME);
				//RXDEBUGLOG("렌더링 보간값 : %.2f", rRenderingInterpolation);				
				RXRendererDX9::Instance()->ResetDrawCallCount();
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
		m_subFunc[static_cast<INT32>(SUBFUNC_TYPE::RELEASE)].subFunc();
		RXRendererDX9::Instance()->Release();
		RXMain::Release();
		return S_OK;
	}

	HRESULT RXMain_DX9::ResizeResolution(INT32 clientWidth, INT32 clientHeight)
	{
		RXMain::ResizeResolution(clientWidth, clientHeight);
		OnLostDevice();
		OnResetDevice();
		return S_OK;
	}

	HRESULT RXMain_DX9::ToggleFullScreenMode(bool bFullScreen)
	{
		RXMain::ToggleFullScreenMode(bFullScreen);
		OnLostDevice();
		OnResetDevice();
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
		if (RXRendererDX9::Instance()->IsLostDevice() == false)
		{
			RXRendererDX9::Instance()->BeginRender();

			if (FAILED(m_subFunc[static_cast<INT32>(SUBFUNC_TYPE::RENDER)].subFunc()))
			{
				RXERRLOG_RETURN_EFAIL("서브 렌더 실패했습니다!");
			}

			RXRendererDX9::Instance()->EndRender();
		}

		g_DXResult = RXRendererDX9::Instance()->Present();

		// 로스트 디바이스는 자주 발생할 수 있으므로 따로 에러 핸들러를 하지 않습니다.
		if (g_DXResult == D3DERR_DEVICELOST)
		{
			RXERRLOG("가상 디바이스 손실 발생!");
			RXRendererDX9::Instance()->setLostDevice(true);
			::SleepEx(100, TRUE); // 가상 디바이스가 손실되었다면 잠깐 CPU를 풀어줍니다.
		}

		// 가상 디바이스가 손실되었다면 처리해줍니다.
		// 위에서 잠깐의 시간을 주기 위해 m_bLostDevice를 이용합니다.
		if (RXRendererDX9::Instance()->IsLostDevice())
		{
			// 가상 디바이스를 계속 사용할 수 있는지 확인합니다.
			if (FAILED(g_DXResult = g_pD3DDevice9->TestCooperativeLevel()))
			{
				// 가상 디바이스가 완전히 손실되어서 리셋할 수 없는 상황입니다.
				// 이럴 경우에는 프로그램을 재실행해야 합니다.
				if (g_DXResult == D3DERR_DEVICELOST)
				{
					RXERRLOG_RETURN_EFAIL("가상 디바이스가 완전히 손실되었습니다!");
				}
				// 가상 디바이스를 리셋할 수 있는 상황입니다.
				// 가상 디바이스를 리셋하려면 가상 디바이스와 관련된
				// 모든 객체들의 상태를 로스트 상태로 만들어야 합니다.
				else if (g_DXResult == D3DERR_DEVICENOTRESET)
				{
					OnLostDevice();
					OnResetDevice();
				}
			}
			else
			{
				// 가상 디바이스를 사용할 수 있다면
				// 가상 디바이스 손실 상태를 초기화해줍니다.
				RXRendererDX9::Instance()->setLostDevice(false);
			}
		}

		return S_OK;
	}

	HRESULT RXMain_DX9::OnLostDevice()
	{
		RXRendererDX9::Instance()->OnLostDevice();

		if (FAILED(m_subFunc[static_cast<INT32>(SUBFUNC_TYPE::LOSTDEVICE)].subFunc()))
		{
			RXERRLOG_RETURN_EFAIL("서브 로스트 디바이스 실패했습니다!");
		}

		RXLOG("로스트 디바이스되었습니다!");
		return S_OK;
	}

	HRESULT RXMain_DX9::OnResetDevice()
	{
		RXRendererDX9::Instance()->OnResetDevice();

		if (FAILED(m_subFunc[static_cast<INT32>(SUBFUNC_TYPE::RESETDEVICE)].subFunc()))
		{
			RXERRLOG_RETURN_EFAIL("서브 리셋 디바이스 실패했습니다!");
		}

		::SleepEx(100, TRUE);

		RXLOG("리셋 디바이스되었습니다!");
		return S_OK;
	}

} // namespace RX end

void OnKeyF12()
{
	RX::RXRendererDX9::Instance()->SaveBackBufferToFile("Test");
}

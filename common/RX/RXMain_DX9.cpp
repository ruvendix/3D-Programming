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
 * DirectX9 ���� �ھ� ���� ��ƾ�Դϴ�.
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
			RXERRLOG_EFAIL_RETURN("D3D9 �ʱ�ȭ ����!");
		}

		return S_OK;
	}

	HRESULT RXMain_DX9::InitD3D9()
	{
		// DirectX9 ��ü�� �����մϴ�.
		// �������̽� �����Ϳ� �ν���ȭ�� ��ü�� ��ȯ���ݴϴ�.
		m_pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
		NULLCHK_EFAIL_RETURN(m_pD3D9, "DirectX9 ��ü ���� ����!");
		RXLOG(false, "DirectX9 ��ü ���� ����!");

		// ====================================================================================
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
		D3DPP.hDeviceWindow = m_hMainWnd;
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
		// ====================================================================================

		HRESULT hDXResult = S_OK;

		// ���� ����̽� ������ ���� ������ ���������Ƿ� ���� ����̽��� �������ݴϴ�.
		hDXResult = m_pD3D9->CreateDevice(
			D3DADAPTER_DEFAULT,                  // ����͸� ���ϴµ� ����� ������� �����ϸ� �˴ϴ�.
			D3DDEVTYPE_HAL,                      // HAL Device�� ����ϰڴٴ� ���Դϴ�.
			D3DPP.hDeviceWindow,                 // ���� ����̽��� Ÿ�� ���α׷� â�� �ǹ��մϴ�.
			D3DCREATE_HARDWARE_VERTEXPROCESSING, // ���� ó���� �׷��� ī�忡�� �ñ�ٴ� ���Դϴ�.
			&D3DPP,                              // ���� ����̽� ������ ���� ������ �Ѱ��ݴϴ�.
			&m_pD3DDevice9);                     // ���� ����̽��� ��ü �����͸� ���� �������̽� �������Դϴ�.
		
		DXERR_HANDLER(hDXResult);
		NULLCHK_EFAIL_RETURN(m_pD3DDevice9, "DirectX9 ���� ����̽� ���� ����!");

		RXLOG(false, "DirectX9 ���� ����̽� ���� ����!");
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
			// �ܺο����� ������ ���� ��Ʈ���� �����մϴ�.
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

				// ������Ʈ ��ƾ�� FPS�� ������ �޽��ϴ�.
				// 25 ������ ���ָ� �����ϴ�.
				// ������ �ϵ��� ���� ���� ���̰� ���� �����ϴ�.
				while ( (::timeGetTime() > nextGameTime) &&
					    (updateCallCount < MAX_FRAMESKIP) )
				{
					if (m_bLostDevice == false)
					{
						Update();
					}

					nextGameTime += FRAME_SKIP_TIME;
					++updateCallCount;

					//RXDEBUGLOG("������Ʈ ȣ�� �� : %d", updateCallCount);
				}

				// ������ �ϵ��� ���� ���� ���̰� ���մϴ�.
				// �������� �����ϴ� �κ��Դϴ�.
				// ���������̼��� 0.0 ~ 1.0 ���̷� �ɴϴ�.
				rRenderingInterpolation =
					float(::timeGetTime() + FRAME_SKIP_TIME - nextGameTime) / float(FRAME_SKIP_TIME);
				//RXDEBUGLOG("������ ������ : %.2f", rRenderingInterpolation);				
				Render(rRenderingInterpolation);

				// �������� �Ϸ��ϸ� ������ ī��Ʈ�� 1���� �˴ϴ�.
				// �������� ���� ������ �ʿ��ϹǷ� ������ ������ �˴ϴ�.
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
		// ���� ����̽��� ������ ���� �������մϴ�.
		if (m_bLostDevice == false)
		{
			BeginRender();

			if (FAILED(m_arrSubFunc[static_cast<INT32>(SUBFUNC_TYPE::RENDER)].func()))
			{
				RXERRLOG_EFAIL_RETURN("���� ���� ����!");
			}

			EndRender();
		}

		NULLCHK(m_pD3DDevice9);
		HRESULT hDXReuslt = m_pD3DDevice9->Present(nullptr, nullptr, nullptr, nullptr);

		// �ν�Ʈ ����̽��� ���� �߻��� �� �����Ƿ� ���� ���� �ڵ鷯�� ���� �ʽ��ϴ�.
		if (hDXReuslt == D3DERR_DEVICELOST)
		{
			RXERRLOG("���� ����̽� �ս� �߻�!");
			m_bLostDevice = true;
			Sleep(100); // ���� ����̽��� �սǵǾ��ٸ� ��� CPU�� Ǯ���ݴϴ�.
		}

		// ���� ����̽��� �սǵǾ��ٸ� ó�����ݴϴ�.
		// ������ ����� �ð��� �ֱ� ���� m_bLostDevice�� �̿��մϴ�.
		if (m_bLostDevice)
		{
			// ���� ����̽��� ��� ����� �� �ִ��� Ȯ���մϴ�.
			if (FAILED(hDXReuslt = m_pD3DDevice9->TestCooperativeLevel()))
			{
				// ���� ����̽��� ������ �սǵǾ ������ �� ���� ��Ȳ�Դϴ�.
				// �̷� ��쿡�� ���α׷��� ������ؾ� �մϴ�.
				if (hDXReuslt == D3DERR_DEVICELOST)
				{
					RXERRLOG_EFAIL_RETURN("���� ����̽��� ������ �սǵǾ����ϴ�!");
				}
				// ���� ����̽��� ������ �� �ִ� ��Ȳ�Դϴ�.
				// ���� ����̽��� �����Ϸ��� ���� ����̽��� ���õ�
				// ��� ��ü���� ���¸� �ν�Ʈ ���·� ������ �մϴ�.
				else if (hDXReuslt == D3DERR_DEVICENOTRESET)
				{
					OnLostDevice();
					OnResetDevice();
				}
			}
			else
			{
				// ���� ����̽��� ����� �� �ִٸ�
				// ���� ����̽� �ս� ���¸� �ʱ�ȭ���ݴϴ�.
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
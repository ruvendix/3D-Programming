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
#include "PCH.h"
#include "RXMainDX9.h"

extern RX::RXMain_DX9* g_pMainDX9 = nullptr;

// �޽��� �ڵ鷯�Դϴ�.
void OnKeyF12(); // ��ũ������ ����ϴ�.

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
			RXERRLOG_RETURN_EFAIL("D3D9 �ʱ�ȭ ����!");
		}

		return S_OK;
	}

	HRESULT RXMain_DX9::InitD3D9()
	{
		HRESULT reuslt = RXRendererDX9::Instance()->CreateDevice();
		RXRendererDX9::Instance()->DefaultRenderState();

		// ��������� �ѹ��� �ʱ�ȭ�ص� �˴ϴ�.
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
				// (1 / UPDATE_FRAME_PER_SECOND) ������ �����ϹǷ�
				// �ƹ��� ���� �ϵ����� �ص� �ʴ� 25�� �����Ǿ��ֽ��ϴ�.
				// �ݸ鿡 �ƹ��� ���� �ϵ����� �ص� ������Ʈ�� MAX_FRAMESKIP��ŭ �� �ڿ�
				// �������ϹǷ� ��� ���� ���� �ӵ��� ����˴ϴ�.
				while ( (::timeGetTime() > nextGameTime) &&
					    (updateCallCount < MAX_FRAMESKIP) )
				{
					if (RXRendererDX9::Instance()->IsLostDevice() == false)
					{
						Update();
					}

					nextGameTime += FRAME_SKIP_TIME; // 1000 / 25 = 40 => 0.04���� ����
					++updateCallCount;

					//RXDEBUGLOG("������Ʈ ȣ�� �� : %d", updateCallCount);
				}

				// ������ �ϵ��� ���� ���� ���̰� ���մϴ�.
				// �������� �����ϴ� �κ��Դϴ�.
				// ���������̼��� 0.0 ~ 1.0 ���̷� �ɴϴ�.
				rRenderingInterpolation =
					float(::timeGetTime() + FRAME_SKIP_TIME - nextGameTime) / float(FRAME_SKIP_TIME);
				//RXDEBUGLOG("������ ������ : %.2f", rRenderingInterpolation);				
				RXRendererDX9::Instance()->ResetDrawCallCount();
				Render(rRenderingInterpolation);

				// �������� �Ϸ��ϸ� ������ ī��Ʈ�� 1���� �˴ϴ�.
				// �������� ���� ������ �ʿ��ϹǷ� ������ ������ �˴ϴ�.
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
		// ���� ����̽��� ������ ���� �������մϴ�.
		if (RXRendererDX9::Instance()->IsLostDevice() == false)
		{
			RXRendererDX9::Instance()->BeginRender();

			if (FAILED(m_subFunc[static_cast<INT32>(SUBFUNC_TYPE::RENDER)].subFunc()))
			{
				RXERRLOG_RETURN_EFAIL("���� ���� �����߽��ϴ�!");
			}

			RXRendererDX9::Instance()->EndRender();
		}

		g_DXResult = RXRendererDX9::Instance()->Present();

		// �ν�Ʈ ����̽��� ���� �߻��� �� �����Ƿ� ���� ���� �ڵ鷯�� ���� �ʽ��ϴ�.
		if (g_DXResult == D3DERR_DEVICELOST)
		{
			RXERRLOG("���� ����̽� �ս� �߻�!");
			RXRendererDX9::Instance()->setLostDevice(true);
			::SleepEx(100, TRUE); // ���� ����̽��� �սǵǾ��ٸ� ��� CPU�� Ǯ���ݴϴ�.
		}

		// ���� ����̽��� �սǵǾ��ٸ� ó�����ݴϴ�.
		// ������ ����� �ð��� �ֱ� ���� m_bLostDevice�� �̿��մϴ�.
		if (RXRendererDX9::Instance()->IsLostDevice())
		{
			// ���� ����̽��� ��� ����� �� �ִ��� Ȯ���մϴ�.
			if (FAILED(g_DXResult = g_pD3DDevice9->TestCooperativeLevel()))
			{
				// ���� ����̽��� ������ �սǵǾ ������ �� ���� ��Ȳ�Դϴ�.
				// �̷� ��쿡�� ���α׷��� ������ؾ� �մϴ�.
				if (g_DXResult == D3DERR_DEVICELOST)
				{
					RXERRLOG_RETURN_EFAIL("���� ����̽��� ������ �սǵǾ����ϴ�!");
				}
				// ���� ����̽��� ������ �� �ִ� ��Ȳ�Դϴ�.
				// ���� ����̽��� �����Ϸ��� ���� ����̽��� ���õ�
				// ��� ��ü���� ���¸� �ν�Ʈ ���·� ������ �մϴ�.
				else if (g_DXResult == D3DERR_DEVICENOTRESET)
				{
					OnLostDevice();
					OnResetDevice();
				}
			}
			else
			{
				// ���� ����̽��� ����� �� �ִٸ�
				// ���� ����̽� �ս� ���¸� �ʱ�ȭ���ݴϴ�.
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
			RXERRLOG_RETURN_EFAIL("���� �ν�Ʈ ����̽� �����߽��ϴ�!");
		}

		RXLOG("�ν�Ʈ ����̽��Ǿ����ϴ�!");
		return S_OK;
	}

	HRESULT RXMain_DX9::OnResetDevice()
	{
		RXRendererDX9::Instance()->OnResetDevice();

		if (FAILED(m_subFunc[static_cast<INT32>(SUBFUNC_TYPE::RESETDEVICE)].subFunc()))
		{
			RXERRLOG_RETURN_EFAIL("���� ���� ����̽� �����߽��ϴ�!");
		}

		::SleepEx(100, TRUE);

		RXLOG("���� ����̽��Ǿ����ϴ�!");
		return S_OK;
	}

} // namespace RX end

void OnKeyF12()
{
	RX::RXRendererDX9::Instance()->SaveBackBufferToFile("Test");
}

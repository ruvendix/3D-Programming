/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <�ۼ� ��¥>
 * 2018-10-15
 *
 * <���� ����>
 * DirectX9 �������Դϴ�.
 * ���� ����̽��� �����ؼ� �������� �ٽ��Դϴ�.
 *
 ====================================================================================*/
#include "PCH.h"
#include "RXRendererDX9.h"
#include "RXRendererDX9_Util.h"

PHOENIX_SINGLETON_INIT(RX::RXRendererDX9);

extern IDirect3D9*        g_pD3D9       = nullptr;
extern IDirect3DDevice9*  g_pD3DDevice9 = nullptr;
extern HRESULT            g_DXResult    = S_OK;

namespace RX
{

	void RXRendererDX9::Init()
	{
		m_bLostDevice = false;
		m_bMSAA       = true;
		m_pD3D9       = nullptr;
		m_pD3DDevice9 = nullptr;
		m_clearColor  = DXCOLOR_TEAL;
		m_dwBehavior  = 0;
		m_drawCallCnt = 0;
	}

	void RXRendererDX9::ArrangeVideoMemory()
	{
		g_DXResult = m_pD3DDevice9->EvictManagedResources();
		DXERR_HANDLER(g_DXResult);
		RXLOG(false, "D3DPOOL_DEFAULT ���ҽ��� �����Ǿ����ϴ�.");
	}

	HRESULT RXRendererDX9::CreateDevice()
	{
		Init();

		// DirectX9 ��ü�� �����մϴ�.
		// �������̽� �����Ϳ� �ν���ȭ�� ��ü�� ��ȯ���ݴϴ�.
		m_pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
		NULLCHK_RETURN_EFAIL(m_pD3D9, "DirectX9 ��ü ���� �����߽��ϴ�!");
		RXLOG(false, "DirectX9 ��ü ���� �����߽��ϴ�!");
		g_pD3D9 = m_pD3D9;
		
		// ====================================================================================
		// ���� ����̽� ������ ���� ������ �������ݴϴ�.
		// �Ϲ������� Present Parameters�� �ٿ��� PP��� �����մϴ�.
		D3DPRESENT_PARAMETERS D3DPP;
		::ZeroMemory(&D3DPP, sizeof(D3DPP));

		// ====================================================================================
		// ������ �ʿ� ���� �������� ä���ݴϴ�.
		//
		// ������ �ø����� �� ���� ȿ���� �����մϴ�.
		// �ʹݿ��� D3DSWAPEFFECT_DISCARD�� ���� �����մϴ�.
		D3DPP.SwapEffect = D3DSWAPEFFECT_DISCARD;

		// Ư���� �÷��׸� �����մϴ�.
		// ���� ����� �÷��װ� ������ 0���� �����մϴ�.
		D3DPP.Flags = 0;

		// ���α׷� â ������ �����մϴ�.
		// ���α׷� â �ڵ�� â ��� ���θ� �������ָ� �˴ϴ�.
		// ���߿� ��ü ȭ�鵵 �����ϰ� ������ �ʹݿ��� â ��常 ����մϴ�.
		D3DPP.hDeviceWindow = g_pMainDX9->getMainWindowHandle();
		D3DPP.Windowed      = (g_pMainDX9->IsFullScreen() == false);
		
		// ====================================================================================
		// ���� ����̽� ������ �������� �����մϴ�.
		VerifyDevice(&D3DPP);

		// ====================================================================================
		// ���� ����̽� ������ ���� ������ ���������Ƿ� ���� ����̽��� �������ݴϴ�.
		g_DXResult = m_pD3D9->CreateDevice(
			D3DADAPTER_DEFAULT,     // ����͸� ���ϴµ� ����� ������� �����ϸ� �˴ϴ�.
			D3DDEVTYPE_HAL,         // HAL Device�� ����ϰڴٴ� ���Դϴ�.
			D3DPP.hDeviceWindow,    // ���� ����̽��� Ÿ�� ���α׷� â�� �ǹ��մϴ�.
			m_dwBehavior,           // ���� ó���� �׷��� ī�忡�� �ñ�ٴ� ���Դϴ�.
			&D3DPP,                 // ���� ����̽� ������ ���� ������ �Ѱ��ݴϴ�.
			&m_pD3DDevice9);        // ���� ����̽��� ��ü �����͸� ���� �������̽� �������Դϴ�.
	
		DXERR_HANDLER(g_DXResult);
		NULLCHK_RETURN_EFAIL(m_pD3DDevice9, "DirectX9 ���� ����̽� ���� �����߽��ϴ�!");
		g_pD3DDevice9 = m_pD3DDevice9;

		RXLOG(false, "DirectX9 ���� ����̽� ���� �����߽��ϴ�!");
		return S_OK;
	}

	HRESULT RXRendererDX9::BeginRender()
	{
		m_pD3DDevice9->Clear(0, nullptr,
			D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, m_clearColor, 1.0f, 0);
		m_pD3DDevice9->BeginScene();
		return S_OK;
	}

	HRESULT RXRendererDX9::EndRender()
	{
		m_pD3DDevice9->EndScene();
		return S_OK;
	}

	// ���� ����̽��� �����ϱ� ���� �������ִ� �Լ��Դϴ�.
	// �����ϴ� ������ ������ �����ϴ�.
	//
	// 1. �׷��� ī���� ��� �ɷ� ����(����� ȣȯ)
	// 2. �ϵ���� ������ �������� ����
	// 3. MSAA ����
	// 4. ���� ó�� ��� ����(���̴�)
	HRESULT RXRendererDX9::VerifyDevice(D3DPRESENT_PARAMETERS* pD3DPP)
	{
		// ====================================================================================
		// �׷��� ī���� ��� �ɷ��� �����մϴ�.
		D3DDISPLAYMODE mainDisplayMode;
		::ZeroMemory(&mainDisplayMode, sizeof(mainDisplayMode));

		// �׷��� ī���� ��� ������ �����ɴϴ�.
		if (FAILED(m_pD3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mainDisplayMode)))
		{
			RXERRLOG_RETURN_EFAIL("�׷��� ī�� ���� ȹ�� �����߽��ϴ�!");
		}

		// ������� ������ �����մϴ�.
		// �������� 0�� �־ 1�� �νĵ����� �������� ���� 1�� �����մϴ�.
		pD3DPP->BackBufferCount = 1;

		// ����� ������ �����մϴ�.
		// ������� ����, �����Դϴ�.
		// ��ü ȭ���� ���� ���÷��� ��� ������ �������ݴϴ�.
		if (pD3DPP->Windowed == TRUE)
		{
			pD3DPP->BackBufferWidth  = g_pMainDX9->getClientWidth();
			pD3DPP->BackBufferHeight = g_pMainDX9->getClientHeight();
		}

		// ������� ������ �������ݴϴ�.
		pD3DPP->BackBufferFormat = mainDisplayMode.Format;

		// ������� �ֻ����� �����մϴ�.
		// â ��带 ����� ���� D3DPRESENT_RATE_DEFAULT�� �����ϸ� �˴ϴ�.
		if (pD3DPP->Windowed == TRUE)
		{
			pD3DPP->FullScreen_RefreshRateInHz = D3DPRESENT_INTERVAL_DEFAULT;
		}

		// ������� ������ �ø��� ������ �����մϴ�.
		pD3DPP->PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT; // �⺻�� ��������

		// ��ü ȭ������ ��ȯ�� ���� ó���Դϴ�.
		if ( (pD3DPP->Windowed == FALSE) &&
		 	 (AdjustFullScreenInfo(pD3DPP, mainDisplayMode) == false) )
		{
			RXERRLOG_RETURN_EFAIL("��ü ȭ�鿡�� �׷��� ī�尡 �ػ� �Ǵ� ������ �������� �ʽ��ϴ�!");
		}

		// ====================================================================================
		// �ϵ���� ������ �������� �����մϴ�.
		if (FAILED(m_pD3D9->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
			pD3DPP->BackBufferFormat, pD3DPP->BackBufferFormat, pD3DPP->Windowed)))
		{
			RXERRLOG_RETURN_EFAIL("�ϵ���� ������ �������� �ʽ��ϴ�!");
		}

		// ====================================================================================
		// MSAA(Multisample AntiAliasing)�� �����մϴ�.
		// AA�� ���� ����� ������ ���� �⺻���� �� MSAA�Դϴ�.
		if (m_bMSAA == true)
		{
			if (AdjustMSAAInfo(pD3DPP) == false)
			{
				RXERRLOG_RETURN_EFAIL("MSAA�� �������� �����Ƿ� 0���� �����մϴ�!");
			}
		}
		else
		{
			// ��Ƽ ���ø� ������ �����մϴ�.
			// MSAA(MultiSample AntiAliasing)��� �մϴ�.
			// �ʹݿ��� ����� ���� �����Ƿ� 0���� �����մϴ�.
			pD3DPP->MultiSampleType    = D3DMULTISAMPLE_NONE;
			pD3DPP->MultiSampleQuality = 0;
		}

		// ====================================================================================
		// ���� ó�� ����� �����մϴ�.
		D3DCAPS9 D3DCaps9;
		::ZeroMemory(&D3DCaps9, sizeof(D3DCaps9));

		// �׷��� ī���� �ɷ� ������ �����ɴϴ�.
		m_pD3D9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &D3DCaps9);

		// �׷��� ī�尡 TL, ���� ���̴� 1.0, �ȼ� ���̴� 1.0�� �������� ������
		// ���� ó�� ����� ����Ʈ���� ���� ó���� �����մϴ�.
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

		// ====================================================================================
		// ���� ���ٽǹ��� ������ �����մϴ�.
		// �Ϲ������δ� ���̹��� 24��Ʈ�� ���ٽǹ��� 8��Ʈ�� ����մϴ�.
		AdjustDepthStencilInfo(pD3DPP, 24, 8);

		return S_OK;
	}

	HRESULT RXRendererDX9::DrawPrimitive(D3DPRIMITIVETYPE primitiveType,
		const RXVertexBufferDX9& vertexBuffer)
	{
		g_pD3DDevice9->SetFVF(VertexInfo::FORMAT);
		g_pD3DDevice9->SetStreamSource(
			0,                    // ��Ʈ�� �ѹ��� 0���� �����մϴ�.
			vertexBuffer.getVB(), // ���� ���۸� �������ݴϴ�.
			0,                    // �������� 0���� �����մϴ�.
			sizeof(VertexInfo));  // ����(Stride)�� FVF�� ������ ũ��� ��ġ�ؾ� �մϴ�.

		g_pD3DDevice9->DrawPrimitive(
			primitiveType, // ������ ������ �����մϴ�.
			0,             // �������� 0���� �����մϴ�.
			// ������Ƽ�� �����Դϴ�.
			CalcPrimitiveCount(primitiveType, vertexBuffer.getVertexCount()));

		return S_OK;
	}

	HRESULT RXRendererDX9::DrawIndexedPrimitive(const RXVertexBufferDX9& vertexBuffer,
		const RXIndexBufferDX9& indexBuffer)
	{
		g_pD3DDevice9->SetFVF(VertexInfo::FORMAT);
		g_pD3DDevice9->SetStreamSource(
			0,                    // ��Ʈ�� �ѹ��� 0���� �����մϴ�.
			vertexBuffer.getVB(), // ���� ���۸� �������ݴϴ�.
			0,                    // �������� 0���� �����մϴ�.
			sizeof(VertexInfo));  // ����(Stride)�� FVF�� ������ ũ��� ��ġ�ؾ� �մϴ�.

		// �ε��� ���۸� ���� ����̽��� �������ݴϴ�.
		g_pD3DDevice9->SetIndices(indexBuffer.getIB());

		g_pD3DDevice9->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST, // ������ ������ �����մϴ�.
			0,                  // ���� ���ۿ��� ������ ���� �ε����� �����մϴ�. (0���� ����)
			0,                  // ������ �ε����� �����մϴ�. (0���� ����)
			vertexBuffer.getVertexCount(), // ���� ������ �����մϴ�.
			0,                  // �ε��� ������ �������Դϴ�. (0���� ����)
			indexBuffer.getTriangleCount()); // ������Ƽ�� �����Դϴ�.

		// �ε��� ������ ������Ƽ�� ������
		// �⺻ ������ �� D3DPT_TRIANGLESTRIP�� ����������
		// �׷��� ���� ��찡 �� �����Ƿ� ������ �����ؾ� �մϴ�.

		return S_OK;
	}

	HRESULT RXRendererDX9::OnLostDevice()
	{
		return S_OK;
	}

	HRESULT RXRendererDX9::OnResetDevice()
	{
		D3DPRESENT_PARAMETERS D3DPP;
		::ZeroMemory(&D3DPP, sizeof(D3DPP));

		// ������ �ʿ� ���� ������ �����մϴ�.
		D3DPP.EnableAutoDepthStencil = true;
		D3DPP.AutoDepthStencilFormat = D3DFMT_D24S8;
		D3DPP.SwapEffect             = D3DSWAPEFFECT_DISCARD;
		D3DPP.Flags                  = 0;
		D3DPP.hDeviceWindow          = g_pMainDX9->getMainWindowHandle();
		D3DPP.Windowed               = (g_pMainDX9->IsFullScreen() == false);

		// ������ �ʿ��� ������ �����մϴ�.
		VerifyDevice(&D3DPP);

		m_pD3DDevice9->Reset(&D3DPP);
		return S_OK;
	}

	HRESULT RXRendererDX9::Present()
	{
		m_pD3DDevice9->Present(nullptr, nullptr, nullptr, nullptr);
		return S_OK;
	}

	HRESULT RXRendererDX9::Release()
	{
		if (m_pD3DDevice9 != nullptr)
		{
			int refCnt = m_pD3DDevice9->Release();

			if (refCnt > 0)
			{
				RXLOG(false, "DirectX9 ���� ����̽� ���� �����߽��ϴ�! ���۷��� ī��Ʈ(%d)", refCnt);
			}
			else
			{
				RXLOG(false, "DirectX9 ���� ����̽� ���� �����߽��ϴ�!");
			}

			m_pD3DDevice9 = nullptr;
		}

		SAFE_RELEASE(m_pD3D9);
		return S_OK;
	}

} // namespace RX end
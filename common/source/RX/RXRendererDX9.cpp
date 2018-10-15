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

PHOENIX_SINGLETON_INIT(RX::RXRendererDX9);

extern IDirect3DDevice9*  g_pD3DDevice9  = nullptr;
extern HRESULT            g_DXResult     = S_OK;

namespace RX
{

	void RXRendererDX9::Init()
	{
		m_bLostDevice = false;
		m_bMSAA       = false;
		m_pD3D9       = nullptr;
		m_pD3DDevice9 = nullptr;
		m_clearColor  = DXCOLOR_TEAL;
		m_dwBehavior  = 0;
		m_drawCallCnt = 0;
	}

	HRESULT RXRendererDX9::CreateDevice()
	{
		Init();

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

		// ====================================================================================
		// ���� ����̽� ������ �������� �����մϴ�.
		VerifyDevice(&D3DPP);

		// ====================================================================================
		// ������ ������ ������ ������ ä���ݴϴ�.
		//
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
		D3DPP.hDeviceWindow = g_pMainDX9->getMainWindowHandle();
		D3DPP.Windowed      = (g_pMainDX9->IsFullScreen() == false);
		
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
		NULLCHK_EFAIL_RETURN(m_pD3DDevice9, "DirectX9 ���� ����̽� ���� ����!");
		g_pD3DDevice9 = m_pD3DDevice9;

		RXLOG(false, "DirectX9 ���� ����̽� ���� ����!");
		return S_OK;
	}

	HRESULT RXRendererDX9::BeginRender()
	{
		m_pD3DDevice9->Clear(0, nullptr,
			D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_clearColor, 1.0f, 0);
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
			RXERRLOG_EFAIL_RETURN("�׷��� ī���� ��� ���� ȹ�� ����!");
		}

		// �ĸ� ������ ������ �����մϴ�.
		// �������� 0�� �־ 1�� �νĵ����� �������� ���� 1�� �����մϴ�.
		pD3DPP->BackBufferCount = 1;

		// �ĸ� ���� ������ �����մϴ�.
		// ������� ����, �����Դϴ�.
		// ��ü ȭ���� ���� ���÷��� ��� ������ �������ݴϴ�.
		bool bFullScreen = g_pMainDX9->IsFullScreen();
		if (bFullScreen == false)
		{
			pD3DPP->BackBufferWidth  = g_pMainDX9->getClientWidth();
			pD3DPP->BackBufferHeight = g_pMainDX9->getClientHeight();
		}

		// �ĸ� ������ ������ �������ݴϴ�.
		pD3DPP->BackBufferFormat = mainDisplayMode.Format;

		// �ĸ� ������ �ֻ����� �����մϴ�.
		// â ��带 ����� ���� D3DPRESENT_RATE_DEFAULT�� �����ϸ� �˴ϴ�.
		if (bFullScreen == false)
		{
			pD3DPP->FullScreen_RefreshRateInHz = D3DPRESENT_INTERVAL_DEFAULT;
		}

		// �ĸ� ������ ������ �ø��� ������ �����մϴ�.
		pD3DPP->PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT; // �⺻�� ��������

		// ��ü ȭ������ ��ȯ�� ���� ó���Դϴ�.
		if (bFullScreen)
		{
			bool bResult = false;

			// ��ü ȭ������ ��ȯ�� �� ��� ������ ��� ������ �����ɴϴ�.
			// ���� �׷��� ī�尡 �����ϴ� �ػ󵵸� ���մϴ�.
			// ���� ��� ���� ����� �ػ󵵴� 1920 X 1080������
			// �׷��� ī��� 800 X 600�� �����մϴ�. �̷� �� ����Դϴ�.
			INT32 adapterCount =
				m_pD3D9->GetAdapterModeCount(D3DADAPTER_DEFAULT, mainDisplayMode.Format);

			for (INT32 i = 0; i < adapterCount; ++i)
			{
				D3DDISPLAYMODE subDisplayMode;
				::ZeroMemory(&subDisplayMode, sizeof(subDisplayMode));

				// ���ϴ� �ػ� �Ǵ� ���İ� ȣȯ�� �Ǵ��� �����մϴ�.
				HRESULT hDXResult = m_pD3D9->EnumAdapterModes(D3DADAPTER_DEFAULT,
					mainDisplayMode.Format, i, &subDisplayMode);
				DXERR_HANDLER(hDXResult);
	
				// ���� �ػ󵵿� ��ü ȭ�� �ػ󵵸� ���մϴ�.
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
				RXERRLOG_EFAIL_RETURN("��ü ȭ���� �� ���� �׷��� ī�尡 �ػ� �Ǵ� ������ �������� ����!");
			}
		}

		// ====================================================================================
		// �ϵ���� ������ �������� �����մϴ�.
		if (FAILED(m_pD3D9->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
			pD3DPP->BackBufferFormat, pD3DPP->BackBufferFormat, !bFullScreen)))
		{
			RXERRLOG_EFAIL_RETURN("�ϵ���� ������ �������� ����!");
		}

		// ====================================================================================
		// MSAA(Multisample AntiAliasing)�� �����մϴ�.
		// AA�� ���� ����� ������ ���� �⺻���� �� MSAA�Դϴ�.
		// 2, 4, 8, 16�� �����մϴ�.
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
					D3DDEVTYPE_HAL, mainDisplayMode.Format, !bFullScreen, type, &dwMSAAQuality)))
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
				RXERRLOG_EFAIL_RETURN("MSAA�� �������� �����Ƿ� 0���� ������!");
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

		return S_OK;
	}

	INT32 RXRendererDX9::CalcPrimitiveCount(D3DPRIMITIVETYPE primitiveType, INT32 vertexCnt)
	{
		INT32 primitiveCnt = 0;
		switch (primitiveType)
		{
		case D3DPT_POINTLIST:
			primitiveCnt = vertexCnt;
			break;
		case D3DPT_LINELIST:
			primitiveCnt = vertexCnt / 2;
			break;
		case D3DPT_LINESTRIP:
			primitiveCnt = vertexCnt - 1;
			break;
		case D3DPT_TRIANGLELIST:
			primitiveCnt = vertexCnt / 3;
			break;
		case D3DPT_TRIANGLESTRIP:
			primitiveCnt = vertexCnt - 2;
			break;
		case D3DPT_TRIANGLEFAN:
			primitiveCnt = vertexCnt - 2;
			break;
		}
		return primitiveCnt;
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

		VerifyDevice(&D3DPP);

		D3DPP.EnableAutoDepthStencil = true;
		D3DPP.AutoDepthStencilFormat = D3DFMT_D24S8;
		D3DPP.SwapEffect             = D3DSWAPEFFECT_DISCARD;
		D3DPP.Flags                  = 0;
		D3DPP.hDeviceWindow          = g_pMainDX9->getMainWindowHandle();
		D3DPP.Windowed               = (g_pMainDX9->IsFullScreen() == false);

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
		SAFE_RELEASE(m_pD3DDevice9);
		SAFE_RELEASE(m_pD3D9);
		return S_OK;
	}

} // namespace RX end
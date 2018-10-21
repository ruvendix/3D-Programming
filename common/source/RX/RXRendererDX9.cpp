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
		m_bMSAA       = false;
		m_bVSync      = false;
		m_pD3D9       = nullptr;
		m_pD3DDevice9 = nullptr;
		m_clearColor  = DXCOLOR_TEAL;
		m_adapterIdx  = 0;
		m_dwBehavior  = 0;
		m_drawCallCnt = 0;

		::SetRect(&m_rtScissor, 0, 0, 0, 0);
		::ZeroMemory(&m_viewport9, sizeof(m_viewport9));
	}

	void RXRendererDX9::ResetD3DPP()
	{
		// ====================================================================================
		// ���� ����̽��� �⺻ ������ �����մϴ�.
		// �Ϲ������� Present Parameters�� �ٿ��� PP��� �����մϴ�.
		SAFE_DELTE(m_pD3DPP);
		m_pD3DPP = RXNew D3DPRESENT_PARAMETERS;
		NULLCHK_HEAPALLOC(m_pD3DPP);
		::ZeroMemory(m_pD3DPP, sizeof(D3DPRESENT_PARAMETERS));

		// ������ �ø����� �� ���� ȿ���� �����մϴ�.
		// �ʹݿ��� D3DSWAPEFFECT_DISCARD�� ���� �����մϴ�.
		m_pD3DPP->SwapEffect = D3DSWAPEFFECT_DISCARD;
		
		// Ư���� �÷��׸� �����մϴ�.
		// ���� ����� �÷��װ� ������ 0���� �����մϴ�.
		m_pD3DPP->Flags = D3DFLAG_NONE;

		// ���α׷� â ������ �����մϴ�.
		// ���α׷� â �ڵ�� â ȭ�� ���θ� �������ָ� �˴ϴ�.
		// ���߿� ��ü ȭ�鵵 �����ϰ� ������ �ʹݿ��� â ȭ�鸸 ����մϴ�.
		m_pD3DPP->hDeviceWindow = g_pMainDX9->getMainWindowHandle();
		m_pD3DPP->Windowed      = (g_pMainDX9->IsFullScreen() == false);
	}

	void RXRendererDX9::ArrangeVideoMemory()
	{
		g_DXResult = m_pD3DDevice9->EvictManagedResources();
		DXERR_HANDLER(g_DXResult);
		RXLOG("D3DPOOL_DEFAULT ���ҽ��� �����Ǿ����ϴ�.");
	}

	void RXRendererDX9::DefaultRenderState()
	{
		// rhw�� ������� �ʴ´ٸ� ��ȯ ������ ������ǥ�� ����ϰ� �ǹǷ�
		// ���� ��ȯ ������ ���ľ� �մϴ�. ����(������, Lighting)�� ���� �ϳ��ε�
		// ���� ���� ������ ���� ���� �ʾ����Ƿ� ������ ����� �մϴ�.
		m_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, false);

		// ä��� �����Դϴ�.
		//m_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
		//m_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		m_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

		// �ø� �����Դϴ�.
		//m_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		//m_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		m_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	}

	void RXRendererDX9::AdjustRenderState(D3DRENDERSTATETYPE state, DWORD dwValue)
	{
		m_pD3DDevice9->SetRenderState(state, dwValue);
	}

	void RXRendererDX9::AdjustViewport(INT32 x, INT32 y, INT32 width, INT32 height)
	{
		if ( (width <= 0) ||
			 (height <= 0) )
		{
			width  = m_pD3DPP->BackBufferWidth;
			height = m_pD3DPP->BackBufferHeight;
		}

		m_viewport9.X      = x;
		m_viewport9.Y      = y;
		m_viewport9.Width  = width;
		m_viewport9.Height = height;
		m_viewport9.MinZ   = 0.0f;
		m_viewport9.MaxZ   = 1.0f;

		g_DXResult = m_pD3DDevice9->SetViewport(&m_viewport9);
		DXERR_HANDLER(g_DXResult);
	}

	void RXRendererDX9::SaveBackBufferToFile(const CHAR* szFile, D3DXIMAGE_FILEFORMAT fileFormat)
	{
		// ��ũ������ ������ ������ ������ �������ݴϴ�.
		static bool bInitScreenshot = true;
		if (bInitScreenshot == true)
		{
			bInitScreenshot = false;
			::CreateDirectoryA("Screenshot", nullptr);
		}

		CHAR szFull[DEFAULT_STRING_LENGTH] = "";
		strcat_s(szFull, "Screenshot\\");
		strcat_s(szFull, szFile);

		switch (fileFormat)
		{
		case D3DXIFF_BMP:
		{
			strcat_s(szFull, ".bmp");
			break;
		}
		case D3DXIFF_JPG:
		{
			strcat_s(szFull, ".jpg");
			break;
		}
		case D3DXIFF_TGA:
		{
			strcat_s(szFull, ".tga");
			break;
		}
		case D3DXIFF_PNG:
		{
			strcat_s(szFull, ".png");
			break;
		}
		default:
		{
			RXERRLOG("��ũ�������� ����� �� ���� �̹��� �����Դϴ�!",
				ConvertD3DXIMAGE_FILEFORMATToString(fileFormat));
			return;
		}
		}

		// ��ũ������ ��� ����� 3���� ���� �Ǵµ� ���� �ڷḦ ����ϴ�.
		// ����� ���� �ӵ� : https://stackoverflow.com/questions/23118686/execution-time-getfrontbufferdata-getbackbuffer-getrendertargetdata#
		// ���� �ڵ� : https://gist.github.com/vinjn/2922506

		// ���� ���� ���� Ÿ���� �����̽��� �����ɴϴ�.
		// ���� Ÿ���̶� ���� ����̽��� �������� ����� ���ϴµ�
		// �Ϲ������� ���� Ÿ���̶� �ϸ� ���� Ÿ���� �����̽��� �ǹ��մϴ�.
		// �ϳ��� ���� Ÿ������ �������ϴ� ����� SPR(Single-Path Rendering)�̶� �ϰ�,
		// ���� ���� ���� Ÿ������ �������ϴ� ����� MPR(Multi-Path Rendering)�̶�� �մϴ�.
		IDirect3DSurface9* pBackBuffer = nullptr;
		m_pD3DDevice9->GetRenderTarget(0, &pBackBuffer); // 0�̸� ������Դϴ�.
		NULLCHK_RETURN_NOCOMENT(pBackBuffer);

		// ������ �����̽��� ������ �����̽��� �ϳ� �������ݴϴ�.
		IDirect3DSurface9* pDestSurface = nullptr;
		m_pD3DDevice9->CreateOffscreenPlainSurface(m_pD3DPP->BackBufferWidth,
			m_pD3DPP->BackBufferHeight, m_pD3DPP->BackBufferFormat,
			D3DPOOL_SYSTEMMEM, &pDestSurface, nullptr);
		NULLCHK_RETURN_NOCOMENT(pDestSurface);

		// ����۸� ������ �����̽��� �����մϴ�.
		g_DXResult = m_pD3DDevice9->GetRenderTargetData(pBackBuffer, pDestSurface);
		DXERR_HANDLER(g_DXResult);

		// ������ �����̽��� ���Ϸ� �����մϴ�.
		g_DXResult = D3DXSaveSurfaceToFileA(szFull, fileFormat, pDestSurface, NULL, NULL);
		DXERR_HANDLER(g_DXResult);

		// �����̽��� �����մϴ�.
		pBackBuffer->Release();
		pDestSurface->Release();

		RXMSGBOXLOG("��ũ������ ����Ǿ����ϴ�.");
	}

	HRESULT RXRendererDX9::CreateDevice()
	{
		Init();

		// DirectX9 ��ü�� �����մϴ�.
		// �������̽� �����Ϳ� �ν���ȭ�� ��ü�� ��ȯ���ݴϴ�.
		m_pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
		NULLCHK_RETURN_EFAIL(m_pD3D9, "DirectX9 ��ü ���� �����߽��ϴ�!");
		RXLOG("DirectX9 ��ü ���� �����߽��ϴ�!");
		g_pD3D9 = m_pD3D9;
		
		// ====================================================================================
		// ���� ����̽� ������ ���� ������ �������ݴϴ�.
		ResetD3DPP();
		
		// ====================================================================================
		// ���� ����̽� ������ �������� �����մϴ�.
		VerifyDevice(m_pD3DPP);

		// ====================================================================================
		// ���� ����̽� ������ ���� ������ ���������Ƿ� ���� ����̽��� �������ݴϴ�.
		g_DXResult = m_pD3D9->CreateDevice(
			m_adapterIdx,            // ����͸� ���ϴµ� ����� ������� �����ϸ� �˴ϴ�.
			D3DDEVTYPE_HAL,          // HAL Device�� ����ϰڴٴ� ���Դϴ�.
			m_pD3DPP->hDeviceWindow, // ���� ����̽��� Ÿ�� ���α׷� â�� �ǹ��մϴ�.
			m_dwBehavior,            // ���� ó���� �׷��� ī�忡�� �ñ�ٴ� ���Դϴ�.
			m_pD3DPP,                // ���� ����̽� ������ ���� ������ �Ѱ��ݴϴ�.
			&m_pD3DDevice9);         // ���� ����̽��� ��ü �����͸� ���� �������̽� �������Դϴ�.
	
		DXERR_HANDLER(g_DXResult);
		NULLCHK_RETURN_EFAIL(m_pD3DDevice9, "DirectX9 ���� ����̽� ���� �����߽��ϴ�!");
		g_pD3DDevice9 = m_pD3DDevice9;

		RXLOG("DirectX9 ���� ����̽� ���� �����߽��ϴ�!");
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
		g_DXResult = m_pD3DDevice9->EndScene();
		DXERR_HANDLER(g_DXResult);
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
		NULLCHK_RETURN_EFAIL_NOCOMENT(pD3DPP);

		// ====================================================================================
		// �׷��� ī���� ��� �ɷ��� �����մϴ�.
		D3DDISPLAYMODE mainDisplayMode;
		::ZeroMemory(&mainDisplayMode, sizeof(mainDisplayMode));

		// ����� �ε����� ã���ϴ�.
		m_adapterIdx = FindAdapterIndex(pD3DPP->hDeviceWindow);
		RXLOG("���� ����� �ε��� : %d", m_adapterIdx);

		// �׷��� ī���� ��� ������ �����ɴϴ�.
		if (FAILED(m_pD3D9->GetAdapterDisplayMode(m_adapterIdx, &mainDisplayMode)))
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
		// â ȭ���� ����� ���� D3DPRESENT_RATE_DEFAULT�� �����ϸ� �˴ϴ�.
		if (pD3DPP->Windowed == TRUE)
		{
			pD3DPP->FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		}

		// ������� ������ �ø��� ������ �����մϴ�.
		pD3DPP->PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT; // �⺻�� ��������

		if (pD3DPP->PresentationInterval == D3DPRESENT_INTERVAL_DEFAULT)
		{
			m_bVSync = true;
		}

		// ��ü ȭ������ ��ȯ�� ���� ó���Դϴ�.
		if ( (pD3DPP->Windowed == FALSE) &&
			 (AdjustFullScreenInfo(m_adapterIdx, pD3DPP, mainDisplayMode) == false) )
		{
			RXERRLOG_RETURN_EFAIL("��ü ȭ�鿡�� �׷��� ī�尡 �ػ� �Ǵ� ������ �������� �ʽ��ϴ�!");
		}

		// ====================================================================================
		// �ϵ���� ������ �������� �����մϴ�.
		if (FAILED(m_pD3D9->CheckDeviceType(m_adapterIdx, D3DDEVTYPE_HAL,
			pD3DPP->BackBufferFormat, pD3DPP->BackBufferFormat, pD3DPP->Windowed)))
		{
			RXERRLOG_RETURN_EFAIL("�ϵ���� ������ �������� �ʽ��ϴ�!");
		}

		// ====================================================================================
		// MSAA(Multisample AntiAliasing)�� �����մϴ�.
		// AA�� ���� ����� ������ ���� �⺻���� �� MSAA�Դϴ�.
		if (m_bMSAA == true)
		{
			if (AdjustMSAAInfo(m_adapterIdx, pD3DPP) == false)
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
		m_pD3D9->GetDeviceCaps(m_adapterIdx, D3DDEVTYPE_HAL, &D3DCaps9);

		// �׷��� ī�尡 TL(Transformation And Lighting)�� �����ϰų�
		// ���� ���̴��� �ȼ� ���̴��� 1.0 �̻��̶��
		// ���� ó�� ����� ����Ʈ���� ���� ó���� �����մϴ�.
		if ( (D3DCaps9.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) ||
			 ( (D3DCaps9.VertexShaderVersion > D3DVS_VERSION(1, 0)) &&
			   (D3DCaps9.PixelShaderVersion > D3DPS_VERSION(1, 0))) )
		{
			// D3DPS_VERSION()�� (0xFFFF0000|((_Major)<<8)|(_Minor))�ε�
			// _Major�� 1����Ʈ��ŭ �������� �а� _Minor�� OR �����մϴ�.
			// �� ���� 0xFFFF0000�� OR �����ϴµ� ���� 2����Ʈ�� �ٽ��Դϴ�.
			// ���� 2����Ʈ�� 1�� ����µ� ���� ���� Ȯ���� ���� 2����Ʈ�Դϴ�.
			// D3DPS_VERSION(1, 1)�� 0xFFFF0101�� �˴ϴ�. ��, �ȼ� ���̴� 1.1�� �ǹ��մϴ�.
			//
			// �ٸ� ������� �˾Ƴ����� PixelShaderVersion >> 8�� �ؼ� _Minor�� ����������
			// 0xFF�� AND �����ؼ� ������ ����Ʈ�� �̾Ƴ��ϴ�.
			// _Minor�� PixelShaderVersion & 0xFF�ؼ� ������ ����Ʈ�� �̾Ƴ��ϴ�.
			// ��, _Major�� _Minor�� �и��ϴ� ����Դϴ�.
			m_dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		}
		else
		{
			// ����Ʈ���� ���� ó���� ����ؾ� �Ѵٸ� �޽��� �ڽ���
			// ���� �׷��� ī�忡 ���� ������ �����ϰ� �˷��ݴϴ�.
			D3DADAPTER_IDENTIFIER9 adapterInfo;
			::ZeroMemory(&adapterInfo, sizeof(adapterInfo));
			g_pD3D9->GetAdapterIdentifier(m_adapterIdx, D3DENUM_WHQL_LEVEL, &adapterInfo);
			RXERRLOG_CHAR("�ϵ���� ���� ó���� �������� �ʽ��ϴ�!\n�׷��� ī�� (%s)",
				adapterInfo.Description);

			// ����Ʈ���� ���� ó���� �����մϴ�.
			m_dwBehavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		}

		// ====================================================================================
		// ���� ���ٽǹ��� ������ �����մϴ�.
		// �Ϲ������δ� ���̹��� 24��Ʈ�� ���ٽǹ��� 8��Ʈ�� ����մϴ�.
		AdjustDepthStencilInfo(m_adapterIdx, pD3DPP, 24, 8);

		// ������� ������ ������ �α׷� ����մϴ�.
		PrintD3DPresentParameters(*pD3DPP);
		return S_OK;
	}

	void RXRendererDX9::ApplyWorldMatrix(const D3DXMATRIXA16& matWorld)
	{
		g_DXResult = m_pD3DDevice9->SetTransform(D3DTS_WORLD, &matWorld);
		DXERR_HANDLER(g_DXResult);
	}

	void RXRendererDX9::ApplyViewMatrix(const D3DXMATRIXA16& matView)
	{
		g_DXResult = m_pD3DDevice9->SetTransform(D3DTS_VIEW, &matView);
		DXERR_HANDLER(g_DXResult);
	}

	void RXRendererDX9::ApplyProjectionMatrix(const D3DXMATRIXA16& matProjection)
	{
		g_DXResult = m_pD3DDevice9->SetTransform(D3DTS_PROJECTION, &matProjection);
		DXERR_HANDLER(g_DXResult);
	}

	void RXRendererDX9::CopyWorldMatrix(D3DXMATRIXA16* pMatWorld)
	{
		g_DXResult = m_pD3DDevice9->GetTransform(D3DTS_WORLD, pMatWorld);
		DXERR_HANDLER(g_DXResult);
	}

	void RXRendererDX9::CopyViewMatrix(D3DXMATRIXA16* pMatView)
	{
		g_DXResult = m_pD3DDevice9->GetTransform(D3DTS_VIEW, pMatView);
		DXERR_HANDLER(g_DXResult);
	}

	void RXRendererDX9::CopyProjectionMatrix(D3DXMATRIXA16* pMatProjection)
	{
		g_DXResult = m_pD3DDevice9->GetTransform(D3DTS_PROJECTION, pMatProjection);
		DXERR_HANDLER(g_DXResult);
	}

	void RXRendererDX9::DefaultProjectionMatrix()
	{
		D3DXMATRIXA16 matProjection;
		D3DXMatrixPerspectiveFovLH(&matProjection, D3DX_PI / 4.0f,
			static_cast<FLOAT>(m_pD3DPP->BackBufferWidth / m_pD3DPP->BackBufferHeight),
			1.0f, 1000.0f);
		ApplyProjectionMatrix(matProjection);
	}

	HRESULT RXRendererDX9::DrawPrimitive(D3DPRIMITIVETYPE primitiveType,
		const RXVertexBufferDX9& vertexBuffer)
	{
		m_pD3DDevice9->SetFVF(VertexInfo::FORMAT);
		m_pD3DDevice9->SetStreamSource(
			0,                    // ��Ʈ�� �ѹ��� 0���� �����մϴ�.
			vertexBuffer.getVB(), // ���� ���۸� �������ݴϴ�.
			0,                    // �������� 0���� �����մϴ�.
			sizeof(VertexInfo));  // ����(Stride)�� FVF�� ������ ũ��� ��ġ�ؾ� �մϴ�.

		g_DXResult = m_pD3DDevice9->DrawPrimitive(
			primitiveType, // ������ ������ �����մϴ�.
			0,             // �������� 0���� �����մϴ�.
			// ������Ƽ�� �����Դϴ�.
			CalcPrimitiveCount(primitiveType, vertexBuffer.getVertexCount()));
		DXERR_HANDLER(g_DXResult);

		++m_drawCallCnt;
		return S_OK;
	}

	HRESULT RXRendererDX9::DrawIdxedPrimitive(const RXVertexBufferDX9& vertexBuffer,
		const RXIndexBufferDX9& IndexBuffer)
	{
		m_pD3DDevice9->SetFVF(VertexInfo::FORMAT);
		m_pD3DDevice9->SetStreamSource(
			0,                    // ��Ʈ�� �ѹ��� 0���� �����մϴ�.
			vertexBuffer.getVB(), // ���� ���۸� �������ݴϴ�.
			0,                    // �������� 0���� �����մϴ�.
			sizeof(VertexInfo));  // ����(Stride)�� FVF�� ������ ũ��� ��ġ�ؾ� �մϴ�.

		// �ε��� ���۸� ���� ����̽��� �������ݴϴ�.
		m_pD3DDevice9->SetIndices(IndexBuffer.getIB());

		g_DXResult = m_pD3DDevice9->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST, // ������ ������ �����մϴ�.
			0,                  // ���� ���ۿ��� ������ ���� �ε����� �����մϴ�. (0���� ����)
			0,                  // ������ �ε����� �����մϴ�. (0���� ����)
			vertexBuffer.getVertexCount(), // ���� ������ �����մϴ�.
			0,                  // �ε��� ������ �������Դϴ�. (0���� ����)
			IndexBuffer.getTriangleCount()); // ������Ƽ�� �����Դϴ�.
		DXERR_HANDLER(g_DXResult);

		// �ε��� ������ ������Ƽ�� ������
		// �⺻ ������ �� D3DPT_TRIANGLESTRIP�� ����������
		// �׷��� ���� ��찡 �� �����Ƿ� ������ �����ؾ� �մϴ�.
		
		++m_drawCallCnt;
		return S_OK;
	}

	HRESULT RXRendererDX9::DrawDXMesh(const LPD3DXMESH pMesh)
	{
		g_DXResult = pMesh->DrawSubset(0);
		DXERR_HANDLER(g_DXResult);
		++m_drawCallCnt;
		return S_OK;
	}

	HRESULT RXRendererDX9::OnLostDevice()
	{
		return S_OK;
	}

	HRESULT RXRendererDX9::OnResetDevice()
	{
		// ������ �ʿ� ���� ������ �����մϴ�.
		ResetD3DPP();

		// ������ �ʿ��� ������ �����մϴ�.
		VerifyDevice(m_pD3DPP);

		if (m_pD3DPP->Windowed == FALSE)
		{
			m_pD3DPP->BackBufferCount = 0;
		}

		// ���� ����̽��� �������ְ� ������ �ٽ� �������ݴϴ�.
		// ���� ���� ����̽��� ���� �Ǵ� ������� ���α׷� â��
		// ������ �ֻ����� ���� ������ �ִµ�... Ȯ�� ���Դϴ�.
		m_pD3DDevice9->Reset(m_pD3DPP);
		RXRendererDX9::Instance()->DefaultRenderState();

		// ��������� �ٽ� �ʱ�ȭ���ݴϴ�.
		RXRendererDX9::Instance()->DefaultProjectionMatrix();
		return S_OK;
	}

	HRESULT RXRendererDX9::OnRecreateDevice()
	{
		Release();
		CreateDevice();
		return S_OK;
	}

	HRESULT RXRendererDX9::Present()
	{
		m_pD3DDevice9->Present(nullptr, nullptr, nullptr, nullptr);
		return S_OK;
	}

	HRESULT RXRendererDX9::Release()
	{
		SAFE_DELTE(m_pD3DPP);

		if (m_pD3DDevice9 != nullptr)
		{
			int refCnt = m_pD3DDevice9->Release();

			if (refCnt > 0)
			{
				RXLOG("DirectX9 ���� ����̽� ���� �����߽��ϴ�! ���۷��� ī��Ʈ(%d)", refCnt);
			}
			else
			{
				RXLOG("DirectX9 ���� ����̽� ���� �����߽��ϴ�!");
			}

			m_pD3DDevice9 = nullptr;
		}

		SAFE_RELEASE(m_pD3D9);
		return S_OK;
	}

} // namespace RX end
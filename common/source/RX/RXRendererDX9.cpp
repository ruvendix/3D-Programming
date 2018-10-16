/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-10-15
 *
 * <파일 내용>
 * DirectX9 렌더러입니다.
 * 가상 디바이스를 포함해서 렌더링의 핵심입니다.
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
		RXLOG(false, "D3DPOOL_DEFAULT 리소스가 정리되었습니다.");
	}

	HRESULT RXRendererDX9::CreateDevice()
	{
		Init();

		// DirectX9 객체를 생성합니다.
		// 인터페이스 포인터에 인스턴화된 객체를 반환해줍니다.
		m_pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
		NULLCHK_RETURN_EFAIL(m_pD3D9, "DirectX9 객체 생성 실패했습니다!");
		RXLOG(false, "DirectX9 객체 생성 성공했습니다!");
		g_pD3D9 = m_pD3D9;
		
		// ====================================================================================
		// 가상 디바이스 생성을 위한 정보를 설정해줍니다.
		// 일반적으로 Present Parameters를 줄여서 PP라고 선언합니다.
		D3DPRESENT_PARAMETERS D3DPP;
		::ZeroMemory(&D3DPP, sizeof(D3DPP));

		// ====================================================================================
		// 검증이 필요 없는 정보부터 채워줍니다.
		//
		// 페이지 플리핑을 할 때의 효과를 설정합니다.
		// 초반에는 D3DSWAPEFFECT_DISCARD가 가장 무난합니다.
		D3DPP.SwapEffect = D3DSWAPEFFECT_DISCARD;

		// 특수한 플래그를 설정합니다.
		// 딱히 사용할 플래그가 없으니 0으로 설정합니다.
		D3DPP.Flags = 0;

		// 프로그램 창 정보를 설정합니다.
		// 프로그램 창 핸들과 창 모드 여부를 설정해주면 됩니다.
		// 나중에 전체 화면도 설정하게 되지만 초반에는 창 모드만 사용합니다.
		D3DPP.hDeviceWindow = g_pMainDX9->getMainWindowHandle();
		D3DPP.Windowed      = (g_pMainDX9->IsFullScreen() == false);
		
		// ====================================================================================
		// 가상 디바이스 생성이 가능한지 검증합니다.
		VerifyDevice(&D3DPP);

		// ====================================================================================
		// 가상 디바이스 생성을 위한 정보를 설정했으므로 가상 디바이스를 생성해줍니다.
		g_DXResult = m_pD3D9->CreateDevice(
			D3DADAPTER_DEFAULT,     // 어댑터를 뜻하는데 모니터 개수라고 생각하면 됩니다.
			D3DDEVTYPE_HAL,         // HAL Device를 사용하겠다는 것입니다.
			D3DPP.hDeviceWindow,    // 가상 디바이스의 타겟 프로그램 창을 의미합니다.
			m_dwBehavior,           // 정점 처리를 그래픽 카드에게 맡긴다는 뜻입니다.
			&D3DPP,                 // 가상 디바이스 생성을 위한 정보를 넘겨줍니다.
			&m_pD3DDevice9);        // 가상 디바이스의 객체 포인터를 받을 인터페이스 포인터입니다.
	
		DXERR_HANDLER(g_DXResult);
		NULLCHK_RETURN_EFAIL(m_pD3DDevice9, "DirectX9 가상 디바이스 생성 실패했습니다!");
		g_pD3DDevice9 = m_pD3DDevice9;

		RXLOG(false, "DirectX9 가상 디바이스 생성 성공했습니다!");
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

	// 가상 디바이스를 생성하기 전에 검증해주는 함수입니다.
	// 검증하는 순서는 다음과 같습니다.
	//
	// 1. 그래픽 카드의 출력 능력 검증(모니터 호환)
	// 2. 하드웨어 가속이 가능한지 검증
	// 3. MSAA 검증
	// 4. 정점 처리 방식 검증(셰이더)
	HRESULT RXRendererDX9::VerifyDevice(D3DPRESENT_PARAMETERS* pD3DPP)
	{
		// ====================================================================================
		// 그래픽 카드의 출력 능력을 검증합니다.
		D3DDISPLAYMODE mainDisplayMode;
		::ZeroMemory(&mainDisplayMode, sizeof(mainDisplayMode));

		// 그래픽 카드의 출력 정보를 가져옵니다.
		if (FAILED(m_pD3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mainDisplayMode)))
		{
			RXERRLOG_RETURN_EFAIL("그래픽 카드 정보 획득 실패했습니다!");
		}

		// 백버퍼의 개수를 설정합니다.
		// 개수에는 0을 넣어도 1로 인식되지만 가독성을 위해 1로 설정합니다.
		pD3DPP->BackBufferCount = 1;

		// 백버퍼 정보를 설정합니다.
		// 순서대로 가로, 세로입니다.
		// 전체 화면일 때는 디스플레이 모드 정보로 설정해줍니다.
		if (pD3DPP->Windowed == TRUE)
		{
			pD3DPP->BackBufferWidth  = g_pMainDX9->getClientWidth();
			pD3DPP->BackBufferHeight = g_pMainDX9->getClientHeight();
		}

		// 백버퍼의 형식을 설정해줍니다.
		pD3DPP->BackBufferFormat = mainDisplayMode.Format;

		// 백버퍼의 주사율을 설정합니다.
		// 창 모드를 사용할 때는 D3DPRESENT_RATE_DEFAULT로 설정하면 됩니다.
		if (pD3DPP->Windowed == TRUE)
		{
			pD3DPP->FullScreen_RefreshRateInHz = D3DPRESENT_INTERVAL_DEFAULT;
		}

		// 백버퍼의 페이지 플리핑 간격을 설정합니다.
		pD3DPP->PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT; // 기본은 수직동기

		// 전체 화면으로 전환될 때의 처리입니다.
		if ( (pD3DPP->Windowed == FALSE) &&
		 	 (AdjustFullScreenInfo(pD3DPP, mainDisplayMode) == false) )
		{
			RXERRLOG_RETURN_EFAIL("전체 화면에서 그래픽 카드가 해상도 또는 형식을 지원하지 않습니다!");
		}

		// ====================================================================================
		// 하드웨어 가속이 가능한지 검증합니다.
		if (FAILED(m_pD3D9->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
			pD3DPP->BackBufferFormat, pD3DPP->BackBufferFormat, pD3DPP->Windowed)))
		{
			RXERRLOG_RETURN_EFAIL("하드웨어 가속을 지원하지 않습니다!");
		}

		// ====================================================================================
		// MSAA(Multisample AntiAliasing)을 검증합니다.
		// AA는 여러 기법이 있지만 가장 기본적인 건 MSAA입니다.
		if (m_bMSAA == true)
		{
			if (AdjustMSAAInfo(pD3DPP) == false)
			{
				RXERRLOG_RETURN_EFAIL("MSAA를 지원하지 않으므로 0으로 설정합니다!");
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

		// ====================================================================================
		// 깊이 스텐실버퍼 형식을 검증합니다.
		// 일반적으로는 깊이버퍼 24비트에 스텐실버퍼 8비트를 사용합니다.
		AdjustDepthStencilInfo(pD3DPP, 24, 8);

		return S_OK;
	}

	HRESULT RXRendererDX9::DrawPrimitive(D3DPRIMITIVETYPE primitiveType,
		const RXVertexBufferDX9& vertexBuffer)
	{
		g_pD3DDevice9->SetFVF(VertexInfo::FORMAT);
		g_pD3DDevice9->SetStreamSource(
			0,                    // 스트림 넘버는 0으로 설정합니다.
			vertexBuffer.getVB(), // 정점 버퍼를 설정해줍니다.
			0,                    // 오프셋은 0으로 설정합니다.
			sizeof(VertexInfo));  // 보폭(Stride)은 FVF로 생성한 크기와 일치해야 합니다.

		g_pD3DDevice9->DrawPrimitive(
			primitiveType, // 렌더링 형식을 설정합니다.
			0,             // 오프셋은 0으로 설정합니다.
			// 프리미티브 개수입니다.
			CalcPrimitiveCount(primitiveType, vertexBuffer.getVertexCount()));

		return S_OK;
	}

	HRESULT RXRendererDX9::DrawIndexedPrimitive(const RXVertexBufferDX9& vertexBuffer,
		const RXIndexBufferDX9& indexBuffer)
	{
		g_pD3DDevice9->SetFVF(VertexInfo::FORMAT);
		g_pD3DDevice9->SetStreamSource(
			0,                    // 스트림 넘버는 0으로 설정합니다.
			vertexBuffer.getVB(), // 정점 버퍼를 설정해줍니다.
			0,                    // 오프셋은 0으로 설정합니다.
			sizeof(VertexInfo));  // 보폭(Stride)은 FVF로 생성한 크기와 일치해야 합니다.

		// 인덱스 버퍼를 가상 디바이스에 적용해줍니다.
		g_pD3DDevice9->SetIndices(indexBuffer.getIB());

		g_pD3DDevice9->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST, // 렌더링 형식을 설정합니다.
			0,                  // 정점 버퍼에서 시작할 정점 인덱스를 설정합니다. (0으로 설정)
			0,                  // 시작할 인덱스를 설정합니다. (0으로 설정)
			vertexBuffer.getVertexCount(), // 정점 개수를 설정합니다.
			0,                  // 인덱스 버퍼의 오프셋입니다. (0으로 설정)
			indexBuffer.getTriangleCount()); // 프리미티브 개수입니다.

		// 인덱스 버퍼의 프리미티브 개수는
		// 기본 설정일 때 D3DPT_TRIANGLESTRIP와 동일하지만
		// 그렇지 않은 경우가 더 많으므로 개수에 주의해야 합니다.

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

		// 검증이 필요 없는 정보를 설정합니다.
		D3DPP.EnableAutoDepthStencil = true;
		D3DPP.AutoDepthStencilFormat = D3DFMT_D24S8;
		D3DPP.SwapEffect             = D3DSWAPEFFECT_DISCARD;
		D3DPP.Flags                  = 0;
		D3DPP.hDeviceWindow          = g_pMainDX9->getMainWindowHandle();
		D3DPP.Windowed               = (g_pMainDX9->IsFullScreen() == false);

		// 검증이 필요한 정보를 설정합니다.
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
				RXLOG(false, "DirectX9 가상 디바이스 해제 실패했습니다! 레퍼런스 카운트(%d)", refCnt);
			}
			else
			{
				RXLOG(false, "DirectX9 가상 디바이스 해제 성공했습니다!");
			}

			m_pD3DDevice9 = nullptr;
		}

		SAFE_RELEASE(m_pD3D9);
		return S_OK;
	}

} // namespace RX end
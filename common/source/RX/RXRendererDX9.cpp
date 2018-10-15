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
		D3DPP.hDeviceWindow = g_pMainDX9->getMainWindowHandle();
		D3DPP.Windowed      = (g_pMainDX9->IsFullScreen() == false);
		
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
		NULLCHK_EFAIL_RETURN(m_pD3DDevice9, "DirectX9 가상 디바이스 생성 실패!");
		g_pD3DDevice9 = m_pD3DDevice9;

		RXLOG(false, "DirectX9 가상 디바이스 생성 성공!");
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
			RXERRLOG_EFAIL_RETURN("그래픽 카드의 출력 정보 획득 실패!");
		}

		// 후면 버퍼의 개수를 설정합니다.
		// 개수에는 0을 넣어도 1로 인식되지만 가독성을 위해 1로 설정합니다.
		pD3DPP->BackBufferCount = 1;

		// 후면 버퍼 정보를 설정합니다.
		// 순서대로 가로, 세로입니다.
		// 전체 화면일 때는 디스플레이 모드 정보로 설정해줍니다.
		bool bFullScreen = g_pMainDX9->IsFullScreen();
		if (bFullScreen == false)
		{
			pD3DPP->BackBufferWidth  = g_pMainDX9->getClientWidth();
			pD3DPP->BackBufferHeight = g_pMainDX9->getClientHeight();
		}

		// 후면 버퍼의 형식을 설정해줍니다.
		pD3DPP->BackBufferFormat = mainDisplayMode.Format;

		// 후면 버퍼의 주사율을 설정합니다.
		// 창 모드를 사용할 때는 D3DPRESENT_RATE_DEFAULT로 설정하면 됩니다.
		if (bFullScreen == false)
		{
			pD3DPP->FullScreen_RefreshRateInHz = D3DPRESENT_INTERVAL_DEFAULT;
		}

		// 후면 버퍼의 페이지 플리핑 간격을 설정합니다.
		pD3DPP->PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT; // 기본은 수직동기

		// 전체 화면으로 전환될 때의 처리입니다.
		if (bFullScreen)
		{
			bool bResult = false;

			// 전체 화면으로 전환될 때 사용 가능한 모드 개수를 가져옵니다.
			// 모드란 그래픽 카드가 지원하는 해상도를 말합니다.
			// 예를 들면 현재 모니터 해상도는 1920 X 1080이지만
			// 그래픽 카드는 800 X 600도 지원합니다. 이런 게 모드입니다.
			INT32 adapterCount =
				m_pD3D9->GetAdapterModeCount(D3DADAPTER_DEFAULT, mainDisplayMode.Format);

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
			pD3DPP->BackBufferFormat, pD3DPP->BackBufferFormat, !bFullScreen)))
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
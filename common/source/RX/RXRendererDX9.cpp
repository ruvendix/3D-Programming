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
		// 가상 디바이스의 기본 정보를 설정합니다.
		// 일반적으로 Present Parameters를 줄여서 PP라고 선언합니다.
		SAFE_DELTE(m_pD3DPP);
		m_pD3DPP = RXNew D3DPRESENT_PARAMETERS;
		NULLCHK_HEAPALLOC(m_pD3DPP);
		::ZeroMemory(m_pD3DPP, sizeof(D3DPRESENT_PARAMETERS));

		// 페이지 플리핑을 할 때의 효과를 설정합니다.
		// 초반에는 D3DSWAPEFFECT_DISCARD가 가장 무난합니다.
		m_pD3DPP->SwapEffect = D3DSWAPEFFECT_DISCARD;
		
		// 특수한 플래그를 설정합니다.
		// 딱히 사용할 플래그가 없으니 0으로 설정합니다.
		m_pD3DPP->Flags = D3DFLAG_NONE;

		// 프로그램 창 정보를 설정합니다.
		// 프로그램 창 핸들과 창 화면 여부를 설정해주면 됩니다.
		// 나중에 전체 화면도 설정하게 되지만 초반에는 창 화면만 사용합니다.
		m_pD3DPP->hDeviceWindow = g_pMainDX9->getMainWindowHandle();
		m_pD3DPP->Windowed      = (g_pMainDX9->IsFullScreen() == false);
	}

	void RXRendererDX9::ArrangeVideoMemory()
	{
		g_DXResult = m_pD3DDevice9->EvictManagedResources();
		DXERR_HANDLER(g_DXResult);
		RXLOG("D3DPOOL_DEFAULT 리소스가 정리되었습니다.");
	}

	void RXRendererDX9::DefaultRenderState()
	{
		// rhw를 사용하지 않는다면 변환 이전의 공간좌표를 사용하게 되므로
		// 각종 변환 과정을 거쳐야 합니다. 조명(라이팅, Lighting)도 그중 하나인데
		// 조명에 관한 연산을 따로 하지 않았으므로 조명은 꺼줘야 합니다.
		m_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, false);

		// 채우기 설정입니다.
		//m_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
		//m_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		m_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

		// 컬링 설정입니다.
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
		// 스크린샷을 저장할 폴더가 없으면 생성해줍니다.
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
			RXERRLOG("스크린샷으로 사용할 수 없는 이미지 형식입니다!",
				ConvertD3DXIMAGE_FILEFORMATToString(fileFormat));
			return;
		}
		}

		// 스크린샷을 찍는 방법은 3가지 정도 되는데 참고 자료를 남깁니다.
		// 방법에 따른 속도 : https://stackoverflow.com/questions/23118686/execution-time-getfrontbufferdata-getbackbuffer-getrendertargetdata#
		// 참고 코드 : https://gist.github.com/vinjn/2922506

		// 먼저 현재 렌더 타겟의 서페이스를 가져옵니다.
		// 렌더 타겟이란 가상 디바이스가 렌더링할 대상을 말하는데
		// 일반적으로 렌더 타겟이라 하면 렌더 타겟의 서페이스를 의미합니다.
		// 하나의 렌더 타겟으로 렌더링하는 방식을 SPR(Single-Path Rendering)이라 하고,
		// 여러 개의 렌더 타겟으로 렌더링하는 방식은 MPR(Multi-Path Rendering)이라고 합니다.
		IDirect3DSurface9* pBackBuffer = nullptr;
		m_pD3DDevice9->GetRenderTarget(0, &pBackBuffer); // 0이면 백버퍼입니다.
		NULLCHK_RETURN_NOCOMENT(pBackBuffer);

		// 복사한 서페이스를 저장할 서페이스를 하나 생성해줍니다.
		IDirect3DSurface9* pDestSurface = nullptr;
		m_pD3DDevice9->CreateOffscreenPlainSurface(m_pD3DPP->BackBufferWidth,
			m_pD3DPP->BackBufferHeight, m_pD3DPP->BackBufferFormat,
			D3DPOOL_SYSTEMMEM, &pDestSurface, nullptr);
		NULLCHK_RETURN_NOCOMENT(pDestSurface);

		// 백버퍼를 생성한 서페이스에 복사합니다.
		g_DXResult = m_pD3DDevice9->GetRenderTargetData(pBackBuffer, pDestSurface);
		DXERR_HANDLER(g_DXResult);

		// 생성한 서페이스를 파일로 저장합니다.
		g_DXResult = D3DXSaveSurfaceToFileA(szFull, fileFormat, pDestSurface, NULL, NULL);
		DXERR_HANDLER(g_DXResult);

		// 서페이스를 정리합니다.
		pBackBuffer->Release();
		pDestSurface->Release();

		RXMSGBOXLOG("스크린샷이 저장되었습니다.");
	}

	HRESULT RXRendererDX9::CreateDevice()
	{
		Init();

		// DirectX9 객체를 생성합니다.
		// 인터페이스 포인터에 인스턴화된 객체를 반환해줍니다.
		m_pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
		NULLCHK_RETURN_EFAIL(m_pD3D9, "DirectX9 객체 생성 실패했습니다!");
		RXLOG("DirectX9 객체 생성 성공했습니다!");
		g_pD3D9 = m_pD3D9;
		
		// ====================================================================================
		// 가상 디바이스 생성을 위한 정보를 설정해줍니다.
		ResetD3DPP();
		
		// ====================================================================================
		// 가상 디바이스 생성이 가능한지 검증합니다.
		VerifyDevice(m_pD3DPP);

		// ====================================================================================
		// 가상 디바이스 생성을 위한 정보를 설정했으므로 가상 디바이스를 생성해줍니다.
		g_DXResult = m_pD3D9->CreateDevice(
			m_adapterIdx,            // 어댑터를 뜻하는데 모니터 개수라고 생각하면 됩니다.
			D3DDEVTYPE_HAL,          // HAL Device를 사용하겠다는 것입니다.
			m_pD3DPP->hDeviceWindow, // 가상 디바이스의 타겟 프로그램 창을 의미합니다.
			m_dwBehavior,            // 정점 처리를 그래픽 카드에게 맡긴다는 뜻입니다.
			m_pD3DPP,                // 가상 디바이스 생성을 위한 정보를 넘겨줍니다.
			&m_pD3DDevice9);         // 가상 디바이스의 객체 포인터를 받을 인터페이스 포인터입니다.
	
		DXERR_HANDLER(g_DXResult);
		NULLCHK_RETURN_EFAIL(m_pD3DDevice9, "DirectX9 가상 디바이스 생성 실패했습니다!");
		g_pD3DDevice9 = m_pD3DDevice9;

		RXLOG("DirectX9 가상 디바이스 생성 성공했습니다!");
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

	// 가상 디바이스를 생성하기 전에 검증해주는 함수입니다.
	// 검증하는 순서는 다음과 같습니다.
	//
	// 1. 그래픽 카드의 출력 능력 검증(모니터 호환)
	// 2. 하드웨어 가속이 가능한지 검증
	// 3. MSAA 검증
	// 4. 정점 처리 방식 검증(셰이더)
	HRESULT RXRendererDX9::VerifyDevice(D3DPRESENT_PARAMETERS* pD3DPP)
	{
		NULLCHK_RETURN_EFAIL_NOCOMENT(pD3DPP);

		// ====================================================================================
		// 그래픽 카드의 출력 능력을 검증합니다.
		D3DDISPLAYMODE mainDisplayMode;
		::ZeroMemory(&mainDisplayMode, sizeof(mainDisplayMode));

		// 어댑터 인덱스를 찾습니다.
		m_adapterIdx = FindAdapterIndex(pD3DPP->hDeviceWindow);
		RXLOG("현재 어댑터 인덱스 : %d", m_adapterIdx);

		// 그래픽 카드의 출력 정보를 가져옵니다.
		if (FAILED(m_pD3D9->GetAdapterDisplayMode(m_adapterIdx, &mainDisplayMode)))
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
		// 창 화면을 사용할 때는 D3DPRESENT_RATE_DEFAULT로 설정하면 됩니다.
		if (pD3DPP->Windowed == TRUE)
		{
			pD3DPP->FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		}

		// 백버퍼의 페이지 플리핑 간격을 설정합니다.
		pD3DPP->PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT; // 기본은 수직동기

		if (pD3DPP->PresentationInterval == D3DPRESENT_INTERVAL_DEFAULT)
		{
			m_bVSync = true;
		}

		// 전체 화면으로 전환될 때의 처리입니다.
		if ( (pD3DPP->Windowed == FALSE) &&
			 (AdjustFullScreenInfo(m_adapterIdx, pD3DPP, mainDisplayMode) == false) )
		{
			RXERRLOG_RETURN_EFAIL("전체 화면에서 그래픽 카드가 해상도 또는 형식을 지원하지 않습니다!");
		}

		// ====================================================================================
		// 하드웨어 가속이 가능한지 검증합니다.
		if (FAILED(m_pD3D9->CheckDeviceType(m_adapterIdx, D3DDEVTYPE_HAL,
			pD3DPP->BackBufferFormat, pD3DPP->BackBufferFormat, pD3DPP->Windowed)))
		{
			RXERRLOG_RETURN_EFAIL("하드웨어 가속을 지원하지 않습니다!");
		}

		// ====================================================================================
		// MSAA(Multisample AntiAliasing)을 검증합니다.
		// AA는 여러 기법이 있지만 가장 기본적인 건 MSAA입니다.
		if (m_bMSAA == true)
		{
			if (AdjustMSAAInfo(m_adapterIdx, pD3DPP) == false)
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
		m_pD3D9->GetDeviceCaps(m_adapterIdx, D3DDEVTYPE_HAL, &D3DCaps9);

		// 그래픽 카드가 TL(Transformation And Lighting)을 지원하거나
		// 정점 셰이더와 픽셀 셰이더가 1.0 이상이라면
		// 정점 처리 방식을 소프트웨어 정점 처리로 설정합니다.
		if ( (D3DCaps9.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) ||
			 ( (D3DCaps9.VertexShaderVersion > D3DVS_VERSION(1, 0)) &&
			   (D3DCaps9.PixelShaderVersion > D3DPS_VERSION(1, 0))) )
		{
			// D3DPS_VERSION()은 (0xFFFF0000|((_Major)<<8)|(_Minor))인데
			// _Major를 1바이트만큼 왼쪽으로 밀고 _Minor와 OR 연산합니다.
			// 그 다음 0xFFFF0000과 OR 연산하는데 하위 2바이트가 핵심입니다.
			// 상위 2바이트는 1로 만드는데 실제 버전 확인은 하위 2바이트입니다.
			// D3DPS_VERSION(1, 1)은 0xFFFF0101이 됩니다. 즉, 픽셀 셰이더 1.1을 의미합니다.
			//
			// 다른 방식으로 알아내려면 PixelShaderVersion >> 8을 해서 _Minor를 날려버리고
			// 0xFF와 AND 연산해서 최하위 바이트만 뽑아냅니다.
			// _Minor는 PixelShaderVersion & 0xFF해서 최하위 바이트만 뽑아냅니다.
			// 즉, _Major와 _Minor를 분리하는 방법입니다.
			m_dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		}
		else
		{
			// 소프트웨어 정점 처리를 사용해야 한다면 메시지 박스로
			// 현재 그래픽 카드에 대한 정보를 간단하게 알려줍니다.
			D3DADAPTER_IDENTIFIER9 adapterInfo;
			::ZeroMemory(&adapterInfo, sizeof(adapterInfo));
			g_pD3D9->GetAdapterIdentifier(m_adapterIdx, D3DENUM_WHQL_LEVEL, &adapterInfo);
			RXERRLOG_CHAR("하드웨어 정점 처리를 지원하지 않습니다!\n그래픽 카드 (%s)",
				adapterInfo.Description);

			// 소프트웨어 정점 처리로 설정합니다.
			m_dwBehavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		}

		// ====================================================================================
		// 깊이 스텐실버퍼 형식을 검증합니다.
		// 일반적으로는 깊이버퍼 24비트에 스텐실버퍼 8비트를 사용합니다.
		AdjustDepthStencilInfo(m_adapterIdx, pD3DPP, 24, 8);

		// 현재까지 설정된 정보를 로그로 출력합니다.
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
			0,                    // 스트림 넘버는 0으로 설정합니다.
			vertexBuffer.getVB(), // 정점 버퍼를 설정해줍니다.
			0,                    // 오프셋은 0으로 설정합니다.
			sizeof(VertexInfo));  // 보폭(Stride)은 FVF로 생성한 크기와 일치해야 합니다.

		g_DXResult = m_pD3DDevice9->DrawPrimitive(
			primitiveType, // 렌더링 형식을 설정합니다.
			0,             // 오프셋은 0으로 설정합니다.
			// 프리미티브 개수입니다.
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
			0,                    // 스트림 넘버는 0으로 설정합니다.
			vertexBuffer.getVB(), // 정점 버퍼를 설정해줍니다.
			0,                    // 오프셋은 0으로 설정합니다.
			sizeof(VertexInfo));  // 보폭(Stride)은 FVF로 생성한 크기와 일치해야 합니다.

		// 인덱스 버퍼를 가상 디바이스에 적용해줍니다.
		m_pD3DDevice9->SetIndices(IndexBuffer.getIB());

		g_DXResult = m_pD3DDevice9->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST, // 렌더링 형식을 설정합니다.
			0,                  // 정점 버퍼에서 시작할 정점 인덱스를 설정합니다. (0으로 설정)
			0,                  // 시작할 인덱스를 설정합니다. (0으로 설정)
			vertexBuffer.getVertexCount(), // 정점 개수를 설정합니다.
			0,                  // 인덱스 버퍼의 오프셋입니다. (0으로 설정)
			IndexBuffer.getTriangleCount()); // 프리미티브 개수입니다.
		DXERR_HANDLER(g_DXResult);

		// 인덱스 버퍼의 프리미티브 개수는
		// 기본 설정일 때 D3DPT_TRIANGLESTRIP와 동일하지만
		// 그렇지 않은 경우가 더 많으므로 개수에 주의해야 합니다.
		
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
		// 검증이 필요 없는 정보를 설정합니다.
		ResetD3DPP();

		// 검증이 필요한 정보를 설정합니다.
		VerifyDevice(m_pD3DPP);

		if (m_pD3DPP->Windowed == FALSE)
		{
			m_pD3DPP->BackBufferCount = 0;
		}

		// 가상 디바이스를 리셋해주고 정보를 다시 설정해줍니다.
		// 현재 가상 디바이스를 리셋 또는 재생성시 프로그램 창이
		// 무조건 최상위로 오는 현상이 있는데... 확인 중입니다.
		m_pD3DDevice9->Reset(m_pD3DPP);
		RXRendererDX9::Instance()->DefaultRenderState();

		// 투영행렬을 다시 초기화해줍니다.
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
				RXLOG("DirectX9 가상 디바이스 해제 실패했습니다! 레퍼런스 카운트(%d)", refCnt);
			}
			else
			{
				RXLOG("DirectX9 가상 디바이스 해제 성공했습니다!");
			}

			m_pD3DDevice9 = nullptr;
		}

		SAFE_RELEASE(m_pD3D9);
		return S_OK;
	}

} // namespace RX end
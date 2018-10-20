/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-10-16
 *
 * <파일 내용>
 * DirectX9 렌더러의 보조 함수입니다.
 *
 ====================================================================================*/
#include "PCH.h"
#include "RXRendererDX9_Util.h"

// ==================================================================================
// 보조 매크로 함수입니다.
#define CASE_D3DFORMAT(D3DFormat)\
case D3DFormat:\
{\
	szD3DFormat = _T(#D3DFormat);\
	break;\
}
// ==================================================================================

namespace RX
{

	INT32 CalcPrimitiveCount(D3DPRIMITIVETYPE primitiveType, INT32 vertexCnt)
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

	bool AdjustFullScreenInfo(INT32 adapterIndex, D3DPRESENT_PARAMETERS* pD3DPP,
		const D3DDISPLAYMODE& mainDisplayMode)
	{
		NULLCHK_RETURN_FALSE(pD3DPP);

		// 전체 화면으로 전환될 때 사용 가능한 모드 개수를 가져옵니다.
		// 모드란 그래픽 카드가 지원하는 해상도를 말합니다.
		// 예를 들면 현재 모니터 해상도는 1920 X 1080이지만
		// 그래픽 카드는 800 X 600도 지원합니다. 이런 게 모드입니다.
		INT32 adapterCount =
			g_pD3D9->GetAdapterModeCount(adapterIndex, mainDisplayMode.Format);

		for (INT32 i = 0; i < adapterCount; ++i)
		{
			D3DDISPLAYMODE subDisplayMode;
			::ZeroMemory(&subDisplayMode, sizeof(subDisplayMode));

			// 원하는 해상도 또는 형식과 호환이 되는지 검증합니다.
			g_DXResult = g_pD3D9->EnumAdapterModes(adapterIndex,
				pD3DPP->BackBufferFormat, i, &subDisplayMode);
			DXERR_HANDLER(g_DXResult);

			// 현재 해상도와 전체 화면 해상도를 비교합니다.
			if ( (subDisplayMode.Width == mainDisplayMode.Width) &&
				 (subDisplayMode.Height == mainDisplayMode.Height) &&
				 (subDisplayMode.RefreshRate >= mainDisplayMode.RefreshRate) )
			{
				pD3DPP->BackBufferWidth  = subDisplayMode.Width;
				pD3DPP->BackBufferHeight = subDisplayMode.Height;
				pD3DPP->FullScreen_RefreshRateInHz = subDisplayMode.RefreshRate;
				return true;
			}
		}
		return false;
	}

	bool AdjustMSAAInfo(INT32 adapterIndex, D3DPRESENT_PARAMETERS* pD3DPP)
	{
		NULLCHK_RETURN_FALSE(pD3DPP);

		// 2, 4, 8, 16만 검증합니다.
		INT32 MSAATable[4] =
		{
			D3DMULTISAMPLE_16_SAMPLES,
			D3DMULTISAMPLE_8_SAMPLES,
			D3DMULTISAMPLE_4_SAMPLES,
			D3DMULTISAMPLE_2_SAMPLES,
		};

		DWORD dwMSAAQuality = 0;
		for (INT32 i = 0; i < _countof(MSAATable); ++i)
		{
			D3DMULTISAMPLE_TYPE type = static_cast<D3DMULTISAMPLE_TYPE>(MSAATable[i]);

			// HAL을 지원할 때 현재 백버퍼 형식과 모드가 원하는 MSAA를 지원하는지 검증합니다.
			// 결과는 dwMSAAQuality에 저장되는데 이용 가능한 MSAA 퀄리티 레벨 수를 알려줍니다.
			if (SUCCEEDED(g_pD3D9->CheckDeviceMultiSampleType(adapterIndex,
				D3DDEVTYPE_HAL, pD3DPP->BackBufferFormat, pD3DPP->Windowed, type, &dwMSAAQuality)))
			{
				// 검증이 통과되어도 원하는 MSAA와 검증된 MSAA가 다를 수 있습니다.
				// 이럴 때는 검증된 MSAA로 설정합니다. 따라서 검증된 MSAA가 우선입니다.
				if (pD3DPP->MultiSampleType != type)
				{
					RXLOG("원하는 MSAA(%d)와 검증된 MSAA(%d)가 달라서 검증된 MSAA로 대체됩니다.",
						pD3DPP->MultiSampleType, type);
				}

				pD3DPP->MultiSampleType    = type;
				pD3DPP->MultiSampleQuality = dwMSAAQuality - 1;
				return true;
			}
		}

		// 검증된 MSAA가 없다면 MSAA를 적용하지 않습니다.
		pD3DPP->MultiSampleType    = D3DMULTISAMPLE_NONE;
		pD3DPP->MultiSampleQuality = 0;
		return false;
	}

	// CheckDeviceFormat()은 지정한 형식을 리소스 형식으로 이용 가능한지 검증해주는 함수로서,
	// 텍스처, 스텐실버퍼, 렌더 타겟 등을 검증할 때 사용합니다.
	//
	// CheckDepthStencilMatch()은 스텐실버퍼의 형식이 특정 모드의 렌더 타겟 형식과
	// 호환이 가능한지 검증해주는 함수입니다.
	bool AdjustDepthStencilInfo(INT32 adapterIndex, D3DPRESENT_PARAMETERS* pD3DPP,
		INT32 DepthBits, INT32 StencilBits)
	{
		NULLCHK_RETURN_FALSE(pD3DPP);

		if ( (DepthBits <= 16) &&
			 (StencilBits == 0) )
		{
			if (SUCCEEDED(g_pD3D9->CheckDeviceFormat(adapterIndex, D3DDEVTYPE_HAL,
				pD3DPP->BackBufferFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16)))
			{
				if (SUCCEEDED(g_pD3D9->CheckDepthStencilMatch(adapterIndex, D3DDEVTYPE_HAL,
					pD3DPP->BackBufferFormat, pD3DPP->BackBufferFormat, D3DFMT_D16)))
				{
					pD3DPP->EnableAutoDepthStencil = TRUE;
					pD3DPP->AutoDepthStencilFormat = D3DFMT_D16;
					return true;
				}
			}
		}

		if ( (DepthBits <= 15) &&
			 (StencilBits == 1) )
		{
			if (SUCCEEDED(g_pD3D9->CheckDeviceFormat(adapterIndex, D3DDEVTYPE_HAL,
				pD3DPP->BackBufferFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D15S1)))
			{
				if (SUCCEEDED(g_pD3D9->CheckDepthStencilMatch(adapterIndex, D3DDEVTYPE_HAL,
					pD3DPP->BackBufferFormat, pD3DPP->BackBufferFormat, D3DFMT_D15S1)))
				{
					pD3DPP->EnableAutoDepthStencil = TRUE;
					pD3DPP->AutoDepthStencilFormat = D3DFMT_D15S1;
					return true;
				}
			}
		}

		if ( (DepthBits <= 24) &&
			 (StencilBits == 0) )
		{
			if (SUCCEEDED(g_pD3D9->CheckDeviceFormat(adapterIndex, D3DDEVTYPE_HAL,
				pD3DPP->BackBufferFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X8)))
			{
				if (SUCCEEDED(g_pD3D9->CheckDepthStencilMatch(adapterIndex, D3DDEVTYPE_HAL,
					pD3DPP->BackBufferFormat, pD3DPP->BackBufferFormat, D3DFMT_D24X8)))
				{
					pD3DPP->EnableAutoDepthStencil = TRUE;
					pD3DPP->AutoDepthStencilFormat = D3DFMT_D24X8;
					return true;
				}
			}
		}

		if ( (DepthBits <= 24) &&
			 (StencilBits <= 8) )
		{
			if (SUCCEEDED(g_pD3D9->CheckDeviceFormat(adapterIndex, D3DDEVTYPE_HAL,
				pD3DPP->BackBufferFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8)))
			{
				if (SUCCEEDED(g_pD3D9->CheckDepthStencilMatch(adapterIndex, D3DDEVTYPE_HAL,
					pD3DPP->BackBufferFormat, pD3DPP->BackBufferFormat, D3DFMT_D24S8)))
				{
					pD3DPP->EnableAutoDepthStencil = TRUE;
					pD3DPP->AutoDepthStencilFormat = D3DFMT_D24S8;
					return true;
				}
			}
		}

		if ( (DepthBits <= 24) &&
			 (StencilBits <= 4) )
		{
			if (SUCCEEDED(g_pD3D9->CheckDeviceFormat(adapterIndex, D3DDEVTYPE_HAL,
				pD3DPP->BackBufferFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X4S4)))
			{
				if (SUCCEEDED(g_pD3D9->CheckDepthStencilMatch(adapterIndex, D3DDEVTYPE_HAL,
					pD3DPP->BackBufferFormat, pD3DPP->BackBufferFormat, D3DFMT_D24X4S4)))
				{
					pD3DPP->EnableAutoDepthStencil = TRUE;
					pD3DPP->AutoDepthStencilFormat = D3DFMT_D24X4S4;
					return true;
				}
			}
		}

		if ( (DepthBits <= 32) &&
			 (StencilBits == 0) )
		{
			if (SUCCEEDED(g_pD3D9->CheckDeviceFormat(adapterIndex, D3DDEVTYPE_HAL,
				pD3DPP->BackBufferFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D32)))
			{
				if (SUCCEEDED(g_pD3D9->CheckDepthStencilMatch(adapterIndex, D3DDEVTYPE_HAL,
					pD3DPP->BackBufferFormat, pD3DPP->BackBufferFormat, D3DFMT_D32)))
				{
					pD3DPP->EnableAutoDepthStencil = TRUE;
					pD3DPP->AutoDepthStencilFormat = D3DFMT_D32;
					return true;
				}
			}
		}

		pD3DPP->EnableAutoDepthStencil = FALSE;
		pD3DPP->AutoDepthStencilFormat = D3DFMT_UNKNOWN;
		return false;
	}

	// 모든 DXT 형식 이미지를 사용할 수 있는지 확인합니다.
	// DXT1부터 DXT5까지 확인합니다.
	bool CheckAvailableAllDXT(INT32 adapterIndex, D3DFORMAT surfaceFormat)
	{
		D3DFORMAT DXTFormat[5] = { D3DFMT_DXT1, D3DFMT_DXT2, D3DFMT_DXT3, D3DFMT_DXT4, D3DFMT_DXT5 };
		for (INT32 i = 0; i < 5; ++i)
		{
			if (FAILED(g_pD3D9->CheckDeviceFormat(adapterIndex,
				D3DDEVTYPE_HAL, surfaceFormat, D3DUSAGE_NONE, D3DRTYPE_TEXTURE, DXTFormat[i])))
			{
				return false;
			}
		}
		return true;
	}

	const TCHAR* ConvertD3DFORMATToString(D3DFORMAT surfaceFormat)
	{
		const TCHAR* szD3DFormat = nullptr;
		switch (surfaceFormat)
		{
			CASE_D3DFORMAT(D3DFMT_UNKNOWN)
			CASE_D3DFORMAT(D3DFMT_R8G8B8)
			CASE_D3DFORMAT(D3DFMT_A8R8G8B8)
			CASE_D3DFORMAT(D3DFMT_X8R8G8B8)
			CASE_D3DFORMAT(D3DFMT_R5G6B5)
			CASE_D3DFORMAT(D3DFMT_X1R5G5B5)
			CASE_D3DFORMAT(D3DFMT_A1R5G5B5)
			CASE_D3DFORMAT(D3DFMT_A4R4G4B4)
			CASE_D3DFORMAT(D3DFMT_R3G3B2)
			CASE_D3DFORMAT(D3DFMT_A8)
			CASE_D3DFORMAT(D3DFMT_A8R3G3B2)
			CASE_D3DFORMAT(D3DFMT_X4R4G4B4)
			CASE_D3DFORMAT(D3DFMT_A2B10G10R10)
			CASE_D3DFORMAT(D3DFMT_A8B8G8R8)
			CASE_D3DFORMAT(D3DFMT_X8B8G8R8)
			CASE_D3DFORMAT(D3DFMT_G16R16)
			CASE_D3DFORMAT(D3DFMT_A2R10G10B10)
			CASE_D3DFORMAT(D3DFMT_A16B16G16R16)			
			CASE_D3DFORMAT(D3DFMT_A8P8)
			CASE_D3DFORMAT(D3DFMT_P8)			
			CASE_D3DFORMAT(D3DFMT_L8)
			CASE_D3DFORMAT(D3DFMT_A8L8)
			CASE_D3DFORMAT(D3DFMT_A4L4)			
			CASE_D3DFORMAT(D3DFMT_V8U8)
			CASE_D3DFORMAT(D3DFMT_L6V5U5)
			CASE_D3DFORMAT(D3DFMT_X8L8V8U8)
			CASE_D3DFORMAT(D3DFMT_Q8W8V8U8)
			CASE_D3DFORMAT(D3DFMT_V16U16)
			CASE_D3DFORMAT(D3DFMT_A2W10V10U10)			
			CASE_D3DFORMAT(D3DFMT_UYVY)
			CASE_D3DFORMAT(D3DFMT_R8G8_B8G8)
			CASE_D3DFORMAT(D3DFMT_YUY2)
			CASE_D3DFORMAT(D3DFMT_G8R8_G8B8)
			CASE_D3DFORMAT(D3DFMT_DXT1)
			CASE_D3DFORMAT(D3DFMT_DXT2)
			CASE_D3DFORMAT(D3DFMT_DXT3)
			CASE_D3DFORMAT(D3DFMT_DXT4)
			CASE_D3DFORMAT(D3DFMT_DXT5)			
			CASE_D3DFORMAT(D3DFMT_D16_LOCKABLE)
			CASE_D3DFORMAT(D3DFMT_D32)
			CASE_D3DFORMAT(D3DFMT_D15S1)
			CASE_D3DFORMAT(D3DFMT_D24S8)
			CASE_D3DFORMAT(D3DFMT_D24X8)
			CASE_D3DFORMAT(D3DFMT_D24X4S4)
			CASE_D3DFORMAT(D3DFMT_D16)			
			CASE_D3DFORMAT(D3DFMT_D32F_LOCKABLE)
			CASE_D3DFORMAT(D3DFMT_D24FS8)			
			CASE_D3DFORMAT(D3DFMT_D32_LOCKABLE)
			CASE_D3DFORMAT(D3DFMT_S8_LOCKABLE)			
			CASE_D3DFORMAT(D3DFMT_L16)			
			CASE_D3DFORMAT(D3DFMT_VERTEXDATA)
			CASE_D3DFORMAT(D3DFMT_INDEX16)
			CASE_D3DFORMAT(D3DFMT_INDEX32)			
			CASE_D3DFORMAT(D3DFMT_Q16W16V16U16)			
			CASE_D3DFORMAT(D3DFMT_MULTI2_ARGB8)
			CASE_D3DFORMAT(D3DFMT_R16F)
			CASE_D3DFORMAT(D3DFMT_G16R16F)
			CASE_D3DFORMAT(D3DFMT_A16B16G16R16F)
			CASE_D3DFORMAT(D3DFMT_R32F)
			CASE_D3DFORMAT(D3DFMT_G32R32F)
			CASE_D3DFORMAT(D3DFMT_A32B32G32R32F)			
			CASE_D3DFORMAT(D3DFMT_CxV8U8)
			CASE_D3DFORMAT(D3DFMT_A1)
			CASE_D3DFORMAT(D3DFMT_A2B10G10R10_XR_BIAS)
			CASE_D3DFORMAT(D3DFMT_BINARYBUFFER)
		default:
		{
			szD3DFormat = _T("It does not exist format");
			break;
		}
		}
		return szD3DFormat;
	}

	// 어댑터 인덱스는 HMONITOR가 달라져도 그대로 유지될 수 있습니다.
	// 반대로 HMONITOR가 같아도 어댑터 인덱스는 변경될 수 있습니다.
	INT32 FindAdapterIndex(const HWND hWnd)
	{
		const INT32 adapterCnt = static_cast<INT32>(g_pD3D9->GetAdapterCount());
		for (INT32 i = 0; i < adapterCnt; ++i)
		{
			if (RX::FindCurrentMonitorHandle(hWnd) == g_pD3D9->GetAdapterMonitor(i))
			{
				return i;
			}
		}

		return -1;
	}

	void PrintD3DPresentParameters(const D3DPRESENT_PARAMETERS& D3DPP)
	{
		RXLOG("=============================================");
		RXLOG("BackBufferWidth(%d)", D3DPP.BackBufferWidth);
		RXLOG("BackBufferHeight(%d)", D3DPP.BackBufferHeight);
		RXLOG("BackBufferFormat(%s)", ConvertD3DFORMATToString(D3DPP.BackBufferFormat));
		RXLOG("BackBufferCount(%d)", D3DPP.BackBufferCount);
		RXLOG("MultiSampleType(%d)", D3DPP.MultiSampleType);
		RXLOG("MultiSampleQuality(%d)", D3DPP.MultiSampleQuality);
		RXLOG("SwapEffect(%d)", D3DPP.SwapEffect);
		RXLOG("hDeviceWindow(%p)", D3DPP.hDeviceWindow);
		RXLOG("Windowed(%s)", (D3DPP.Windowed) ? _T("창 화면"): _T("전체 화면"));
		RXLOG("EnableAutoDepthStencil(%d)", D3DPP.EnableAutoDepthStencil);
		RXLOG("AutoDepthStencilFormat(%s)", ConvertD3DFORMATToString(D3DPP.AutoDepthStencilFormat));
		RXLOG("Flags(%d)", D3DPP.Flags);
		RXLOG("FullScreen_RefreshRateInHz(%d)", D3DPP.FullScreen_RefreshRateInHz);
		RXLOG("PresentationInterval(%d)", D3DPP.PresentationInterval);
		RXLOG("=============================================");
	}

} // namespace RX end
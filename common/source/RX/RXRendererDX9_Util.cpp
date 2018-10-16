/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <�ۼ� ��¥>
 * 2018-10-16
 *
 * <���� ����>
 * DirectX9 �������� ���� �Լ��Դϴ�.
 *
 ====================================================================================*/
#include "PCH.h"
#include "RXRendererDX9_Util.h"

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

	bool AdjustFullScreenInfo(INT32 adapterIndex, D3DPRESENT_PARAMETERS* pD3DPP, const D3DDISPLAYMODE& mainDisplayMode)
	{
		NULLCHK_RETURN_FALSE(pD3DPP);

		// ��ü ȭ������ ��ȯ�� �� ��� ������ ��� ������ �����ɴϴ�.
		// ���� �׷��� ī�尡 �����ϴ� �ػ󵵸� ���մϴ�.
		// ���� ��� ���� ����� �ػ󵵴� 1920 X 1080������
		// �׷��� ī��� 800 X 600�� �����մϴ�. �̷� �� ����Դϴ�.
		INT32 adapterCount =
			g_pD3D9->GetAdapterModeCount(adapterIndex, mainDisplayMode.Format);

		for (INT32 i = 0; i < adapterCount; ++i)
		{
			D3DDISPLAYMODE subDisplayMode;
			::ZeroMemory(&subDisplayMode, sizeof(subDisplayMode));

			// ���ϴ� �ػ� �Ǵ� ���İ� ȣȯ�� �Ǵ��� �����մϴ�.
			g_DXResult = g_pD3D9->EnumAdapterModes(adapterIndex,
				pD3DPP->BackBufferFormat, i, &subDisplayMode);
			DXERR_HANDLER(g_DXResult);

			// ���� �ػ󵵿� ��ü ȭ�� �ػ󵵸� ���մϴ�.
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

		// 2, 4, 8, 16�� �����մϴ�.
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

			// HAL�� ������ �� ���� ����� ���İ� ��尡 ���ϴ� MSAA�� �����ϴ��� �����մϴ�.
			// ����� dwMSAAQuality�� ����Ǵµ� �̿� ������ MSAA ����Ƽ ���� ���� �˷��ݴϴ�.
			if (SUCCEEDED(g_pD3D9->CheckDeviceMultiSampleType(adapterIndex,
				D3DDEVTYPE_HAL, pD3DPP->BackBufferFormat, pD3DPP->Windowed, type, &dwMSAAQuality)))
			{
				// ������ ����Ǿ ���ϴ� MSAA�� ������ MSAA�� �ٸ� �� �ֽ��ϴ�.
				// �̷� ���� ������ MSAA�� �����մϴ�. ���� ������ MSAA�� �켱�Դϴ�.
				if (pD3DPP->MultiSampleType != type)
				{
					RXLOG(false, "���ϴ� MSAA(%d)�� ������ MSAA(%d)�� �޶� ������ MSAA�� ��ü�˴ϴ�.",
						pD3DPP->MultiSampleType, type);
				}

				pD3DPP->MultiSampleType    = type;
				pD3DPP->MultiSampleQuality = dwMSAAQuality - 1;
				return true;
			}
		}

		// ������ MSAA�� ���ٸ� MSAA�� �������� �ʽ��ϴ�.
		pD3DPP->MultiSampleType    = D3DMULTISAMPLE_NONE;
		pD3DPP->MultiSampleQuality = 0;
		return false;
	}

	// CheckDeviceFormat()�� ������ ������ ���ҽ� �������� �̿� �������� �������ִ� �Լ��μ�,
	// �ؽ�ó, ���ٽǹ���, ���� Ÿ�� ���� ������ �� ����մϴ�.
	//
	// CheckDepthStencilMatch()�� ���ٽǹ����� ������ Ư�� ����� ���� Ÿ�� ���İ�
	// ȣȯ�� �������� �������ִ� �Լ��Դϴ�.
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
			if (SUCCEEDED(g_pD3D9->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
				pD3DPP->BackBufferFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D15S1)))
			{
				if (SUCCEEDED(g_pD3D9->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
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
			if (SUCCEEDED(g_pD3D9->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
				pD3DPP->BackBufferFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X8)))
			{
				if (SUCCEEDED(g_pD3D9->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
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
			if (SUCCEEDED(g_pD3D9->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
				pD3DPP->BackBufferFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8)))
			{
				if (SUCCEEDED(g_pD3D9->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
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
			if (SUCCEEDED(g_pD3D9->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
				pD3DPP->BackBufferFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X4S4)))
			{
				if (SUCCEEDED(g_pD3D9->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
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
			if (SUCCEEDED(g_pD3D9->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
				pD3DPP->BackBufferFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D32)))
			{
				if (SUCCEEDED(g_pD3D9->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
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

	// ��� DXT ���� �̹����� ����� �� �ִ��� Ȯ���մϴ�.
	// DXT1���� DXT5���� Ȯ���մϴ�.
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

} // namespace RX end
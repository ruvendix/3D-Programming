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
#ifndef RXRENDERERDX9_H__
#define RXRENDERERDX9_H__

#include "common.h"

namespace RX
{

	class DLL_DEFINE RXRendererDX9
	{
		PHOENIX_SINGLETON(RXRendererDX9);
	public:
		void Init();

		HRESULT CreateDevice();
		HRESULT BeginRender();
		HRESULT EndRender();
		HRESULT Present();
		HRESULT Release();
		HRESULT VerifyDevice(D3DPRESENT_PARAMETERS* pD3DPP);
		
		// ====================================================================================
		// 로스트 디바이스, 리셋 디바이스
		HRESULT OnLostDevice();
		HRESULT OnResetDevice();

		bool IsLostDevice() const noexcept
		{
			return (m_bLostDevice == true);
		}

		// ====================================================================================
		// Getter
		IDirect3D9* getD3D9() const noexcept
		{
			return m_pD3D9;
		}

		IDirect3DDevice9* getD3DDevice9() const noexcept
		{
			return m_pD3DDevice9;
		}

		// ====================================================================================
		// Setter
		void setClearColor(D3DCOLOR clearColor)
		{
			m_clearColor = clearColor;
		}

		void setLostDevice(bool bLostDevice)
		{
			m_bLostDevice = bLostDevice;
		}

	private:
		bool                 m_bLostDevice;
		bool                 m_bMSAA;      // 멀티 샘플링(안티 얼라이징) 사용 여부입니다.
		DWORD                m_dwBehavior; // 정점 처리 방식입니다.
		IDirect3D9*          m_pD3D9;
		IDirect3DDevice9*    m_pD3DDevice9;
		D3DCOLOR             m_clearColor;
	};

} // namespace RX end
#endif
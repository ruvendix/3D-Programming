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
		// �ν�Ʈ ����̽�, ���� ����̽�
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
		bool                 m_bMSAA;      // ��Ƽ ���ø�(��Ƽ �����¡) ��� �����Դϴ�.
		DWORD                m_dwBehavior; // ���� ó�� ����Դϴ�.
		IDirect3D9*          m_pD3D9;
		IDirect3DDevice9*    m_pD3DDevice9;
		D3DCOLOR             m_clearColor;
	};

} // namespace RX end
#endif
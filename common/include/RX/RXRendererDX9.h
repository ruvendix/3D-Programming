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
#include "RXVertexBufferDX9.h"
#include "RXIndexBufferDX9.h"

namespace RX
{

	class DLL_DEFINE RXRendererDX9
	{
		PHOENIX_SINGLETON(RXRendererDX9);
	public:
		void Init();
		void ResetDrawCallCount()
		{
			m_drawCallCnt = 0;
		}

		// ���ҽ��� �޸�Ǯ�� D3DPOOL_DEFAULT�� �͸� �����մϴ�.
		// �� ���� ���ҽ��� �������� �ʽ��ϴ�.
		void ArrangeVideoMemory();

		HRESULT CreateDevice();
		HRESULT BeginRender();
		HRESULT EndRender();
		HRESULT Present();
		HRESULT Release();
		HRESULT VerifyDevice(D3DPRESENT_PARAMETERS* pD3DPP);
		
		// ====================================================================================
		// ���� ����
		HRESULT DrawPrimitive(D3DPRIMITIVETYPE primitiveType,
			const RXVertexBufferDX9& vertexBuffer);
		HRESULT DrawIndexedPrimitive(const RXVertexBufferDX9& vertexBuffer,
			const RXIndexBufferDX9& indexBuffer);

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
		// ====================================================================================
		// �⺻ ����
		INT32                m_drawCallCnt; // �����Ӵ� ������ �Լ� ȣ�� Ƚ���Դϴ�.
		INT32		         m_adapterIdx;  // ���� ����͸� ���� �̴ϴ�.
		bool                 m_bLostDevice;
		bool                 m_bMSAA;       // ��Ƽ ���ø�(��Ƽ�����¡) ��� �����Դϴ�.
		bool		         m_bVSync;      // ���� ����ȭ �����Դϴ�.
		DWORD                m_dwBehavior;  // ���� ó�� ����Դϴ�.
		RECT                 m_rtScissor;   // ������ �������� ����(����Ʈ�� �ƴ� Ŭ����)
		IDirect3D9*          m_pD3D9;
		IDirect3DDevice9*    m_pD3DDevice9;
		D3DCOLOR             m_clearColor;
	};

} // namespace RX end
#endif
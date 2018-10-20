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

		// 리소스의 메모리풀이 D3DPOOL_DEFAULT인 것만 정리합니다.
		// 그 외의 리소스는 정리되지 않습니다.
		void ArrangeVideoMemory();

		HRESULT CreateDevice();
		HRESULT BeginRender();
		HRESULT EndRender();
		HRESULT Present();
		HRESULT Release();
		HRESULT VerifyDevice(D3DPRESENT_PARAMETERS* pD3DPP);
		
		// ====================================================================================
		// 정점 관련
		HRESULT DrawPrimitive(D3DPRIMITIVETYPE primitiveType,
			const RXVertexBufferDX9& vertexBuffer);
		HRESULT DrawIndexedPrimitive(const RXVertexBufferDX9& vertexBuffer,
			const RXIndexBufferDX9& indexBuffer);

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
		// ====================================================================================
		// 기본 정보
		INT32                m_drawCallCnt; // 프레임당 렌더링 함수 호출 횟수입니다.
		INT32		         m_adapterIdx;  // 다중 모니터를 위한 겁니다.
		bool                 m_bLostDevice;
		bool                 m_bMSAA;       // 멀티 샘플링(안티얼라이징) 사용 여부입니다.
		bool		         m_bVSync;      // 수직 동기화 여부입니다.
		DWORD                m_dwBehavior;  // 정점 처리 방식입니다.
		RECT                 m_rtScissor;   // 실제로 렌더링될 영역(뷰포트가 아닌 클리핑)
		IDirect3D9*          m_pD3D9;
		IDirect3DDevice9*    m_pD3DDevice9;
		D3DCOLOR             m_clearColor;
	};

} // namespace RX end
#endif
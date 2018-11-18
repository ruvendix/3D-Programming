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
		void ResetD3DPP(); // 검증이 필요 없는 기본 정보를 설정합니다.
		void ResetDrawCallCount()
		{
			m_drawCallCnt = 0;
		}

		// 리소스의 메모리풀이 D3DPOOL_DEFAULT인 것만 정리합니다.
		// 그 외의 리소스는 정리되지 않습니다.
		void ArrangeVideoMemory();

		// 렌더링 상태를 설정합니다.
		void DefaultRenderState();
		void AdjustRenderState(D3DRENDERSTATETYPE state, DWORD dwValue);

		// 뷰포트를 설정합니다.
		void AdjustViewport(INT32 x, INT32 y, INT32 width, INT32 height);

		// 현재 백버퍼의 서페이스를 파일로 저장합니다. (스크린샷 기능, 많이 느림...)
		void SaveBackBufferToFile(const CHAR* szFile, D3DXIMAGE_FILEFORMAT fileFormat = D3DXIFF_JPG);

		HRESULT CreateDevice();
		HRESULT BeginRender();
		HRESULT EndRender();
		HRESULT Present();
		HRESULT Release();
		HRESULT VerifyDevice(D3DPRESENT_PARAMETERS* pD3DPP);
		
		// ====================================================================================
		// 정점 관련
		void ApplyWorldMatrix(const D3DXMATRIXA16& matWorld);
		void ApplyViewMatrix(const D3DXMATRIXA16& matView);
		void ApplyProjectionMatrix(const D3DXMATRIXA16& matProjection);

		void CopyWorldMatrix(D3DXMATRIXA16* pMatWorld);
		void CopyViewMatrix(D3DXMATRIXA16* pMatView);
		void CopyProjectionMatrix(D3DXMATRIXA16* pMatProjection);

		void DefaultProjectionMatrix();

		HRESULT DrawPrimitive(D3DPRIMITIVETYPE primitiveType,
			const RXVertexBufferDX9& vertexBuffer);
		HRESULT DrawIdxedPrimitive(const RXVertexBufferDX9& vertexBuffer,
			const RXIndexBufferDX9& IndexBuffer);
		HRESULT DrawDXMesh(const LPD3DXMESH pMesh);

		// ====================================================================================
		// 폰트 관련 함수
		HRESULT CreateFontDX9(const WCHAR* szFont, INT32 height);

		// 클라이언트 영역 왼쪽 위를 기준으로 텍스트를 렌더링하는 함수
		void DrawTextOriginClientArea(const TCHAR* szText, DWORD flag, D3DCOLOR color);

		// ====================================================================================
		// 가상 디바이스 상태에 따른 처리 함수
		HRESULT OnLostDevice();
		HRESULT OnResetDevice();
		HRESULT OnRecreateDevice();

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

		ID3DXFont* getD3DXFont() const noexcept
		{
			return m_pD3DXFont;
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
		INT32                  m_drawCallCnt; // 프레임당 렌더링 함수 호출 횟수입니다.
		INT32		           m_adapterIdx;  // 다중 모니터를 위한 겁니다.
		bool                   m_bLostDevice; // 가상 디바이스 손실 여부입니다.
		bool                   m_bMSAA;       // 멀티 샘플링(안티얼라이징) 사용 여부입니다.
		bool		           m_bVSync;      // 수직동기화 여부입니다.
		DWORD                  m_dwBehavior;  // 정점 처리 방식입니다.
		RECT                   m_rtScissor;   // 실제로 렌더링될 영역(뷰포트가 아닌 클리핑)
		IDirect3D9*            m_pD3D9;       // D3D9 객체입니다.
		IDirect3DDevice9*      m_pD3DDevice9; // D3D9 가상 디바이스입니다.
		ID3DXFont*             m_pD3DXFont;   // D3DX 폰트 객체입니다.
		D3DPRESENT_PARAMETERS* m_pD3DPP;      // D3D9 가상 디바이스의 정보입니다.
		D3DVIEWPORT9           m_viewport9;   // 백버퍼 서페이스의 뷰포트입니다.
		D3DCOLOR               m_clearColor;  // 백버퍼 서페이스의 클리어 색상입니다.
	};

} // namespace RX end
#endif
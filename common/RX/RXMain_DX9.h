/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-08-12
 *
 * <파일 내용>
 * _tWinMain()의 코어 서브 루틴입니다.
 * 외부 프로젝트에서 사용하면 편하게 프로그래밍을 할 수 있습니다.
 * 서브 클래싱도 지원하므로 자유로운 수정도 가능합니다.
 * DirectX9 전용 코어 서브 루틴입니다.
 *
 ====================================================================================*/
#ifndef MAIN_SUB_DIRECTX9_H_
#define MAIN_SUB_DIRECTX9_H_

#include "RXMain.h"
#include "RXFrame.h"

#ifdef RXBASEDX_EXPORTS
#include "stdafx.h"
#endif

namespace RX
{

	class DLL_DEFINE RXMain_DX9 : public RXMain
	{
	public:
		RXMain_DX9();
		virtual ~RXMain_DX9();

		virtual HRESULT InitMain()  override;
		virtual HRESULT InitD3D9();
		virtual HRESULT DriveMain() override;
		virtual HRESULT Release()   override;
		virtual void    ChangeScreenMode(bool bFullScreen) override;

		HRESULT VerifyDevice(D3DPRESENT_PARAMETERS* pD3DPP);

		// ====================================================================================
		// 메인 루프
		virtual HRESULT Update();
		HRESULT Render(FLOAT rInterpolation);
		HRESULT BeginRender();
		HRESULT EndRender();

		// ====================================================================================
		// 로스트 디바이스, 리셋 디바이스
		HRESULT OnLostDevice();
		HRESULT OnResetDevice();

		// ====================================================================================
		// getter
		IDirect3D9* getD3D9() const noexcept
		{
			return m_pD3D9;
		}

		IDirect3DDevice9* getD3DDevice9() const noexcept
		{
			return m_pD3DDevice9;
		}

		// ====================================================================================
		// setter

	protected:
		// ====================================================================================
		// 기본 정보
		bool                 m_bLostDevice;
		bool                 m_bMSAA;      // 멀티 샘플링(안티 얼라이징) 사용 여부입니다.
		DWORD                m_dwBehavior; // 정점 처리 방식입니다.
		IDirect3D9*          m_pD3D9;
		IDirect3DDevice9*    m_pD3DDevice9;
		RXFrame              m_frame;
	};

} // namespace RX end

#endif
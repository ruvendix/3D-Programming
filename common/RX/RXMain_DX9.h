/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <�ۼ� ��¥>
 * 2018-08-12
 *
 * <���� ����>
 * _tWinMain()�� �ھ� ���� ��ƾ�Դϴ�.
 * �ܺ� ������Ʈ���� ����ϸ� ���ϰ� ���α׷����� �� �� �ֽ��ϴ�.
 * ���� Ŭ���̵� �����ϹǷ� �����ο� ������ �����մϴ�.
 * DirectX9 ���� �ھ� ���� ��ƾ�Դϴ�.
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

		// ====================================================================================
		// ���� ����
		virtual HRESULT Update();
		HRESULT Render(FLOAT rInterpolation);
		HRESULT BeginRender();
		HRESULT EndRender();

		// ====================================================================================
		// �ν�Ʈ ����̽�, ���� ����̽�
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
		// �⺻ ����
		bool                 m_bLostDevice;
		IDirect3D9*          m_pD3D9;
		IDirect3DDevice9*    m_pD3DDevice9;
		RXFrame              m_frame;
	};

} // namespace RX end

#endif
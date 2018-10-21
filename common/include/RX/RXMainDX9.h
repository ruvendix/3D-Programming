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
#ifndef RXMAINDX9_H_
#define RXMAINDX9_H_

#include "RXMain.h"

namespace RX
{

	class DLL_DEFINE RXMain_DX9 : public RXMain
	{
	public:
		RXMain_DX9();
		virtual ~RXMain_DX9();

		virtual HRESULT InitMain()     override;
		virtual HRESULT InitD3D9();
		virtual HRESULT DriveMain() override;
		virtual HRESULT Release()   override;
		virtual HRESULT ResizeResolution(INT32 clientWidth, INT32 clientHeight) override;
		virtual HRESULT ToggleFullScreenMode(bool bFullScreen = false) override;

		// ====================================================================================
		// 메인 루프
		virtual HRESULT Update();
		HRESULT Render(FLOAT rInterpolation);

		// ====================================================================================
		// 가상 디바이스 상태에 따른 처리 함수
		HRESULT OnLostDevice();
		HRESULT OnResetDevice();
		HRESULT OnRecreateDevice();
	};

} // namespace RX end

#endif
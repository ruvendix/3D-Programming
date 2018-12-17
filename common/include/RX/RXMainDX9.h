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

#include "common.h"

namespace RX
{

	class DLL_DEFINE RXMain_DX9
	{
		PHOENIX_SINGLETON_CTOR(RXMain_DX9);
	public:
		RXMain_DX9();

		// ====================================================================================
		// 초기화
		HRESULT InitInstance();
		HRESULT CreateProgramWindow();
		HRESULT InitMain();
		HRESULT InitD3D9();

		// ====================================================================================
		// 메인 루프
		HRESULT RunMainRoutine(HINSTANCE hInst, INT32 iconID = 0);
		HRESULT Update();
		HRESULT Render(FLOAT rInterpolation);
		HRESULT Release();
		HRESULT DriveMain();

		// ====================================================================================
		// 화면 정보 변경
		HRESULT ResizeResolution(INT32 clientWidth, INT32 clientHeight);
		HRESULT ToggleFullScreenMode(bool bFullScreen = false);
		
		// 원하는 클라이언트 해상도를 넣으면
		// 윈도우 영역까지 계산해서 적용해줍니다.
		void AdjustProgramRange(INT32 width, INT32 height);

		// ====================================================================================
		// 나머지
		void ChangeProgramTitle(const TCHAR* szTitle);
		
		bool IsFullScreen() const
		{
			return (m_bFullScreen == true);
		}

		// ====================================================================================
		// 가상 디바이스 상태에 따른 처리 함수
		HRESULT OnLostDevice(); // 무한대기
		HRESULT OnResetDevice();

		// ====================================================================================
		// Getter
		HWND getMainWindowHandle() const noexcept
		{
			return m_hMainWnd;
		}

		HMONITOR getMainMonitorHandle() const noexcept
		{
			return m_hMainMonitor;
		}

		HINSTANCE getProgramInstance() const noexcept
		{
			return m_hInst;
		}

		INT32 getMessageCode() const noexcept
		{
			return m_msgCode;
		}

		RXRect* getWindowRect()
		{
			return &m_rtWindow;
		}

		RXRect* getClientRect()
		{
			return &m_rtClient;
		}

		RXRect* getMonitorRect()
		{
			return &m_rtMonitor;
		}

		// ====================================================================================
		// Setter
		void setSubFunc(const SubFunc& subFunc, SUBFUNC_TYPE type)
		{
			m_subFunc[static_cast<INT32>(type)].subFunc = subFunc;
		}

	private:
		bool          m_bFullScreen;  // 전체 화면 여부입니다.
		HWND          m_hMainWnd;     // 프로그램 주 창 핸들입니다.
		HMONITOR      m_hMainMonitor; // 주 모니터입니다.
		HINSTANCE     m_hInst;        // 프로그램 인스턴스 핸들입니다.
		ROUTINE_STATE m_routineState; // 프로그램 루틴 상태입니다.
		INT32         m_msgCode;      // 메시지 핸들러의 코드값입니다.
		INT32         m_iconID;       // 프로그램 아이콘 아이디입니다.
		RXRect        m_rtClient;     // 프로그램 클라이언트 영역입니다.
		RXRect        m_rtWindow;     // 프로그램 전체 영역입니다.
		RXRect        m_rtMonitor;    // 모니터 전체 영역입니다.
		SubFuncInfo   m_subFunc[SubFuncInfo::MAX_SUBFUNC]; // 콜백 서브 루틴입니다.
	};

} // namespace RX end

#endif
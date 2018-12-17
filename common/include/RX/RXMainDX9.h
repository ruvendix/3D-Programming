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
		// �ʱ�ȭ
		HRESULT InitInstance();
		HRESULT CreateProgramWindow();
		HRESULT InitMain();
		HRESULT InitD3D9();

		// ====================================================================================
		// ���� ����
		HRESULT RunMainRoutine(HINSTANCE hInst, INT32 iconID = 0);
		HRESULT Update();
		HRESULT Render(FLOAT rInterpolation);
		HRESULT Release();
		HRESULT DriveMain();

		// ====================================================================================
		// ȭ�� ���� ����
		HRESULT ResizeResolution(INT32 clientWidth, INT32 clientHeight);
		HRESULT ToggleFullScreenMode(bool bFullScreen = false);
		
		// ���ϴ� Ŭ���̾�Ʈ �ػ󵵸� ������
		// ������ �������� ����ؼ� �������ݴϴ�.
		void AdjustProgramRange(INT32 width, INT32 height);

		// ====================================================================================
		// ������
		void ChangeProgramTitle(const TCHAR* szTitle);
		
		bool IsFullScreen() const
		{
			return (m_bFullScreen == true);
		}

		// ====================================================================================
		// ���� ����̽� ���¿� ���� ó�� �Լ�
		HRESULT OnLostDevice(); // ���Ѵ��
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
		bool          m_bFullScreen;  // ��ü ȭ�� �����Դϴ�.
		HWND          m_hMainWnd;     // ���α׷� �� â �ڵ��Դϴ�.
		HMONITOR      m_hMainMonitor; // �� ������Դϴ�.
		HINSTANCE     m_hInst;        // ���α׷� �ν��Ͻ� �ڵ��Դϴ�.
		ROUTINE_STATE m_routineState; // ���α׷� ��ƾ �����Դϴ�.
		INT32         m_msgCode;      // �޽��� �ڵ鷯�� �ڵ尪�Դϴ�.
		INT32         m_iconID;       // ���α׷� ������ ���̵��Դϴ�.
		RXRect        m_rtClient;     // ���α׷� Ŭ���̾�Ʈ �����Դϴ�.
		RXRect        m_rtWindow;     // ���α׷� ��ü �����Դϴ�.
		RXRect        m_rtMonitor;    // ����� ��ü �����Դϴ�.
		SubFuncInfo   m_subFunc[SubFuncInfo::MAX_SUBFUNC]; // �ݹ� ���� ��ƾ�Դϴ�.
	};

} // namespace RX end

#endif
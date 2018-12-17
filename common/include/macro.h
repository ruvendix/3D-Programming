/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-08-07
 *
 * <파일 내용>
 * 공동으로 사용되는 매크로 모음입니다.
 * 전역 함수에 영향을 받습니다.
 *
 ====================================================================================*/
#ifndef MACRO_H__
#define MACRO_H__

#include "global_function.h"

 // ====================================================================================
 // 비트 관련 매크로입니다.
#define ON_BIT(data, value)  (data |= value)
#define OFF_BIT(data, value) (data &= ~value)
#define IS_BIT(data, value)  ((data & value) != 0)
#define TOGGLE_BIT(data, value)\
if (IS_BIT(data, value))\
{\
	OFF_BIT(data, value);\
}\
else\
{\
	ON_BIT(data, value);\
}

// ====================================================================================
// 동적 할당, Release, LostDevice, ResetDevice 관련 매크로입니다.
// 동적 할당 영역은 _NORMAL_BLOCK으로 설정합니다.
#if defined(DEBUG) | defined(_DEBUG)
#define RXNew new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define RXNew new
#endif

#if defined(DEBUG) | defined(_DEBUG)
#define RXMalloc(size) _malloc_dbg(size, _NORMAL_BLOCK, __FILE__ , __LINE__)
#else
#define RXMalloc(size) malloc(size)
#endif

#define SAFE_DELTE(ptr)\
if (ptr)\
{\
	delete ptr;\
	ptr = nullptr;\
}

#define SAFE_DELTE_ARR(ptr)\
if (ptr)\
{\
	delete[] ptr;\
	ptr = nullptr;\
}

#define SAFE_RELEASE(ptr)\
if (ptr)\
{\
    ptr->Release();\
    ptr = nullptr;\
}

// 장치를 손실했을 때 사용됩니다.
#define SAFE_LOST_DEVICE(ptr)\
if (ptr)\
{\
	ptr->OnLostDevice();\
}

// 장치를 복구할 때 사용됩니다.
#define SAFE_RESET_DEVICE(ptr)\
if (ptr)\
{\
	ptr->OnResetDevice();\
}

// ====================================================================================
// 메시지 박스 관련 매크로입니다.
#if defined(_UNICODE) || defined(UNICODE)
#define WIDEN(x)       L##x
#define WIDEN2(x)      WIDEN(x)
#define __TFILE__      WIDEN2(__FILE__)
#define __TFUNCTION__  WIDEN2(__FUNCTION__)
#define __TFUNCSIG__   WIDEN2(__FUNCSIG__)
#else
#define __TFILE__      __FILE__
#define __TFUNCTION__  __FUNCTION__
#define __TFUNCSIG__   __FUNCSIG__
#endif

// WCHAR 전용입니다.
#define __WFILE__     WIDEN2(__FILE__)
#define __WFUNCTION__ WIDEN2(__FUNCTION__)
#define __WFUNCSIG__  WIDEN2(__FUNCSIG__)

// 메시지 박스만 출력합니다.
// 프로젝트 모드에 상관없이 무조건 알려줘야 할 때 사용합니다.
#if defined(_UNICODE) || defined(UNICODE)
#define MSGBOX(szText)      RX::ShowMessageBoxImplW(L##szText)
#else					         
#define MSGBOX(szText)      RX::ShowMessageBoxImplA(szText)
#endif

#define NULLCHK(ptr)\
if (ptr == nullptr)\
{\
    RXERRLOG(#ptr " is nullptr!");\
}

#define NULLCHK_RETURN(ptr, szErr)\
if (ptr == nullptr)\
{\
    RXERRLOG(#ptr " is nullptr!");\
	RXERRLOG_RETURN(szErr);\
}

#define NULLCHK_RETURN_NOCOMENT(ptr)\
if (ptr == nullptr)\
{\
    RXERRLOG_RETURN(#ptr " is nullptr!");\
}

#define NULLCHK_RETURN_FALSE(ptr)\
if (ptr == nullptr)\
{\
	RXERRLOG_RETURN_FALSE(#ptr " is nullptr!");\
    return false;\
}

#define NULLCHK_RETURN_EFAIL(ptr, szErr)\
if (ptr == nullptr)\
{\
    RXERRLOG(#ptr " is nullptr!");\
	RXERRLOG_RETURN_EFAIL(szErr);\
}

#define NULLCHK_RETURN_EFAIL_NOCOMENT(ptr)\
if (ptr == nullptr)\
{\
    RXERRLOG_RETURN_EFAIL(#ptr " is nullptr!");\
}

#define NULLCHK_HEAPALLOC(ptr)\
if (ptr == nullptr)\
{\
    RXERRLOG(#ptr " is failed in heap allocation!");\
}

#define NULL_RETURN(ptr)\
if (ptr == nullptr)\
{\
    return;\
}

#define NULL_RETURN_OK(ptr)\
if (ptr == nullptr)\
{\
    return S_OK;\
}

// 가변인자가 지원되지 않는 매크로 함수입니다.
#define RXERRLOG_RETURN(szErr)       RXERRLOG(szErr); return
#define RXERRLOG_RETURN_FALSE(szErr) RXERRLOG(szErr); return false
#define RXERRLOG_RETURN_EFAIL(szErr) RXERRLOG(szErr); return E_FAIL

// ====================================================================================
// 스트링 관련 매크로입니다.

// 디버그 모드만 작동, 릴리즈 모드는 X
#if defined(DEBUG) || defined(_DEBUG)
	#if defined(_UNICODE) || defined(UNICODE)
	#define RXDEBUGLOG(szFormat, ...)\
		RX::RXLogImplW(PROJECT_MODE::PM_DEBUG, false, false, false,\
			nullptr, __LINE__, __TFUNCSIG__, L##szFormat, __VA_ARGS__)
	#else
	#define RXDEBUGLOG(szFormat, ...)\
		RX::RXLogImplA(PROJECT_MODE::PM_DEBUG, false, false, false,\
			nullptr, __LINE__, __TFUNCSIG__, szFormat, __VA_ARGS__)
	#endif
#else
	#define RXDEBUGLOG __noop
#endif

// 서식 문자열 지원, 디버그 모드에서는 디버그 출력창에도 출력
#if defined(DEBUG) || defined(_DEBUG)
	#if defined(_UNICODE) || defined(UNICODE)
	#define RXLOG(szFormat, ...)\
		RX::RXLogImplW(PROJECT_MODE::PM_DEBUG, false, true, false,\
			nullptr, __LINE__, __TFUNCSIG__, L##szFormat, __VA_ARGS__)
	#else
	#define RXLOG(szFormat, ...)\
		RX::RXLogImplA(PROJECT_MODE::PM_DEBUG, false, true, false,\
			nullptr, __LINE__, __TFUNCSIG__, szFormat, __VA_ARGS__)
	#endif
#else
	#if defined(_UNICODE) || defined(UNICODE)
	#define RXLOG(szFormat, ...)\
		RX::RXLogImplW(PROJECT_MODE::PM_RELEASE, false, true, false,\
			nullptr, __LINE__, __TFUNCSIG__, L##szFormat, __VA_ARGS__)
	#else
	#define RXLOG(szFormat, ...)\
		RX::RXLogImplA(PROJECT_MODE::PM_RELEASE, false, true, false,\
			nullptr, __LINE__, __TFUNCSIG__, szFormat, __VA_ARGS__)
	#endif
#endif

// RXLOG()에 메시지 박스 기능이 추가된 함수입니다.
#if defined(DEBUG) || defined(_DEBUG)
#if defined(_UNICODE) || defined(UNICODE)
#define RXMSGBOXLOG(szFormat, ...)\
		RX::RXLogImplW(PROJECT_MODE::PM_DEBUG, true, true, false,\
			nullptr, __LINE__, __TFUNCSIG__, L##szFormat, __VA_ARGS__)
#else
#define RXMSGBOXLOG(szFormat, ...)\
		RX::RXLogImplA(PROJECT_MODE::PM_DEBUG, true, true, false,\
			nullptr, __LINE__, __TFUNCSIG__, szFormat, __VA_ARGS__)
#endif
#else
#if defined(_UNICODE) || defined(UNICODE)
#define RXMSGBOXLOG(szFormat, ...)\
		RX::RXLogImplW(PROJECT_MODE::PM_RELEASE, true, true, false,\
			nullptr, __LINE__, __TFUNCSIG__, L##szFormat, __VA_ARGS__)
#else
#define RXMSGBOXLOG(szFormat, ...)\
		RX::RXLogImplA(PROJECT_MODE::PM_RELEASE, true, true, false,\
			nullptr, __LINE__, __TFUNCSIG__, szFormat, __VA_ARGS__)
#endif
#endif

// 서식 문자열 지원, 디버그 모드에서는 디버그 출력창에도 출력
// 메시지 박스 출력 지원, 에러 메시지 박스만 지원합니다.
#if defined(DEBUG) || defined(_DEBUG)
	#if defined(_UNICODE) || defined(UNICODE)
	#define RXERRLOG(szFormat, ...) RX::RXLogImplW(PROJECT_MODE::PM_DEBUG, true, true, true,\
		nullptr, __LINE__, __TFUNCSIG__, L##szFormat, __VA_ARGS__)
	#else
	#define RXERRLOG(szFormat, ...) RX::RXLogImplA(PROJECT_MODE::PM_DEBUG, true, true, true,\
		nullptr, __LINE__, __TFUNCSIG__, szFormat, __VA_ARGS__)
	#endif
#else
	#if defined(_UNICODE) || defined(UNICODE)
	#define RXERRLOG(szFormat, ...) RX::RXLogImplW(PROJECT_MODE::PM_RELEASE, true, true, true,\
		nullptr, __LINE__, __TFUNCSIG__, L##szFormat, __VA_ARGS__)
	#else
	#define RXERRLOG(szFormat, ...) RX::RXLogImplA(PROJECT_MODE::PM_RELEASE, true, true, true,\
		nullptr, __LINE__, __TFUNCSIG__, szFormat, __VA_ARGS__)
	#endif
#endif

// CHAR 전용입니다.
#define RXLOG_CHAR(bMessageBox, szFormat, ...)\
		RX::RXLogImplA(PROJECT_MODE::PM_DEBUG, bMessageBox, true, false,\
			nullptr, __LINE__, __FUNCSIG__, szFormat, __VA_ARGS__)

#define RXERRLOG_CHAR(szFormat, ...) RX::RXLogImplA(PROJECT_MODE::PM_DEBUG, true, true, true,\
		nullptr, __LINE__, __FUNCSIG__, szFormat, __VA_ARGS__)

// WCHAR 전용입니다.
#define RXLOG_WCHAR(bMessageBox, szFormat, ...)\
		RX::RXLogImplW(PROJECT_MODE::PM_DEBUG, bMessageBox, true, false,\
			nullptr, __LINE__, __WFUNCSIG__, L##szFormat, __VA_ARGS__)

#define RXERRLOG_WCHAR(szFormat, ...) RX::RXLogImplW(PROJECT_MODE::PM_DEBUG, true, true, true,\
		nullptr, __LINE__, __WFUNCSIG__, L##szFormat, __VA_ARGS__)

// ====================================================================================
// DirectX 관련 매크로입니다.

#define DXCOLOR_WHITE      D3DCOLOR_ARGB(255, 255, 255, 255) // 흰색
#define DXCOLOR_BLACK      D3DCOLOR_ARGB(255,   0,   0,   0) // 검은색
#define DXCOLOR_RED        D3DCOLOR_ARGB(255, 255,   0,   0) // 빨간색
#define DXCOLOR_GREEN      D3DCOLOR_ARGB(255,   0, 255,   0) // 초록색
#define DXCOLOR_BLUE       D3DCOLOR_ARGB(255,   0,   0, 255) // 파란색
#define DXCOLOR_MAGENTA    D3DCOLOR_ARGB(255, 255,   0, 255) // 심홍색
#define DXCOLOR_YELLOW     D3DCOLOR_ARGB(255, 255, 255,   0) // 노란색
#define DXCOLOR_CYAN       D3DCOLOR_ARGB(255,   0, 255, 255) // 청록색
#define DXCOLOR_TEAL       D3DCOLOR_ARGB(255,   0, 128, 128) // 옥색
#define DXCOLOR_OLIVE      D3DCOLOR_ARGB(255, 128, 128,   0) // 올리브색
#define DXCOLOR_PURPLE     D3DCOLOR_ARGB(255, 128,   0, 128) // 자주색
#define DXCOLOR_MAROON     D3DCOLOR_ARGB(255, 128,   0,   0) // 고동색
#define DXCOLOR_DARK_GREEN D3DCOLOR_ARGB(255,   0, 128,   0) // 어두운 초록색
#define DXCOLOR_NAVY       D3DCOLOR_ARGB(255,   0,   0, 128) // 짙은 파란색

// DirectX 에러 핸들러 매크로입니다.
// HRESULT값만 넣어야 정상 작동합니다.
#define DXERR_HANDLER(hDXResult)\
if (FAILED(hDXResult))\
{\
	DXERR_HANDLER_IMPL(hDXResult);\
}

// 파일, 라인, 에러 이름, 에러 내용
#if defined(DEBUG) || defined(_DEBUG)
	#if defined(_UNICODE) || defined(UNICODE)
	#define DXERR_HANDLER_IMPL(hDXResult) RX::DXErrorHandlerImplW(hDXResult, PROJECT_MODE::PM_DEBUG,\
				nullptr, __LINE__, __TFUNCSIG__);
	#else
	#define DXERR_HANDLER_IMPL(hDXResult) RX::DXErrorHandlerImplA(hDXResult, PROJECT_MODE::PM_DEBUG,\
				nullptr, __LINE__, __TFUNCSIG__);
	#endif
#else
	#if defined(_UNICODE) || defined(UNICODE)
	#define DXERR_HANDLER_IMPL(hDXResult) RX::DXErrorHandlerImplW(hDXResult, PROJECT_MODE::PM_RELEASE,\
				nullptr, __LINE__, __TFUNCSIG__);
	#else
	#define DXERR_HANDLER_IMPL(hDXResult) RX::DXErrorHandlerImplA(hDXResult, PROJECT_MODE::PM_RELEASE,\
				nullptr, __LINE__, __TFUNCSIG__);
	#endif
#endif

// ====================================================================================
// 그 외의 매크로입니다.

// 파일, 라인, 에러 이름, 에러 내용
// GetLastError()를 이용한 Win32 API의 에러 핸들러입니다.
#if defined(DEBUG) || defined(_DEBUG)
	#if defined(_UNICODE) || defined(UNICODE)
	#define WIN32ERR_HANDLER() RX::Win32LastErrorHandlerImplW(PROJECT_MODE::PM_DEBUG,\
				nullptr, __LINE__, __TFUNSIG__);
	#else
	#define WIN32ERR_HANDLER() RX::Win32LastErrorHandlerImplW(PROJECT_MODE::PM_DEBUG,\
				nullptr, __LINE__, __TFUNSIG__);
#endif
#else
	#if defined(_UNICODE) || defined(UNICODE)
	#define WIN32ERR_HANDLER() RX::Win32LastErrorHandlerImplA(PROJECT_MODE::PM_RELEASE,\
				nullptr, __LINE__, __TFUNSIG__);
	#else
	#define WIN32ERR_HANDLER() RX::Win32LastErrorHandlerImplA(PROJECT_MODE::PM_RELEASE,\
				nullptr, __LINE__, __TFUNSIG__);
	#endif
#endif

// 플래그값을 문자열로 변경시키는 매크로입니다.
#define CONVERT_FLAG_TO_STRING(flag) _T(#flag)

// ====================================================================================
// 피닉스 싱글톤입니다.
// 죽었다가 다시 부활한다는 의미로 피닉스라고 합니다...
//
// 중간에 new(m_pInst) classType은 Replacement New입니다.
// 메모리를 새로 할당하지 않고 생성자만 호출합니다.
//
// 생성자와 소멸자는 시점으로 인해 고정입니다.
// 따라서 생성 작업은 Init(), 해제 작업은 Release() 같은 함수를 만들어야 합니다.
#define PHOENIX_SINGLETON(classType)\
public:\
	static classType* Instance()\
	{\
		if (m_bDestroy == true)\
		{\
			Create();\
			new(m_pInst) classType;\
			std::atexit(Destroy);\
			m_bDestroy = true;\
		}\
		if (m_pInst == nullptr)\
		{\
			Create();\
		}\
		return m_pInst;\
	}\
private:\
	classType() = default;\
	classType(const classType& rhs)  = delete;\
	classType(const classType&& rhs) = delete;\
	~classType()\
	{\
		m_bDestroy = true;\
	}\
	classType& operator=(const classType& rhs)  = delete;\
	classType& operator=(const classType&& rhs) = delete;\
	static void Create()\
	{\
		static classType inst;\
		m_pInst = &inst;\
	}\
	static void Destroy()\
	{\
		m_pInst->~classType();\
	}\
	static bool m_bDestroy;\
	static classType* m_pInst;

// 피닉스 싱글톤의 생성자 재정의가 가능한 매크로 함수입니다.
#define PHOENIX_SINGLETON_CTOR(classType)\
public:\
	static classType* Instance()\
	{\
		if (m_bDestroy == true)\
		{\
			Create();\
			new(m_pInst) classType;\
			std::atexit(Destroy);\
			m_bDestroy = true;\
		}\
		if (m_pInst == nullptr)\
		{\
			Create();\
		}\
		return m_pInst;\
	}\
private:\
	classType(const classType& rhs)  = delete;\
	classType(const classType&& rhs) = delete;\
	~classType()\
	{\
		m_bDestroy = true;\
	}\
	classType& operator=(const classType& rhs)  = delete;\
	classType& operator=(const classType&& rhs) = delete;\
	static void Create()\
	{\
		static classType inst;\
		m_pInst = &inst;\
	}\
	static void Destroy()\
	{\
		m_pInst->~classType();\
	}\
	static bool m_bDestroy;\
	static classType* m_pInst;

// 피닉스 싱글톤의 멤버변수 초기화 매크로 함수입니다.
#define PHOENIX_SINGLETON_INIT(classType)\
	classType* classType::m_pInst = nullptr;\
	bool classType::m_bDestroy = false;

// 전역 인스턴스 모음입니다.
#define RXMAIN_DX9    RX::RXMain_DX9::Instance()
#define MAININST      RX::RXMain_DX9::Instance()->getmain()->getProgramInstanceHandle()
#define MAINWND       RX::RXMain_DX9::Instance()->getMainWindowHandle()
#define RXRENDERERDX9 RX::RXRendererDX9::Instance()
#define D3D9          RX::RXRendererDX9::Instance()->getD3D9()
#define D3DDEVICE9    RX::RXRendererDX9::Instance()->getD3DDevice9()
#define RXFPSMGR      RX::RXProgramFPSMgr::Instance()

#endif
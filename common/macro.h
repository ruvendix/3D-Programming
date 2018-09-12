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
// 동적 할당, Release, LostDevice, ResetDevice 관련 매크로입니다.
// 동적 할당 영역은 _NORMAL_BLOCK으로 설정합니다.
//
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
//
#if defined(_UNICODE) || defined(UNICODE)
#define WIDEN(x)       L##x
#define WIDEN2(x)      WIDEN(x)
#define __TFILE__      WIDEN2(__FILE__)
#define __TFUNCTION__  WIDEN2(__FUNCTION__)
#define __TFUNSIG__    WIDEN2(__FUNCSIG__)
#else
#define __TFILE__      __FILE__
#define __TFUNCTION__  __FUNCTION__
#define __TFUNSIG__    __FUNCSIG__
#endif

// 메시지 박스만 출력합니다.
// 프로젝트 모드에 상관없이 무조건 알려줘야 할 때 사용합니다.
// ERRMSGBOX()는 로그로도 출력합니다.
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

#define NULLCHK_RETURN_NOCOMENT(ptr)\
if (ptr == nullptr)\
{\
    RXERRLOG_RETURN(#ptr " is nullptr!");\
}

#define NULLCHK_RETURN(ptr, szErr)\
if (ptr == nullptr)\
{\
    RXERRLOG(#ptr " is nullptr!");\
	RXERRLOG_RETURN(szErr);\
}

#define NULLCHK_EFAIL_RETURN(ptr, szErr)\
if (ptr == nullptr)\
{\
    RXERRLOG(#ptr " is nullptr!");\
	RXERRLOG_EFAIL_RETURN(szErr);\
}

#define NULLCHK_HEAPALLOC(ptr)\
if (ptr == nullptr)\
{\
    RXERRLOG(#ptr " is failed in heap allocation!");\
}

#define RXERRLOG_RETURN(szErr)       RXERRLOG(szErr); return
#define RXERRLOG_EFAIL_RETURN(szErr) RXERRLOG(szErr); return E_FAIL

// ====================================================================================
// 스트링 관련 매크로입니다.
//
// 디버그 모드만 작동, 릴리즈 모드는 X
// RXDEBUGLOG() RX::RXDebugLogImpl()
#if defined(DEBUG) || defined(_DEBUG)
	#if defined(_UNICODE) || defined(UNICODE)
#define RXDEBUGLOG(szText) RX::RXDebugLogImplW(L##szText)
	#else
	#define RXDEBUGLOG(szText) RX::RXDebugLogImplA(szText)
	#endif
#else
	#define RXDEBUGLOG(szText) __noop
#endif

// 서식 문자열 지원, 디버그 모드에서는 디버그 출력창에도 출력
// 메시지 박스 출력 지원, 일반 메시지 박스만 지원합니다.
#if defined(DEBUG) || defined(_DEBUG)
	#if defined(_UNICODE) || defined(UNICODE)
	#define RXLOG(bMessageBox, szFormat, ...)\
		RX::RXLogImplW(PROJECT_MODE::PM_DEBUG, bMessageBox, false,\
			__TFILE__, __LINE__, __TFUNSIG__, L##szFormat, __VA_ARGS__)
	#else
	#define RXLOG(bMessageBox, szFormat, ...)\
		RX::RXLogImplA(PROJECT_MODE::PM_DEBUG, bMessageBox, false,\
			__TFILE__, __LINE__, __TFUNSIG__, szFormat, __VA_ARGS__)
	#endif
#else
	#if defined(_UNICODE) || defined(UNICODE)
	#define RXLOG(bMessageBox, szFormat, ...)\
		RX::RXLogImplW(PROJECT_MODE::PM_RELEASE, bMessageBox, false,\
			__TFILE__, __LINE__, __TFUNSIG__, L##szFormat, __VA_ARGS__)
	#else
	#define RXLOG(bMessageBox, szFormat, ...)\
		RX::RXLogImplA(PROJECT_MODE::PM_RELEASE, bMessageBox, false,\
			__TFILE__, __LINE__, __TFUNSIG__, szFormat, __VA_ARGS__)
	#endif
#endif

// 서식 문자열 지원, 디버그 모드에서는 디버그 출력창에도 출력
// 메시지 박스 출력 지원, 에러 메시지 박스만 지원합니다.
#if defined(DEBUG) || defined(_DEBUG)
	#if defined(_UNICODE) || defined(UNICODE)
	#define RXERRLOG(szFormat, ...) RX::RXLogImplW(PROJECT_MODE::PM_DEBUG, true, true,\
		__TFILE__, __LINE__, __TFUNSIG__, L##szFormat, __VA_ARGS__)
	#else
	#define RXERRLOG(szFormat, ...) RX::RXLogImplA(PROJECT_MODE::PM_DEBUG, true, true,\
		__TFILE__, __LINE__, __TFUNSIG__, szFormat, __VA_ARGS__)
	#endif
#else
	#if defined(_UNICODE) || defined(UNICODE)
	#define RXERRLOG(szFormat, ...) RX::RXLogImplW(PROJECT_MODE::PM_RELEASE, true, true,\
		__TFILE__, __LINE__, __TFUNSIG__, L##szFormat, __VA_ARGS__)
	#else
	#define RXERRLOG(szFormat, ...) RX::RXLogImplA(PROJECT_MODE::PM_RELEASE, true, true,\
		__TFILE__, __LINE__, __TFUNSIG__, szFormat, __VA_ARGS__)
	#endif
#endif

// ====================================================================================
// DirectX 관련 매크로입니다.
//
#define DXCOLOR_WHITE   D3DCOLOR_ARGB(255, 255, 255, 255)
#define DXCOLOR_BLACK   D3DCOLOR_ARGB(255,   0,   0,   0)
#define DXCOLOR_RED     D3DCOLOR_ARGB(255, 255,   0,   0)
#define DXCOLOR_GREEN   D3DCOLOR_ARGB(255,   0, 255,   0)
#define DXCOLOR_BLUE    D3DCOLOR_ARGB(255,   0,   0, 255)
#define DXCOLOR_MAGENTA D3DCOLOR_ARGB(255, 255,   0, 255)

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
				__TFILE__, __LINE__, __TFUNSIG__);
	#else
	#define DXERR_HANDLER_IMPL(hDXResult) RX::DXErrorHandlerImplA(hDXResult, PROJECT_MODE::PM_DEBUG,\
				__TFILE__, __LINE__, __TFUNSIG__);
	#endif
#else
	#if defined(_UNICODE) || defined(UNICODE)
	#define DXERR_HANDLER_IMPL(hDXResult) RX::DXErrorHandlerImplW(hDXResult, PROJECT_MODE::PM_RELEASE,\
				__TFILE__, __LINE__, __TFUNSIG__);
	#else
	#define DXERR_HANDLER_IMPL(hDXResult) RX::DXErrorHandlerImplA(hDXResult, PROJECT_MODE::PM_RELEASE,\
				__TFILE__, __LINE__, __TFUNSIG__);
	#endif
#endif

// ====================================================================================
// 그 외의 매크로입니다.
//
#if defined(_UNICODE) || defined(UNICODE)
#define Win32LastErrorHandler Win32LastErrorHandlerImplW()
#else
#define Win32LastErrorHandler Win32LastErrorHandlerImplA()
#endif

// 파일, 라인, 에러 이름, 에러 내용
// GetLastError()를 이용한 Win32 API의 에러 핸들러입니다.
#if defined(DEBUG) || defined(_DEBUG)
	#if defined(_UNICODE) || defined(UNICODE)
	#define WIN32ERR_HANDLER() RX::Win32LastErrorHandlerImplW(PROJECT_MODE::PM_DEBUG,\
				__TFILE__, __LINE__, __TFUNSIG__);
	#else
	#define WIN32ERR_HANDLER() RX::Win32LastErrorHandlerImplW(PROJECT_MODE::PM_DEBUG,\
				__TFILE__, __LINE__, __TFUNSIG__);
#endif
#else
	#if defined(_UNICODE) || defined(UNICODE)
	#define WIN32ERR_HANDLER() RX::Win32LastErrorHandlerImplA(PROJECT_MODE::PM_RELEASE,\
				__TFILE__, __LINE__, __TFUNSIG__);
	#else
	#define WIN32ERR_HANDLER() RX::Win32LastErrorHandlerImplA(PROJECT_MODE::PM_RELEASE,\
				__TFILE__, __LINE__, __TFUNSIG__);
	#endif
#endif

#endif
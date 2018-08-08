//////////////////////////////////////////////////////////////////////
// 작성 날짜 : 2018.08.07
// 작성자    : Ruvendix
// 파일 내용 : 공동으로 사용되는 매크로 함수 모음입니다.
//////////////////////////////////////////////////////////////////////
#ifndef MACRO_H__
#define MACRO_H__

//////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////
// 메시지 박스 관련 매크로입니다.
//
#if defined(_UNICODE) || defined(UNICODE)
#define WIDEN(x)       L##x
#define WIDEN2(x)      WIDEN(x)
#define __TFILE__      WIDEN2(__FILE__)
#define __TFUNCTION__  WIDEN2(__FUNCTION__)
#else
#define __TFILE__      __FILE__
#define __TFUNCTION__  __FUNCTION__
#endif

// 메시지 박스만 출력합니다.
// RXLOG()와 RXERRLOG()는 활용 범위가 더 넓습니다.
#if defined(_UNICODE) || defined(UNICODE)
#define MSGBOX(szText)      RX::ShowMessageBoxW(L#szText)
#define ERRMSGBOX(szErr)    RX::ShowErrorMessageBoxW(L#szErr, __TFILE__, __LINE__)
#else					         
#define MSGBOX(szText)      RX::ShowMessageBoxA(szText)
#define ERRMSGBOX(szErr)    RX::ShowErrorMessageBoxA(szErr, __TFILE__, __LINE__)
#endif

#define NULLCHK(ptr)\
if (ptr == nullptr)\
{\
    ERRMSGBOX(#ptr " is nullptr!");\
}

#define NULLCHK_RETURN(ptr)\
if (ptr == nullptr)\
{\
    ERRMSGBOX(#ptr " is nullptr!");\
    return;\
}

#define NULLCHK_EFAIL_RETURN(ptr)\
if (ptr == nullptr)\
{\
    ERRMSGBOX(#ptr " is nullptr!");\
    return E_FAIL;\
}

#define NULLCHK_HEAPALLOC(ptr)\
if (ptr == nullptr)\
{\
    ERRMSGBOX(#ptr " is failed in heap allocation!");\
}

#define ERRMSG_RETURNBOX(szErr)          ERRMSGBOX(szErr); return
#define ERRMSG_EFAIL_RETURNBOX(szErr)    ERRMSGBOX(szErr); return E_FAIL

//////////////////////////////////////////////////////////////////////////
// 스트링 관련 매크로입니다.
//
// 디버그 모드만 작동, 릴리즈 모드는 X
// RXDEBUGLOG() RX::RXDebugLogImpl()
#if defined(DEBUG) || defined(_DEBUG)
#define RXDEBUGLOG(szText) RX::RXDebugLogImpl(szText)
#else
#define RXDEBUGLOG(szText) __noop
#endif

// 서식 문자열 지원, 디버그 모드에서는 디버그 출력창에도 출력
// 메시지 박스 출력 지원, 일반 메시지 박스만 지원합니다.
#if defined(DEBUG) || defined(_DEBUG)
#define RXLOG(bMessageBox, szFormat, ...)\
RX::RXLogImpl(PROJECT_MODE::PM_DEBUG, bMessageBox, false, szFormat, __VA_ARGS__)
#else
#define RXLOG(bMessageBox, szFormat, ...)\
RX::RXLogImpl(PROJECT_MODE::PM_RELEASE, bMessageBox, false, szFormat, __VA_ARGS__)
#endif

// 서식 문자열 지원, 디버그 모드에서는 디버그 출력창에도 출력
// 메시지 박스 출력 지원, 에러 메시지 박스만 지원합니다.
#if defined(DEBUG) || defined(_DEBUG)
#define RXERRLOG(bMessageBox, szFormat, ...)\
RX::RXLogImpl(PROJECT_MODE::PM_DEBUG, bMessageBox, true, szFormat, __VA_ARGS__)
#else
#define RXERRLOG(bMessageBox, szFormat, ...)\
RX::RXLogImpl(PROJECT_MODE::PM_RELEASE, bMessageBox, true, szFormat, __VA_ARGS__)
#endif

//////////////////////////////////////////////////////////////////////////
// DirectX 관련 매크로입니다.
//
#define DXCOLOR_WHITE   D3DCOLOR_ARGB(255, 255, 255, 255)
#define DXCOLOR_BLACK   D3DCOLOR_ARGB(255,   0,   0,   0)
#define DXCOLOR_RED     D3DCOLOR_ARGB(255, 255,   0,   0)
#define DXCOLOR_GREEN   D3DCOLOR_ARGB(255,   0, 255,   0)
#define DXCOLOR_BLUE    D3DCOLOR_ARGB(255,   0,   0, 255)
#define DXCOLOR_MAGENTA D3DCOLOR_ARGB(255, 255,   0, 255)

// 파일, 라인, 에러 이름, 에러 내용
#if defined(DEBUG) || defined(_DEBUG)
#define DXERR_HANDLER(hResult)\
if (FAILED(hResult))\
{\
    RX::DXErrorHandler(DXGetErrorStringA(hResult),\
        DXGetErrorDescriptionA(hResult), PROJECT_MODE::PM_DEBUG);\
}
#else
#define DXERR_HANDLER(hResult)\
if (FAILED(hResult))\
{\
    RX::DXErrorHandler(DXGetErrorStringA(hResult),\
        DXGetErrorDescriptionA(hResult), PROJECT_MODE::PM_RELEASE);\
}
#endif

#endif
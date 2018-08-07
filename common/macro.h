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
#define __MFILE__      __FILE__
#define __MFUNCTION__  __FUNCTION__
#endif

#define MSGBOX_A(szOutPut)         ::MessageBoxA(nullptr, szOutPut, "Caption", MB_OK)
#define MSGBOX_W(szOutPut)         ::MessageBoxW(nullptr, L#szOutPut, L"Caption", MB_OK)

#if defined(_UNICODE) || defined(UNICODE)
#define MSGBOX(szOutPut)           MSGBOX_W(L#szOutPut)
#define ERRMSGBOX(szErr)           RX::ShowErrorMessageBoxW(L#szErr, __TFILE__, __LINE__)
#else						       
#define MSGBOX(szOutPut)           MSGBOX_A(szOutPut)
#define ERRMSGBOX(szErr)           RX::ShowErrorMessageBoxA(szErr, __TFILE__, __LINE__)
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

#define NULLCHK_FAIL_HEAPALLOC(ptr)\
if (ptr == nullptr)\
{\
    ERRMSGBOX(#ptr " is fail heap alloc!");\
}

#define ERRMSG_RETURNBOX(szErr) ERRMSGBOX(szErr); return
#define ERRMSG_EFAIL_RETURNBOX(szErr)  ERRMSGBOX(szErr); return E_FAIL

//////////////////////////////////////////////////////////////////////////
// 스트링 관련 매크로입니다.
//
#if defined(DEBUG) || defined(_DEBUG)
#define RXLOG(sz) MSGBOX_A(sz)
#else
#define RXLOG(sz) OutputDebugStringA(sz)
#endif

// 디버그 모드에서는 메시지 박스로 알려주고
// 그 외의 모드에서는 로그로만 남깁니다. (파일에 쓰도록 개선 예정)
#if defined(DEBUG) || defined(_DEBUG)
#define RXERRLOG(szErr) OutputDebugStringA(szErr)
#else
#define RXERRLOG(szErr) RX::ShowErrorMessageBoxA(szErr, __FILE__, __LINE__)
#endif

// printf()를 사용하는 것처럼 서식 문자열이 필요합니다.
#if defined(DEBUG) || defined(_DEBUG)
#define RXDEBUGLOG(strFormat, ...) RX::OutputDebugLog(strFormat, __VA_ARGS__)
#else
#define RXDEBUGLOG(strFormat) __noop
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
#define DXERR_HANDLER(result)\
if (FAILED(result))\
{\
    RX::ErrorHandler(DXGetErrorStringA(result),\
        DXGetErrorDescriptionA(result));\
}

#endif
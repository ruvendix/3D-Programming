//////////////////////////////////////////////////////////////////////
// �ۼ� ��¥ : 2018.08.07
// �ۼ���    : Ruvendix
// ���� ���� : �������� ���Ǵ� ��ũ�� �Լ� �����Դϴ�.
//////////////////////////////////////////////////////////////////////
#ifndef MACRO_H__
#define MACRO_H__

//////////////////////////////////////////////////////////////////////////
// ���� �Ҵ�, Release, LostDevice, ResetDevice ���� ��ũ���Դϴ�.
// ���� �Ҵ� ������ _NORMAL_BLOCK���� �����մϴ�.
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

// ��ġ�� �ս����� �� ���˴ϴ�.
#define SAFE_LOST_DEVICE(ptr)\
if (ptr)\
{\
	ptr->OnLostDevice();\
}

// ��ġ�� ������ �� ���˴ϴ�.
#define SAFE_RESET_DEVICE(ptr)\
if (ptr)\
{\
	ptr->OnResetDevice();\
}

//////////////////////////////////////////////////////////////////////////
// �޽��� �ڽ� ���� ��ũ���Դϴ�.
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
// ��Ʈ�� ���� ��ũ���Դϴ�.
//
#if defined(DEBUG) || defined(_DEBUG)
#define RXLOG(sz) MSGBOX_A(sz)
#else
#define RXLOG(sz) OutputDebugStringA(sz)
#endif

// ����� ��忡���� �޽��� �ڽ��� �˷��ְ�
// �� ���� ��忡���� �α׷θ� ����ϴ�. (���Ͽ� ������ ���� ����)
#if defined(DEBUG) || defined(_DEBUG)
#define RXERRLOG(szErr) OutputDebugStringA(szErr)
#else
#define RXERRLOG(szErr) RX::ShowErrorMessageBoxA(szErr, __FILE__, __LINE__)
#endif

// printf()�� ����ϴ� ��ó�� ���� ���ڿ��� �ʿ��մϴ�.
#if defined(DEBUG) || defined(_DEBUG)
#define RXDEBUGLOG(strFormat, ...) RX::OutputDebugLog(strFormat, __VA_ARGS__)
#else
#define RXDEBUGLOG(strFormat) __noop
#endif

//////////////////////////////////////////////////////////////////////////
// DirectX ���� ��ũ���Դϴ�.
//
#define DXCOLOR_WHITE   D3DCOLOR_ARGB(255, 255, 255, 255)
#define DXCOLOR_BLACK   D3DCOLOR_ARGB(255,   0,   0,   0)
#define DXCOLOR_RED     D3DCOLOR_ARGB(255, 255,   0,   0)
#define DXCOLOR_GREEN   D3DCOLOR_ARGB(255,   0, 255,   0)
#define DXCOLOR_BLUE    D3DCOLOR_ARGB(255,   0,   0, 255)
#define DXCOLOR_MAGENTA D3DCOLOR_ARGB(255, 255,   0, 255)

// ����, ����, ���� �̸�, ���� ����
#define DXERR_HANDLER(result)\
if (FAILED(result))\
{\
    RX::ErrorHandler(DXGetErrorStringA(result),\
        DXGetErrorDescriptionA(result));\
}

#endif
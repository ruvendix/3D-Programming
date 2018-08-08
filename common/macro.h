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
#define __TFILE__      __FILE__
#define __TFUNCTION__  __FUNCTION__
#endif

// �޽��� �ڽ��� ����մϴ�.
// RXLOG()�� RXERRLOG()�� Ȱ�� ������ �� �н��ϴ�.
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
// ��Ʈ�� ���� ��ũ���Դϴ�.
//
// ����� ��常 �۵�, ������ ���� X
// RXDEBUGLOG() RX::RXDebugLogImpl()
#if defined(DEBUG) || defined(_DEBUG)
#define RXDEBUGLOG(szText) RX::RXDebugLogImpl(szText)
#else
#define RXDEBUGLOG(szText) __noop
#endif

// ���� ���ڿ� ����, ����� ��忡���� ����� ���â���� ���
// �޽��� �ڽ� ��� ����, �Ϲ� �޽��� �ڽ��� �����մϴ�.
#if defined(DEBUG) || defined(_DEBUG)
#define RXLOG(bMessageBox, szFormat, ...)\
RX::RXLogImpl(PROJECT_MODE::PM_DEBUG, bMessageBox, false, szFormat, __VA_ARGS__)
#else
#define RXLOG(bMessageBox, szFormat, ...)\
RX::RXLogImpl(PROJECT_MODE::PM_RELEASE, bMessageBox, false, szFormat, __VA_ARGS__)
#endif

// ���� ���ڿ� ����, ����� ��忡���� ����� ���â���� ���
// �޽��� �ڽ� ��� ����, ���� �޽��� �ڽ��� �����մϴ�.
#if defined(DEBUG) || defined(_DEBUG)
#define RXERRLOG(bMessageBox, szFormat, ...)\
RX::RXLogImpl(PROJECT_MODE::PM_DEBUG, bMessageBox, true, szFormat, __VA_ARGS__)
#else
#define RXERRLOG(bMessageBox, szFormat, ...)\
RX::RXLogImpl(PROJECT_MODE::PM_RELEASE, bMessageBox, true, szFormat, __VA_ARGS__)
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
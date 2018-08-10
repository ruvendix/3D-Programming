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
// ������Ʈ ��忡 ������� ������ �˷���� �� �� ����մϴ�.
// ERRMSGBOX()�� �α׷ε� ����մϴ�.
#if defined(_UNICODE) || defined(UNICODE)
#define MSGBOX(szText)      RX::ShowMessageBoxImplW(L#szText)
#define ERRMSGBOX(szErr)    RX::ShowErrorMessageBoxImplW(L#szErr, __TFILE__, __LINE__); RXERRLOG(false, szErr)
#else					         
#define MSGBOX(szText)      RX::ShowMessageBoxImplA(szText)
#define ERRMSGBOX(szErr)    RX::ShowErrorMessageBoxImplA(szErr, __TFILE__, __LINE__); RXERRLOG(false, szErr)
#endif

#define NULLCHK(ptr)\
if (ptr == nullptr)\
{\
    ERRMSGBOX(#ptr " is nullptr!");\
}

#define NULLCHK_RETURN(ptr, szErr)\
if (ptr == nullptr)\
{\
    ERRMSGBOX(#ptr " is nullptr!");\
	ERRMSG_RETURNBOX(szErr);\
    return;\
}

#define NULLCHK_EFAIL_RETURN(ptr, szErr)\
if (ptr == nullptr)\
{\
    ERRMSGBOX(#ptr " is nullptr!");\
	ERRMSG_EFAIL_RETURNBOX(szErr);\
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
RX::RXLogImpl(PROJECT_MODE::PM_DEBUG, bMessageBox, false, __FILE__, __LINE__, szFormat, __VA_ARGS__)
#else
#define RXLOG(bMessageBox, szFormat, ...)\
RX::RXLogImpl(PROJECT_MODE::PM_RELEASE, bMessageBox, false, _FILE__, __LINE__, szFormat, __VA_ARGS__)
#endif

// ���� ���ڿ� ����, ����� ��忡���� ����� ���â���� ���
// �޽��� �ڽ� ��� ����, ���� �޽��� �ڽ��� �����մϴ�.
#if defined(DEBUG) || defined(_DEBUG)
#define RXERRLOG(bMessageBox, szFormat, ...)\
RX::RXLogImpl(PROJECT_MODE::PM_DEBUG, bMessageBox, true, __FILE__, __LINE__, szFormat, __VA_ARGS__)
#else
#define RXERRLOG(bMessageBox, szFormat, ...)\
RX::RXLogImpl(PROJECT_MODE::PM_RELEASE, bMessageBox, true, __FILE__, __LINE__, szFormat, __VA_ARGS__)
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
// ���� ������ g_hResult�θ� �۵��մϴ�.
// ��ũ�� �Լ��� ���ڸ� �־ ����� ������ ��� �߻��մϴ�.
#if defined(DEBUG) || defined(_DEBUG)
#define DXERR_HANDLER()\
if (FAILED(g_hResult))\
{\
    RX::DXErrorHandlerImpl(DXGetErrorStringA(g_hResult),\
        DXGetErrorDescriptionA(g_hResult), PROJECT_MODE::PM_DEBUG, __FILE__, __LINE__);\
}
#else
#define DXERR_HANDLER()\
if (FAILED(g_hResult))\
{\
    RX::DXErrorHandlerImpl(DXGetErrorStringA(g_hResult),\
        DXGetErrorDescriptionA(g_hResult), PROJECT_MODE::PM_RELEASE, __FILE__, __LINE__);\
}
#endif

#endif
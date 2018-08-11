//////////////////////////////////////////////////////////////////////
// 작성 날짜 : 2018.08.07
// 작성자    : Ruvendix
// 파일 내용 : 공동으로 사용되는 전역 변수 모음입니다.
//////////////////////////////////////////////////////////////////////
#ifndef GLOBAL_VARIABLE_DEFINITION_H__
#define GLOBAL_VARIABLE_DEFINITION_H__

HINSTANCE   g_hInst     = nullptr;
HWND        g_hMainWnd  = nullptr;
HRESULT     g_hDXResult = S_OK;              // 에러 핸들러 변수입니다.

#endif
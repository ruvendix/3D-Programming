//////////////////////////////////////////////////////////////////////
// <작성 날짜>
// 2018.08.07
//
// <작성자>
// Ruvendix
//
// <파일 내용>
// 공동으로 사용되는 전역 변수 모음입니다. 선언만 되어있습니다.
// 실제로 각 프로젝트에서는 이 헤더 파일을 포함해야 합니다.
//////////////////////////////////////////////////////////////////////
#ifndef GLOBAL_VARIABLE_DECLARATION_H__
#define GLOBAL_VARIABLE_DECLARATION_H__

extern HINSTANCE g_hInst;
extern HWND      g_hMainWnd;
extern HRESULT   g_hDXResult; // DirectX 에러 핸들러 변수입니다.

#endif
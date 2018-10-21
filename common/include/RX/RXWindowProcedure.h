/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-08-21
 *
 * <파일 내용>
 * 윈도우 프로시저(메시지 프로시저)들이 정의되어있습니다.
 *
 ====================================================================================*/
#ifndef RXWINDOWPROCEDURE_H_
#define RXWINDOWPROCEDURE_H_

#include "common.h"

void             BaseWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK DefaultWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK DefaultWndProcDX9(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 공통 메시지 핸들러입니다.
void OnCreate();
void OnClose();
void OnDestroy();
void OnKeyEscape();
void OnMouseLButtonDown(LPARAM lParam);
void OnMouseRButtonDown(LPARAM lParam);
void OnMouseMove();
void OnGetMinMaxInfo(LPARAM lParam);

// 윈도우 전용 메시지 핸들러입니다.
void OnMaximize();
void OnAltEnter(WPARAM wParam, LPARAM lParam);

// DirectX9 전용 메시지 핸들러입니다.
void OnKeyF12DX9(); // 스크린샷을 찍습니다.
void OnMaximizeDX9();
void OnAltEnterDX9(WPARAM wParam, LPARAM lParam);

#endif
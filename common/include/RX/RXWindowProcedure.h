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

LRESULT CALLBACK DefaultWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 메시지 핸들러입니다.
void OnCreate();
void OnDestroy();
void OnMouseLButtonDown(LPARAM lParam);
void OnMouseRButtonDown(LPARAM lParam);
void OnMouseMove();
void OnNoneClientHitTest();
void OnEraseBackGround();
void OnClose();
void OnKeyEscape();
void OnKeyF12(); // 스크린샷을 찍습니다.
void OnMaximize();
void OnGetMinMaxInfo(LPARAM lParam);
void OnAltEnter(WPARAM wParam, LPARAM lParam);

#endif
/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <�ۼ� ��¥>
 * 2018-08-21
 *
 * <���� ����>
 * ������ ���ν���(�޽��� ���ν���)���� ���ǵǾ��ֽ��ϴ�.
 *
 ====================================================================================*/
#ifndef RXWINDOWPROCEDURE_H_
#define RXWINDOWPROCEDURE_H_

#include "common.h"

LRESULT CALLBACK DefaultWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// �޽��� �ڵ鷯�Դϴ�.
void OnCreate();
void OnDestroy();
void OnMouseLButtonDown(LPARAM lParam);
void OnMouseRButtonDown(LPARAM lParam);
void OnMouseMove();
void OnNoneClientHitTest();
void OnEraseBackGround();
void OnClose();
void OnKeyEscape();
void OnKeyF12(); // ��ũ������ ����ϴ�.
void OnMaximize();
void OnGetMinMaxInfo(LPARAM lParam);
void OnAltEnter(WPARAM wParam, LPARAM lParam);

#endif
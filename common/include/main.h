/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-10-11
 *
 * <파일 내용>
 * 메인 헤더 파일입니다.
 * 프로젝트의 메인 함수가 있는 소스 파일에만 포함시켜야 합니다.
 *
 ====================================================================================*/
#ifndef MAIN_H__
#define MAIN_H__

// 프로그램의 기본 리소스 헤더 파일입니다.
#include "resource.h"

// Win32 API의 Lib 연결입니다.
#pragma comment(lib, "winmm.lib")
 
// DirectX9 라이브러리 연결입니다
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "DxErr.lib")
 
// Window Kit의 버전이 높아서 링크되지 않는 함수들을
// 링크시켜주기 위해 사용합니다. 예를 들면 "DxErr.h"가 있습니다.
#pragma comment(lib, "legacy_stdio_definitions.lib")
 
// RX 라이브러리 연결입니다.
#pragma comment(lib, "RXBaseDX_Debug.lib")

#endif
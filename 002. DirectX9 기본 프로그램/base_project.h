/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-08-06
 *
 * <파일 내용>
 * 프로젝트 내부에서 공동으로 사용되는 헤더 파일 모음입니다.
 *
 ====================================================================================*/
#ifndef BASE_PROJECT_H__
#define BASE_PROJECT_H__

#include "../common/common.h"

// ====================================================================================
// DirectX9을 사용하기 위한 헤더 파일입니다.
// 프로젝트에 DirectX SDK가 설치된 경로를 설정해줘야 합니다.
#include <d3d9.h>
#include <d3dx9.h>
#include <DxErr.h>

// ====================================================================================
// main 루틴을 사용하기 위한 헤더 파일입니다.
#include "../common/RX/RXMain_DX9.h"

#include "resource.h"

// ====================================================================================
// <라이브러리 연결부입니다.>

// Win32 API의 Lib 연결입니다.
#pragma comment(lib, "winmm.lib")
//
// DirectX9 라이브러리 연결입니다
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "DxErr.lib")
//
// Window Kit의 버전이 높아서 링크되지 않는 함수들을
// 링크시켜주기 위해 사용합니다. 예를 들면 "DxErr.h"가 있습니다.
#pragma comment(lib, "legacy_stdio_definitions.lib")
//
// RX 라이브러리 연결입니다.
#pragma comment(lib, "RXBaseDX_Debug.lib")

#endif
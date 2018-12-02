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

#include "common.h"

// ====================================================================================
// VLD를 사용하기 위한 헤더 파일입니다.
#include <vld.h>

// ====================================================================================
// DirectX9을 사용하기 위한 헤더 파일입니다.
// 프로젝트에 DirectX SDK가 설치된 경로를 설정해줘야 합니다.
#include <d3d9.h>
#include <d3dx9.h>
#include <DxErr.h>

// ====================================================================================
// main 루틴을 사용하기 위한 헤더 파일입니다.
#include "RX/RXMainDX9.h"
#include "RX/RXRendererDX9.h"
#include "RX/RXProgramFPSMgr.h"

// ====================================================================================
// 솔루션 및 프로젝트에서 사용되는 전역변수의 선언을 모아놓은 헤더 파일입니다.
#include "global_variable_declaration.h"

// ====================================================================================
// 그 외 필요한 헤더 파일들입니다.
#include "math_function.h"

#endif
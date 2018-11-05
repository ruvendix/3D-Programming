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

// ====================================================================================
// 구조체 및 공용체 선언부입니다.

// 정점의 형식을 설정합니다.
// P4 => Position4 (X, Y, Z, RHW) 2D 좌표계로 변형된 좌표
// P3 => Position3 (X, Y, Z)      3D 좌표계의 좌표
// D  => Diffuse                  확산(색상)
// S  => Specular                 반사

struct VertexP4D
{
	D3DXVECTOR3 vPos;    // v는 vector의 약자입니다. 공간좌표를 뜻합니다.
	float       rRHW;    // RHW는 Reciprocal Homogeneous W의 약자입니다. 동차좌표를 뜻합니다.
	DWORD       dwColor; // 정점의 색상을 의미합니다. Diffuse는 확산이라는 뜻입니다.
};

struct VertexP3D
{
	D3DXVECTOR3 vPos;    // 위치
	DWORD       dwColor; // 색상
	const static DWORD format = D3DFVF_XYZ | D3DFVF_DIFFUSE; // 형식
};

// 인덱스의 형식을 설정합니다.
struct Index16
{
	WORD index; // 인덱스값
	const static D3DFORMAT format = D3DFMT_INDEX16; // 형식은 2바이트를 사용합니다.
};

extern std::vector<VertexP4D> g_vecP4D;
extern std::vector<VertexP3D> g_vecP3D;
extern std::vector<Index16>   g_vecIndex16;

#endif
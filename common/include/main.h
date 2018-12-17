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

// 프로그램의 기본 리소스 정보 모음입니다.
#include "resource.h"

// Win32 API의 Lib 링크입니다.
#pragma comment(lib, "winmm.lib")
 
// DirectX9 라이브러리 링크입니다
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dx9d.lib")
#pragma comment(lib, "DxErr.lib")
 
// Window Kit의 버전이 높아서 링크되지 않는 함수들을
// 링크시켜주기 위해 사용합니다. 예를 들면 "DxErr.h"가 있습니다.
#pragma comment(lib, "legacy_stdio_definitions.lib")
 
// RX 라이브러리 링크입니다.
#pragma comment(lib, "RXBaseDX_Debug.lib")

// ====================================================================================
// 구조체 및 공용체 선언부입니다.

// 정점의 형식을 설정합니다.
// P4 => Position4 (X, Y, Z, RHW) 2D 좌표계로 변형된 좌표
// P3 => Position3 (X, Y, Z)      3D 좌표계의 좌표
// N  => Normal (X, Y, Z)         법선벡터
// D  => Diffuse                  확산(색상)
// S  => Specular                 반사
// Tn => Texture Num              텍스처 UV 좌표 개수

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

// 광원을 이용하기 위해서는 새로운 정점 형식이 필요합니다.
// 법선벡터와 연산해야 하므로 법선벡터가 필수로 들어가야 합니다.
// 법선벡터가 추가되면 색상과 함께 생각해야 합니다.
// 정점 형식에 색상을 넣게 되면 재질을 따로 설정해주지 않아도 되지만
// 정점 형식에 따로 색상이 없으면 재질을 반드시 설정해줘야 합니다.
struct VertexP3N
{
	D3DXVECTOR3 vPos; // 위치
	D3DXVECTOR3 vN;   // 법선벡터(정규화 필수!)
	const static DWORD format = D3DFVF_XYZ | D3DFVF_NORMAL; // 형식
};

// 3D 좌표와 텍스처 UV 좌표만 있는 형식입니다.
struct VertexP3T1
{
	D3DXVECTOR3 vPos; // 위치
	TextureUV   tex;  // 텍스처 UV 좌표
	const static DWORD format = D3DFVF_XYZ | D3DFVF_TEX1; // 형식
};

// 3D 좌표와 텍스처 UV 좌표만 있는 형식입니다.
struct VertexP3T2
{
	D3DXVECTOR3 vPos; // 위치
	TextureUV   tex1; // 첫번째 텍스처 UV 좌표
	TextureUV   tex2; // 두번째 텍스처 UV 좌표
	const static DWORD format = D3DFVF_XYZ | D3DFVF_TEX2; // 형식 (D3DFVF_TEX1은 넣을 필요 없음!)
};

struct VertexP3ND
{
	D3DXVECTOR3 vPos;    // 위치
	D3DXVECTOR3 vN;      // 법선벡터(정규화 필수!)
	DWORD       dwColor; // 정점의 확산광
	const static DWORD format = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE; // 형식
};

// 인덱스의 형식을 설정합니다.
struct Index16
{
	WORD index; // 인덱스값
	const static D3DFORMAT format = D3DFMT_INDEX16; // 형식은 2바이트를 사용합니다.
};

extern std::vector<VertexP4D>  g_vecP4D;
extern std::vector<VertexP3D>  g_vecP3D;
extern std::vector<VertexP3N>  g_vecP3N;
extern std::vector<VertexP3T1> g_vecP3T1;
extern std::vector<VertexP3T2> g_vecP3T2;
extern std::vector<VertexP3ND> g_vecP3ND;
extern std::vector<Index16>    g_vecIndex16;

#endif
/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <�ۼ� ��¥>
 * 2018-10-11
 *
 * <���� ����>
 * ���� ��� �����Դϴ�.
 * ������Ʈ�� ���� �Լ��� �ִ� �ҽ� ���Ͽ��� ���Խ��Ѿ� �մϴ�.
 *
 ====================================================================================*/
#ifndef MAIN_H__
#define MAIN_H__

// ���α׷��� �⺻ ���ҽ� ��� �����Դϴ�.
#include "resource.h"

// Win32 API�� Lib �����Դϴ�.
#pragma comment(lib, "winmm.lib")
 
// DirectX9 ���̺귯�� �����Դϴ�
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "DxErr.lib")
 
// Window Kit�� ������ ���Ƽ� ��ũ���� �ʴ� �Լ�����
// ��ũ�����ֱ� ���� ����մϴ�. ���� ��� "DxErr.h"�� �ֽ��ϴ�.
#pragma comment(lib, "legacy_stdio_definitions.lib")
 
// RX ���̺귯�� �����Դϴ�.
#pragma comment(lib, "RXBaseDX_Debug.lib")

// ====================================================================================
// ����ü �� ����ü ������Դϴ�.

// ������ ������ �����մϴ�.
// P4 => Position4 (X, Y, Z, RHW) 2D ��ǥ��� ������ ��ǥ
// P3 => Position3 (X, Y, Z)      3D ��ǥ���� ��ǥ
// D  => Diffuse                  Ȯ��(����)
// S  => Specular                 �ݻ�

struct VertexP4D
{
	D3DXVECTOR3 vPos;    // v�� vector�� �����Դϴ�. ������ǥ�� ���մϴ�.
	float       rRHW;    // RHW�� Reciprocal Homogeneous W�� �����Դϴ�. ������ǥ�� ���մϴ�.
	DWORD       dwColor; // ������ ������ �ǹ��մϴ�. Diffuse�� Ȯ���̶�� ���Դϴ�.
};

struct VertexP3D
{
	D3DXVECTOR3 vPos;    // ��ġ
	DWORD       dwColor; // ����
	const static DWORD format = D3DFVF_XYZ | D3DFVF_DIFFUSE; // ����
};

// �ε����� ������ �����մϴ�.
struct Index16
{
	WORD index; // �ε�����
	const static D3DFORMAT format = D3DFMT_INDEX16; // ������ 2����Ʈ�� ����մϴ�.
};

extern std::vector<VertexP4D> g_vecP4D;
extern std::vector<VertexP3D> g_vecP3D;
extern std::vector<Index16>   g_vecIndex16;

#endif
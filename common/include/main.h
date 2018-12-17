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

// ���α׷��� �⺻ ���ҽ� ���� �����Դϴ�.
#include "resource.h"

// Win32 API�� Lib ��ũ�Դϴ�.
#pragma comment(lib, "winmm.lib")
 
// DirectX9 ���̺귯�� ��ũ�Դϴ�
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dx9d.lib")
#pragma comment(lib, "DxErr.lib")
 
// Window Kit�� ������ ���Ƽ� ��ũ���� �ʴ� �Լ�����
// ��ũ�����ֱ� ���� ����մϴ�. ���� ��� "DxErr.h"�� �ֽ��ϴ�.
#pragma comment(lib, "legacy_stdio_definitions.lib")
 
// RX ���̺귯�� ��ũ�Դϴ�.
#pragma comment(lib, "RXBaseDX_Debug.lib")

// ====================================================================================
// ����ü �� ����ü ������Դϴ�.

// ������ ������ �����մϴ�.
// P4 => Position4 (X, Y, Z, RHW) 2D ��ǥ��� ������ ��ǥ
// P3 => Position3 (X, Y, Z)      3D ��ǥ���� ��ǥ
// N  => Normal (X, Y, Z)         ��������
// D  => Diffuse                  Ȯ��(����)
// S  => Specular                 �ݻ�
// Tn => Texture Num              �ؽ�ó UV ��ǥ ����

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

// ������ �̿��ϱ� ���ؼ��� ���ο� ���� ������ �ʿ��մϴ�.
// �������Ϳ� �����ؾ� �ϹǷ� �������Ͱ� �ʼ��� ���� �մϴ�.
// �������Ͱ� �߰��Ǹ� ����� �Բ� �����ؾ� �մϴ�.
// ���� ���Ŀ� ������ �ְ� �Ǹ� ������ ���� ���������� �ʾƵ� ������
// ���� ���Ŀ� ���� ������ ������ ������ �ݵ�� ��������� �մϴ�.
struct VertexP3N
{
	D3DXVECTOR3 vPos; // ��ġ
	D3DXVECTOR3 vN;   // ��������(����ȭ �ʼ�!)
	const static DWORD format = D3DFVF_XYZ | D3DFVF_NORMAL; // ����
};

// 3D ��ǥ�� �ؽ�ó UV ��ǥ�� �ִ� �����Դϴ�.
struct VertexP3T1
{
	D3DXVECTOR3 vPos; // ��ġ
	TextureUV   tex;  // �ؽ�ó UV ��ǥ
	const static DWORD format = D3DFVF_XYZ | D3DFVF_TEX1; // ����
};

// 3D ��ǥ�� �ؽ�ó UV ��ǥ�� �ִ� �����Դϴ�.
struct VertexP3T2
{
	D3DXVECTOR3 vPos; // ��ġ
	TextureUV   tex1; // ù��° �ؽ�ó UV ��ǥ
	TextureUV   tex2; // �ι�° �ؽ�ó UV ��ǥ
	const static DWORD format = D3DFVF_XYZ | D3DFVF_TEX2; // ���� (D3DFVF_TEX1�� ���� �ʿ� ����!)
};

struct VertexP3ND
{
	D3DXVECTOR3 vPos;    // ��ġ
	D3DXVECTOR3 vN;      // ��������(����ȭ �ʼ�!)
	DWORD       dwColor; // ������ Ȯ�걤
	const static DWORD format = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE; // ����
};

// �ε����� ������ �����մϴ�.
struct Index16
{
	WORD index; // �ε�����
	const static D3DFORMAT format = D3DFMT_INDEX16; // ������ 2����Ʈ�� ����մϴ�.
};

extern std::vector<VertexP4D>  g_vecP4D;
extern std::vector<VertexP3D>  g_vecP3D;
extern std::vector<VertexP3N>  g_vecP3N;
extern std::vector<VertexP3T1> g_vecP3T1;
extern std::vector<VertexP3T2> g_vecP3T2;
extern std::vector<VertexP3ND> g_vecP3ND;
extern std::vector<Index16>    g_vecIndex16;

#endif
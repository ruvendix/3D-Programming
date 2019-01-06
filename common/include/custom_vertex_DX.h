/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <�ۼ� ��¥>
 * 2019-01-03
 *
 * <���� ����>
 * DirectX�� ���� ������ ���������ο��� ����� �� �ִ� ���� ���� �����Դϴ�.
 *
 ====================================================================================*/
#ifndef CUSTOM_VERTEX_DX_H__
#define CUSTOM_VERTEX_DX_H__

#include <d3d9.h>
#include <d3dx9.h>

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

#endif
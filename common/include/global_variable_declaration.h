/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <�ۼ� ��¥>
 * 2018-08-07
 *
 * <���� ����>
 * �������� ���Ǵ� ���� ���� �����Դϴ�. ���� �Ǿ��ֽ��ϴ�.
 * ������ �� ������Ʈ������ �� ��� ������ �����ؾ� �մϴ�.
 *
 ====================================================================================*/
#ifndef GLOBAL_VARIABLE_DECLARATION_H__
#define GLOBAL_VARIABLE_DECLARATION_H__

extern HWND g_hMainWnd;
extern HDC  g_hMainDC;

extern HBRUSH  g_hHighlightBrush;
extern HDC     g_hBackBufferDC;
extern HBITMAP g_hBackBufferBitmap;
extern HBITMAP g_hOldBackBufferBitmap;

extern IDirect3DDevice9* g_pD3DDevice9;
extern HRESULT           g_DXResult;

extern RX::RXMain_DX9*         g_pMainDX;
extern IDirect3DVertexBuffer9* g_pVertexBuffer;
extern IDirect3DIndexBuffer9*  g_pIndexBuffer;
extern D3DXVECTOR3             g_vBaseVertex[8];

extern ID3DXLine*  g_pLine; // ���� �׸��� ���� ��
extern D3DXVECTOR3 g_vCubeTriangleNormal[12]; // �ﰢ�������� ��������

extern D3DXMATRIXA16 g_matViewAndProjection; // �̸� ����ص� ����� * �������
extern D3DXMATRIXA16 g_matProjection;        // �̸� ����ص� �������
extern D3DXMATRIXA16 g_matAll; // ������İ� �����ϱ� ���� ��ü��ȯ���

extern DWORD g_dwNormalVectorRenderingFlag;

#endif
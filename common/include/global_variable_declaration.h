/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-08-07
 *
 * <파일 내용>
 * 공동으로 사용되는 전역 변수 모음입니다. 선언만 되어있습니다.
 * 실제로 각 프로젝트에서는 이 헤더 파일을 포함해야 합니다.
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

extern ID3DXLine*  g_pLine; // 선을 그리기 위한 것
extern D3DXVECTOR3 g_vCubeTriangleNormal[12]; // 삼각형에서의 법선벡터

extern D3DXMATRIXA16 g_matViewAndProjection; // 미리 계산해둔 뷰행렬 * 투영행렬
extern D3DXMATRIXA16 g_matProjection;        // 미리 계산해둔 투영행렬
extern D3DXMATRIXA16 g_matAll; // 월드행렬과 결합하기 위한 전체변환행렬

extern DWORD g_dwNormalVectorRenderingFlag;

#endif
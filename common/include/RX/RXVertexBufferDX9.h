/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-10-11
 *
 * <파일 내용>
 * DirectX9의 정점 버퍼를 클래스로 래핑했습니다.
 *
 ====================================================================================*/
#ifndef RXVERTEXBUFFERDX9_H__
#define RXVERTEXBUFFERDX9_H__

#include "common.h"

// 정점의 구조를 설정합니다.
struct VertexInfo
{
	D3DXVECTOR3 vPos;    // 정점의 좌표입니다.
	DWORD       dwColor; // 정점의 색상입니다.
	static const DWORD FORMAT = D3DFVF_XYZ | D3DFVF_DIFFUSE;
};

// 인덱스의 정보를 설정합니다.
struct IndexInfo
{
	WORD triangleIndices[3];
	static const D3DFORMAT FORMAT = D3DFMT_INDEX16; // 인덱스 형식은 2바이트입니다.
};

namespace RX
{

	class DLL_DEFINE RXVertexBufferDX9
	{
	public:
		RXVertexBufferDX9();
		virtual ~RXVertexBufferDX9();

		void    InsertVertex(FLOAT rX, FLOAT rY, FLOAT rZ, DWORD dwColor);
		void    InsertVertex(const D3DXVECTOR3& vPos, DWORD dwColor);
		void    InsertIndex(WORD first, WORD second, WORD third);
		void    DrawPrimitive(D3DPRIMITIVETYPE primitiveType);
		void    DrawIndexedPrimitive(); // D3DPT_TRIANGLELIST 고정입니다.

		HRESULT CreateVertexBuffer();
		HRESULT CreateIndexBuffer(INT32 triangleCnt);

		// ====================================================================================
		// Getter
		INT32 getVertexCount() const noexcept
		{
			return (m_vecVertex.size());
		}

		// ====================================================================================
		// Setter

	private:
		INT32                      m_vertexCnt;
		INT32                      m_triangleCnt;
		std::vector<VertexInfo>    m_vecVertex;
		std::vector<IndexInfo>     m_vecIndex;
		IDirect3DVertexBuffer9*    m_pVB;
		IDirect3DIndexBuffer9*     m_pIB;
	};

} // namespace RX end
#endif
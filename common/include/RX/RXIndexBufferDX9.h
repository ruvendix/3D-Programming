/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-10-15
 *
 * <파일 내용>
 * DirectX9의 인덱스 버퍼를 클래스로 래핑했습니다.
 *
 ====================================================================================*/
#ifndef RXINDEXBUFFERDX9_H__
#define RXINDEXBUFFERDX9_H__

#include "common.h"

// 인덱스의 구조를 설정합니다.
struct IndexInfo
{
	WORD triangleIndices[3];
	static const D3DFORMAT FORMAT = D3DFMT_INDEX16; // 인덱스 형식은 2바이트입니다.
};

namespace RX
{

	class DLL_DEFINE RXIndexBufferDX9
	{
	public:
		RXIndexBufferDX9();
		virtual ~RXIndexBufferDX9();

		void    InsertIndex(WORD first, WORD second, WORD third);
		HRESULT CreateIndexBuffer(INT32 triangleCnt);

		// ====================================================================================
		// Getter
		IDirect3DIndexBuffer9* getIB() const noexcept
		{
			return m_pIB;
		}

		INT32 getTriangleCount() const noexcept
		{
			return m_triangleCnt;
		}

		INT32 getIndexCount() const noexcept
		{
			return m_indexCnt;
		}

	private:
		INT32                     m_triangleCnt;
		INT32                     m_indexCnt;
		std::vector<IndexInfo>    m_vecIndex;
		IDirect3DIndexBuffer9*    m_pIB;
	};

} // namespace RX end
#endif
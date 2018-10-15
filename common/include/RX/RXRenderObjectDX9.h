/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-10-15
 *
 * <파일 내용>
 * 렌더링되는 오브젝트의 기본 클래스입니다.
 *
 ====================================================================================*/
#ifndef RXRENDEROBJECTDX9_H__
#define RXRENDEROBJECTDX9_H__

#include "common.h"
#include "RXVertexBufferDX9.h"

namespace RX
{

	class DLL_DEFINE RXRenderObjectDX9
	{
	public:
		RXRenderObjectDX9();
		virtual ~RXRenderObjectDX9();

		void AllocVertexBuffer();
		void AllocIndexBuffer();
		void CreateVertexBuffer();
		void CreateIndexBuffer(INT32 triangleCnt);
		void InsertVertex(FLOAT rX, FLOAT rY, FLOAT rZ, DWORD dwColor);
		void InsertVertex(const D3DXVECTOR3& vPos, DWORD dwColor);
		void InsertIndex(WORD first, WORD second, WORD third);

		HRESULT DrawPrimitive();
		HRESULT DrawIndexedPrimitive();

		// ====================================================================================
		// Setter
		void setPrimitiveType(D3DPRIMITIVETYPE primitiveType)
		{
			m_primitiveType = primitiveType;
		}

	private:
		RX::RXVertexBufferDX9* m_pVB;
		RX::RXIndexBufferDX9*  m_pIB;
		D3DPRIMITIVETYPE       m_primitiveType;
	};

} // namespace RX end
#endif
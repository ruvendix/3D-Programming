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
#include "PCH.h"
#include "RXRenderObjectDX9.h"

namespace RX
{
	RXRenderObjectDX9::RXRenderObjectDX9()
	{
		m_pVB = nullptr;
		m_pIB = nullptr;
		m_primitiveType = D3DPT_TRIANGLELIST;
	}

	RXRenderObjectDX9::~RXRenderObjectDX9()
	{
		SAFE_DELTE(m_pVB);
		SAFE_DELTE(m_pIB);
	}

	void RXRenderObjectDX9::AllocVertexBuffer()
	{
		m_pVB = RXNew RXVertexBufferDX9;
		NULLCHK_HEAPALLOC(m_pVB);
	}

	void RXRenderObjectDX9::AllocIndexBuffer()
	{
		m_pIB = RXNew RXIndexBufferDX9;
		NULLCHK_HEAPALLOC(m_pIB);
	}

	void RXRenderObjectDX9::CreateVertexBuffer()
	{
		NULLCHK_RETURN_NOCOMENT(m_pVB);
		if (m_pVB->getVertexCount() == 0)
		{
			RXERRLOG_RETURN("정점이 0개입니다!");
		}

		m_pVB->CreateVertexBuffer();
	}

	void RXRenderObjectDX9::CreateIndexBuffer(INT32 triangleCnt)
	{
		NULLCHK_RETURN_NOCOMENT(m_pIB);
		if (triangleCnt == 0)
		{
			RXERRLOG_RETURN("폴리곤 렌더링이 불가능합니다!");
		}

		m_pIB->CreateIndexBuffer(triangleCnt);
	}

	void RXRenderObjectDX9::InsertVertex(FLOAT rX, FLOAT rY, FLOAT rZ, DWORD dwColor)
	{
		NULLCHK_RETURN_NOCOMENT(m_pVB);
		m_pVB->InsertVertex(rX, rY, rZ, dwColor);
	}

	void RXRenderObjectDX9::InsertVertex(const D3DXVECTOR3& vPos, DWORD dwColor)
	{
		NULLCHK_RETURN_NOCOMENT(m_pVB);
		m_pVB->InsertVertex(vPos, dwColor);
	}

	void RXRenderObjectDX9::InsertIndex(WORD first, WORD second, WORD third)
	{
		NULLCHK_RETURN_NOCOMENT(m_pIB);
		m_pIB->InsertIndex(first, second, third);
	}

	HRESULT RXRenderObjectDX9::DrawPrimitive()
	{
		NULLCHK_RETURN_EFAIL_NOCOMENT(m_pVB);
		RXRendererDX9::Instance()->DrawPrimitive(m_primitiveType, *m_pVB);
		return S_OK;
	}

	HRESULT RXRenderObjectDX9::DrawIndexedPrimitive()
	{
		NULLCHK_RETURN_EFAIL_NOCOMENT(m_pVB);
		NULLCHK_RETURN_EFAIL_NOCOMENT(m_pIB);
		RXRendererDX9::Instance()->DrawIndexedPrimitive(*m_pVB, *m_pIB);
		return E_NOTIMPL;
	}

} // namespace RX end
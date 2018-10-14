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
#include "PCH.h"
#include "RXVertexBufferDX9.h"

namespace
{
	INT32 CalcPrimitiveCount(D3DPRIMITIVETYPE primitiveType, INT32 vertexCnt)
	{
		INT32 primitiveCnt = 0;
		switch (primitiveType)
		{
		case D3DPT_POINTLIST:
			primitiveCnt = vertexCnt;
			break;
		case D3DPT_LINELIST:
			primitiveCnt = vertexCnt / 2;
			break;
		case D3DPT_LINESTRIP:
			primitiveCnt = vertexCnt - 1;
			break;
		case D3DPT_TRIANGLELIST:
			primitiveCnt = vertexCnt / 3;
			break;
		case D3DPT_TRIANGLESTRIP:
			primitiveCnt = vertexCnt - 2;
			break;
		case D3DPT_TRIANGLEFAN:
			primitiveCnt = vertexCnt - 2;
			break;
		}

		return primitiveCnt;
	}
}

namespace RX
{

	RXVertexBufferDX9::RXVertexBufferDX9()
	{
		m_vertexCnt = 0;
		m_pVB       = nullptr;
		m_pIB       = nullptr;
	}

	RXVertexBufferDX9::~RXVertexBufferDX9()
	{
		SAFE_RELEASE(m_pIB);
		SAFE_RELEASE(m_pVB);
	}

	void RXVertexBufferDX9::InsertVertex(FLOAT rX, FLOAT rY, FLOAT rZ, DWORD dwColor)
	{
		VertexInfo vertexInfo = { { rX, rY, rZ }, dwColor };
		m_vecVertex.push_back(vertexInfo);
	}

	void RXVertexBufferDX9::InsertVertex(const D3DXVECTOR3& vPos, DWORD dwColor)
	{
		VertexInfo vertexInfo = { { vPos.x, vPos.y, vPos.z }, dwColor };
		m_vecVertex.push_back(vertexInfo);
	}

	void RXVertexBufferDX9::InsertIndex(WORD first, WORD second, WORD third)
	{
		IndexInfo indexInfo = { first, second, third };
		m_vecIndex.push_back(indexInfo);
	}

	void RXVertexBufferDX9::DrawPrimitive(D3DPRIMITIVETYPE primitiveType)
	{
		g_pD3DDevice9->SetFVF(VertexInfo::FORMAT);
		g_pD3DDevice9->SetStreamSource(
			0,                   // 스트림 넘버는 0으로 설정합니다.
			m_pVB,               // 정점 버퍼를 설정해줍니다.
			0,                   // 오프셋은 0으로 설정합니다.
			sizeof(VertexInfo)); // 보폭(Stride)은 FVF로 생성한 크기와 일치해야 합니다.

		g_pD3DDevice9->DrawPrimitive(
			primitiveType, // 렌더링 형식을 설정합니다.
			0,             // 오프셋은 0으로 설정합니다.
			CalcPrimitiveCount(primitiveType, m_vecVertex.size())); // 프리미티브 개수입니다.
	}

	void RXVertexBufferDX9::DrawIndexedPrimitive()
	{
		NULL_RETURN(m_pIB); // 인덱스 버퍼가 존재할 때만 처리됩니다.

		g_pD3DDevice9->SetFVF(VertexInfo::FORMAT);
		g_pD3DDevice9->SetStreamSource(
			0,                   // 스트림 넘버는 0으로 설정합니다.
			m_pVB,               // 정점 버퍼를 설정해줍니다.
			0,                   // 오프셋은 0으로 설정합니다.
			sizeof(VertexInfo)); // 보폭(Stride)은 FVF로 생성한 크기와 일치해야 합니다.

		// 인덱스 버퍼를 가상 디바이스에 적용해줍니다.
		g_pD3DDevice9->SetIndices(m_pIB);

		g_pD3DDevice9->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST, // 렌더링 형식을 설정합니다.
			0,                  // 정점 버퍼에서 시작할 정점 인덱스를 설정합니다. (0으로 설정)
			0,                  // 시작할 인덱스를 설정합니다. (0으로 설정)
			m_vertexCnt,        // 정점 개수를 설정합니다.
			0,                  // 인덱스 버퍼의 오프셋입니다. (0으로 설정)
			m_triangleCnt);     // 프리미티브 개수입니다.

		// 인덱스 버퍼의 프리미티브 개수는 기본 설정일 때
		// D3DPT_TRIANGLESTRIP와 동일합니다.
	}

	HRESULT RXVertexBufferDX9::CreateVertexBuffer()
	{
		m_vertexCnt = m_vecVertex.size();

		// 정점 버퍼를 생성합니다.
		g_DXResult = g_pD3DDevice9->CreateVertexBuffer(
			sizeof(VertexInfo) * m_vertexCnt, // 정점 버퍼의 크기입니다.
			D3DUSAGE_NONE,                    // Usage는 0으로 설정합니다.
			VertexInfo::FORMAT,               // FVF 형식입니다.
			D3DPOOL_MANAGED,                  // 메모리풀입니다.
			&m_pVB,                           // 정점 버퍼의 포인터입니다.
			nullptr);                         // nullptr로 설정합니다.

		DXERR_HANDLER(g_DXResult);
		NULLCHK_EFAIL_RETURN(m_pVB, "정점 버퍼 초기화 실패!");

		// 정점 버퍼에 실제로 정점 정보를 복사합니다.
		// 메모리에 접근하기 때문에 메모리를 잠그고 푸는 과정이 있습니다.
		void* pVertexData = nullptr;
		m_pVB->Lock(
			0,                                // 오프셋이 0이면 전체 잠금입니다.
			sizeof(VertexInfo) * m_vertexCnt, // 복사할 정점 정보의 크기를 넘겨줍니다.
			&pVertexData,                     // 복사된 정점 정보를 다룰 수 있는 포인터를 설정해줍니다.
			D3DFLAG_NONE);                    // 잠금 플래그는 0으로 설정합니다.

		::CopyMemory(pVertexData, &m_vecVertex[0], sizeof(VertexInfo) * m_vertexCnt);
		m_pVB->Unlock();
		
		return S_OK;
	}

	HRESULT RXVertexBufferDX9::CreateIndexBuffer(INT32 triangleCnt)
	{
		m_triangleCnt = triangleCnt;

		// 인덱스 버퍼를 생성합니다.
		g_DXResult = g_pD3DDevice9->CreateIndexBuffer(
			sizeof(IndexInfo) * m_triangleCnt, // 인덱스의 크기입니다.
			D3DUSAGE_NONE,                     // Usage는 0으로 설정합니다.
			IndexInfo::FORMAT,                 // 인덱스 버퍼 형식입니다. (D3DFMT_INDEX16)
			D3DPOOL_MANAGED,                   // 메모리풀입니다.
			&m_pIB,                            // 인덱스 버퍼의 포인터입니다.
			nullptr);                          // nullptr로 설정합니다.
			
		DXERR_HANDLER(g_DXResult);
		NULLCHK_EFAIL_RETURN(m_pIB, "인덱스 버퍼 초기화 실패!");

		// 인덱스 버퍼에 실제로 인덱스 정보를 복사합니다.
		// 메모리에 접근하기 때문에 메모리를 잠그고 푸는 과정이 있습니다.
		void* pIndexData = nullptr;
		m_pIB->Lock(
			0,                                 // 오프셋이 0이면 전체 잠금입니다.
			sizeof(IndexInfo) * m_triangleCnt, // 복사할 인덱스 정보의 크기를 넘겨줍니다.
			&pIndexData,                       // 복사된 인덱스 정보를 다룰 수 있는 포인터를 설정해줍니다.
			D3DFLAG_NONE);                     // 잠금 플래그는 0으로 설정합니다.

		::CopyMemory(pIndexData, &m_vecIndex[0], sizeof(IndexInfo) * m_triangleCnt);
		m_pIB->Unlock();

		return S_OK;
	}

} // namespace RX end
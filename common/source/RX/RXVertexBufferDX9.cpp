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

namespace RX
{

	RXVertexBufferDX9::RXVertexBufferDX9()
	{
		m_pVB = nullptr;
	}

	RXVertexBufferDX9::~RXVertexBufferDX9()
	{
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

	HRESULT RXVertexBufferDX9::CreateVertexBuffer()
	{
		m_vertexCnt = m_vecVertex.size();

		// 정점 버퍼를 생성합니다.
		g_DXResult = g_pD3DDevice9->CreateVertexBuffer(
			sizeof(VertexInfo) * m_vertexCnt, // 정점 버퍼의 크기입니다.
			D3DUSAGE_NONE,                   // Usage는 0으로 설정합니다.
			VertexInfo::FORMAT,              // FVF 형식입니다.
			D3DPOOL_MANAGED,                 // 메모리풀입니다.
			&m_pVB,                          // 정점 버퍼의 포인터입니다.
			nullptr);                        // nullptr로 설정합니다.

		DXERR_HANDLER(g_DXResult);
		NULLCHK_RETURN_EFAIL(m_pVB, "정점 버퍼 초기화 실패!");
		
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

} // namespace RX end
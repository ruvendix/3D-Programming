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
#include "base_project.h"
#include "RXVertexBufferDX9_test.h"

namespace RX
{

	RXVertexBufferDX9::RXVertexBufferDX9()
	{
		m_dwFVF = 0;
		m_pVertexBuffer = nullptr;
	}

	RXVertexBufferDX9::~RXVertexBufferDX9()
	{
		SAFE_RELEASE(m_pVertexBuffer);
	}

	void RXVertexBufferDX9::InsertVertex(FLOAT rX, FLOAT rY, FLOAT rZ, DWORD dwColor)
	{
		CustomVertex customVertex = { { rX, rY, rZ }, dwColor };
		m_vecVertex.push_back(customVertex);
	}

	void RXVertexBufferDX9::DrawPrimitive()
	{
		g_pD3DDevice9->SetFVF(m_dwFVF);
		g_pD3DDevice9->SetStreamSource(
			0,                     // 스트림 넘버는 0으로 설정합니다.
			m_pVertexBuffer,       // 정점 버퍼를 설정해줍니다.
			0,                     // 오프셋은 0으로 설정합니다.
			sizeof(CustomVertex)); // 보폭(Stride)은 FVF로 생성한 크기와 일치해야 합니다.

		g_pD3DDevice9->DrawPrimitive(
			D3DPT_TRIANGLELIST,      // 렌더링 형식을 설정합니다.
			0,                       // 오프셋은 0으로 설정합니다.
			m_vecVertex.size() / 3); // 프리미티브 개수입니다.
	}

	HRESULT RXVertexBufferDX9::CreateVertexBuffer()
	{
		INT32 vertexCnt = m_vecVertex.size();

		// 정점 버퍼를 생성합니다.
		g_hDXResult = g_pD3DDevice9->CreateVertexBuffer(
			sizeof(CustomVertex) * vertexCnt, // 정점 버퍼의 크기입니다.
			0,                                // Usage는 0으로 설정합니다.
			m_dwFVF,                          // FVF 형식입니다.
			D3DPOOL_MANAGED,                  // 메모리풀입니다.
			&m_pVertexBuffer,                 // 정점 버퍼의 포인터입니다.
			nullptr);                         // nullptr로 설정합니다.

		DXERR_HANDLER(g_hDXResult);
		NULLCHK_EFAIL_RETURN(m_pVertexBuffer, "정점 버퍼 초기화 실패!");

		// 정점 버퍼에 실제로 정점 정보를 복사합니다.
		// 메모리에 접근하기 때문에 메모리를 잠그고 푸는 과정이 있습니다.
		void* pVertexData = nullptr;
		m_pVertexBuffer->Lock(
			0,                                // 오프셋이 0이면 전체 잠금입니다.
			sizeof(CustomVertex) * vertexCnt, // 복사할 정점 정보의 크기를 넘겨줍니다.
			&pVertexData,                     // 복사된 정점 정보를 다룰 수 있는 포인터를 설정해줍니다.
			0);                               // 잠금 플래그는 0으로 설정합니다.

		::CopyMemory(pVertexData, &m_vecVertex[0], sizeof(CustomVertex) * vertexCnt);
		m_pVertexBuffer->Unlock();

		return S_OK;
	}

} // namespace RX end
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
#include "PCH.h"
#include "RXIndexBufferDX9.h"

namespace RX
{

	RXIndexBufferDX9::RXIndexBufferDX9()
	{
		m_triangleCnt = 0;
		m_indexCnt    = 0;
		m_pIB         = nullptr;
	}

	RXIndexBufferDX9::~RXIndexBufferDX9()
	{
		SAFE_RELEASE(m_pIB);
	}

	void RXIndexBufferDX9::InsertIndex(WORD first, WORD second, WORD third)
	{
		IndexInfo indexInfo = { first, second, third };
		m_vecIndex.push_back(indexInfo);
	}

	HRESULT RXIndexBufferDX9::CreateIndexBuffer(INT32 triangleCnt)
	{
		m_indexCnt    = m_vecIndex.size();
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
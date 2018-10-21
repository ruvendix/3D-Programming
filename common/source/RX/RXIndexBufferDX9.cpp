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
		m_IdxCnt    = 0;
		m_pIB         = nullptr;
	}

	RXIndexBufferDX9::~RXIndexBufferDX9()
	{
		SAFE_RELEASE(m_pIB);
	}

	void RXIndexBufferDX9::InsertIndex(WORD first, WORD second, WORD third)
	{
		IdxInfo IdxInfo = { first, second, third };
		m_vecIdx.push_back(IdxInfo);
	}

	HRESULT RXIndexBufferDX9::CreateIndexBuffer(INT32 triangleCnt)
	{
		m_IdxCnt    = m_vecIdx.size();
		m_triangleCnt = triangleCnt;

		// 인덱스 버퍼를 생성합니다.
		g_DXResult = g_pD3DDevice9->CreateIndexBuffer(
			sizeof(IdxInfo) * m_triangleCnt, // 인덱스의 크기입니다.
			D3DUSAGE_NONE,                     // Usage는 0으로 설정합니다.
			IdxInfo::FORMAT,                 // 인덱스 버퍼 형식입니다. (D3DFMT_Idx16)
			D3DPOOL_MANAGED,                   // 메모리풀입니다.
			&m_pIB,                            // 인덱스 버퍼의 포인터입니다.
			nullptr);                          // nullptr로 설정합니다.

		DXERR_HANDLER(g_DXResult);
		NULLCHK_RETURN_EFAIL(m_pIB, "인덱스 버퍼 초기화 실패!");

		// 인덱스 버퍼에 실제로 인덱스 정보를 복사합니다.
		// 메모리에 접근하기 때문에 메모리를 잠그고 푸는 과정이 있습니다.
		void* pIdxData = nullptr;
		m_pIB->Lock(
			0,                                 // 오프셋이 0이면 전체 잠금입니다.
			sizeof(IdxInfo) * m_triangleCnt, // 복사할 인덱스 정보의 크기를 넘겨줍니다.
			&pIdxData,                       // 복사된 인덱스 정보를 다룰 수 있는 포인터를 설정해줍니다.
			D3DFLAG_NONE);                     // 잠금 플래그는 0으로 설정합니다.

		::CopyMemory(pIdxData, &m_vecIdx[0], sizeof(IdxInfo) * m_triangleCnt);
		m_pIB->Unlock();

		return S_OK;
	}

} // namespace RX end
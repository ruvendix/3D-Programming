/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <�ۼ� ��¥>
 * 2018-10-15
 *
 * <���� ����>
 * DirectX9�� �ε��� ���۸� Ŭ������ �����߽��ϴ�.
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

		// �ε��� ���۸� �����մϴ�.
		g_DXResult = g_pD3DDevice9->CreateIndexBuffer(
			sizeof(IndexInfo) * m_triangleCnt, // �ε����� ũ���Դϴ�.
			D3DUSAGE_NONE,                     // Usage�� 0���� �����մϴ�.
			IndexInfo::FORMAT,                 // �ε��� ���� �����Դϴ�. (D3DFMT_INDEX16)
			D3DPOOL_MANAGED,                   // �޸�Ǯ�Դϴ�.
			&m_pIB,                            // �ε��� ������ �������Դϴ�.
			nullptr);                          // nullptr�� �����մϴ�.

		DXERR_HANDLER(g_DXResult);
		NULLCHK_EFAIL_RETURN(m_pIB, "�ε��� ���� �ʱ�ȭ ����!");

		// �ε��� ���ۿ� ������ �ε��� ������ �����մϴ�.
		// �޸𸮿� �����ϱ� ������ �޸𸮸� ��װ� Ǫ�� ������ �ֽ��ϴ�.
		void* pIndexData = nullptr;
		m_pIB->Lock(
			0,                                 // �������� 0�̸� ��ü ����Դϴ�.
			sizeof(IndexInfo) * m_triangleCnt, // ������ �ε��� ������ ũ�⸦ �Ѱ��ݴϴ�.
			&pIndexData,                       // ����� �ε��� ������ �ٷ� �� �ִ� �����͸� �������ݴϴ�.
			D3DFLAG_NONE);                     // ��� �÷��״� 0���� �����մϴ�.

		::CopyMemory(pIndexData, &m_vecIndex[0], sizeof(IndexInfo) * m_triangleCnt);
		m_pIB->Unlock();

		return S_OK;
	}

} // namespace RX end
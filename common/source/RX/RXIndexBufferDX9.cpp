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

		// �ε��� ���۸� �����մϴ�.
		g_DXResult = g_pD3DDevice9->CreateIndexBuffer(
			sizeof(IdxInfo) * m_triangleCnt, // �ε����� ũ���Դϴ�.
			D3DUSAGE_NONE,                     // Usage�� 0���� �����մϴ�.
			IdxInfo::FORMAT,                 // �ε��� ���� �����Դϴ�. (D3DFMT_Idx16)
			D3DPOOL_MANAGED,                   // �޸�Ǯ�Դϴ�.
			&m_pIB,                            // �ε��� ������ �������Դϴ�.
			nullptr);                          // nullptr�� �����մϴ�.

		DXERR_HANDLER(g_DXResult);
		NULLCHK_RETURN_EFAIL(m_pIB, "�ε��� ���� �ʱ�ȭ ����!");

		// �ε��� ���ۿ� ������ �ε��� ������ �����մϴ�.
		// �޸𸮿� �����ϱ� ������ �޸𸮸� ��װ� Ǫ�� ������ �ֽ��ϴ�.
		void* pIdxData = nullptr;
		m_pIB->Lock(
			0,                                 // �������� 0�̸� ��ü ����Դϴ�.
			sizeof(IdxInfo) * m_triangleCnt, // ������ �ε��� ������ ũ�⸦ �Ѱ��ݴϴ�.
			&pIdxData,                       // ����� �ε��� ������ �ٷ� �� �ִ� �����͸� �������ݴϴ�.
			D3DFLAG_NONE);                     // ��� �÷��״� 0���� �����մϴ�.

		::CopyMemory(pIdxData, &m_vecIdx[0], sizeof(IdxInfo) * m_triangleCnt);
		m_pIB->Unlock();

		return S_OK;
	}

} // namespace RX end
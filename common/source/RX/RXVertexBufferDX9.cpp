/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <�ۼ� ��¥>
 * 2018-10-11
 *
 * <���� ����>
 * DirectX9�� ���� ���۸� Ŭ������ �����߽��ϴ�.
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

		// ���� ���۸� �����մϴ�.
		g_DXResult = g_pD3DDevice9->CreateVertexBuffer(
			sizeof(VertexInfo) * m_vertexCnt, // ���� ������ ũ���Դϴ�.
			D3DUSAGE_NONE,                   // Usage�� 0���� �����մϴ�.
			VertexInfo::FORMAT,              // FVF �����Դϴ�.
			D3DPOOL_MANAGED,                 // �޸�Ǯ�Դϴ�.
			&m_pVB,                          // ���� ������ �������Դϴ�.
			nullptr);                        // nullptr�� �����մϴ�.

		DXERR_HANDLER(g_DXResult);
		NULLCHK_RETURN_EFAIL(m_pVB, "���� ���� �ʱ�ȭ ����!");
		
		// ���� ���ۿ� ������ ���� ������ �����մϴ�.
		// �޸𸮿� �����ϱ� ������ �޸𸮸� ��װ� Ǫ�� ������ �ֽ��ϴ�.
		void* pVertexData = nullptr;
		m_pVB->Lock(
			0,                                // �������� 0�̸� ��ü ����Դϴ�.
			sizeof(VertexInfo) * m_vertexCnt, // ������ ���� ������ ũ�⸦ �Ѱ��ݴϴ�.
			&pVertexData,                     // ����� ���� ������ �ٷ� �� �ִ� �����͸� �������ݴϴ�.
			D3DFLAG_NONE);                    // ��� �÷��״� 0���� �����մϴ�.

		::CopyMemory(pVertexData, &m_vecVertex[0], sizeof(VertexInfo) * m_vertexCnt);
		m_pVB->Unlock();
		
		return S_OK;
	}

} // namespace RX end
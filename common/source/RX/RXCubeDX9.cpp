/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <�ۼ� ��¥>
 * 2018-10-14
 *
 * <���� ����>
 * �׽�Ʈ�� ť�긦 �ٷ�ϴ�.
 *
 ====================================================================================*/
#include "PCH.h"
#include "RXCubeDX9.h"

namespace RX
{

	HRESULT RXCubeDX9::CreateCubeByVertex(const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax)
	{
		InitBaseVertex(vMin, vMax);
		AllocVertexBuffer();

		// ===============================================
		// ���� �ﰢ�� 2��
		InsertVertex(m_vBase[0], DXCOLOR_RED);
		InsertVertex(m_vBase[1], DXCOLOR_GREEN);
		InsertVertex(m_vBase[5], DXCOLOR_BLUE);

		InsertVertex(m_vBase[0], DXCOLOR_RED);
		InsertVertex(m_vBase[5], DXCOLOR_GREEN);
		InsertVertex(m_vBase[4], DXCOLOR_BLUE);

		// ===============================================
		// ���� ���� �ﰢ�� 2��
		InsertVertex(m_vBase[0], DXCOLOR_RED);
		InsertVertex(m_vBase[4], DXCOLOR_GREEN);
		InsertVertex(m_vBase[3], DXCOLOR_BLUE);
					
		InsertVertex(m_vBase[3], DXCOLOR_RED);
		InsertVertex(m_vBase[4], DXCOLOR_GREEN);
		InsertVertex(m_vBase[7], DXCOLOR_BLUE);

		// ===============================================
		// ������ ���� �ﰢ�� 2��
		InsertVertex(m_vBase[1], DXCOLOR_RED);
		InsertVertex(m_vBase[2], DXCOLOR_GREEN);
		InsertVertex(m_vBase[5], DXCOLOR_BLUE);

		InsertVertex(m_vBase[2], DXCOLOR_RED);
		InsertVertex(m_vBase[6], DXCOLOR_GREEN);
		InsertVertex(m_vBase[5], DXCOLOR_BLUE);

		// ===============================================
		// ���� �ﰢ�� 2��
		InsertVertex(m_vBase[0], DXCOLOR_RED);
		InsertVertex(m_vBase[3], DXCOLOR_GREEN);
		InsertVertex(m_vBase[2], DXCOLOR_BLUE);

		InsertVertex(m_vBase[0], DXCOLOR_RED);
		InsertVertex(m_vBase[2], DXCOLOR_GREEN);
		InsertVertex(m_vBase[1], DXCOLOR_BLUE);

		// ===============================================
		// �Ʒ��� �ﰢ�� 2��
		InsertVertex(m_vBase[4], DXCOLOR_RED);
		InsertVertex(m_vBase[6], DXCOLOR_GREEN);
		InsertVertex(m_vBase[7], DXCOLOR_BLUE);

		InsertVertex(m_vBase[4], DXCOLOR_RED);
		InsertVertex(m_vBase[5], DXCOLOR_GREEN);
		InsertVertex(m_vBase[6], DXCOLOR_BLUE);

		// ===============================================
		// �޸� �ﰢ�� 2��
		InsertVertex(m_vBase[3], DXCOLOR_RED);
		InsertVertex(m_vBase[6], DXCOLOR_GREEN);
		InsertVertex(m_vBase[2], DXCOLOR_BLUE);

		InsertVertex(m_vBase[3], DXCOLOR_RED);
		InsertVertex(m_vBase[7], DXCOLOR_GREEN);
		InsertVertex(m_vBase[6], DXCOLOR_BLUE);

		// ���� ���۷� ť�� �ϳ��� �������ϱ� ���� ���� 36���� �ʿ��մϴ�.
		// �ߺ��Ǵ� ������ �����Ƿ� ����ȭ�� �ʿ��մϴ�.

		CreateVertexBuffer();
		return S_OK;
	}

	HRESULT RXCubeDX9::CreateCubeByIndex(const D3DXVECTOR3 & vMin, const D3DXVECTOR3 & vMax)
	{
		InitBaseVertex(vMin, vMax);
		AllocVertexBuffer();
		AllocIndexBuffer();		

		// �ε����� ���� ���� 8���� �˴ϴ�.
		InsertVertex(m_vBase[0], DXCOLOR_RED);
		InsertVertex(m_vBase[1], DXCOLOR_GREEN);
		InsertVertex(m_vBase[2], DXCOLOR_BLUE);
		InsertVertex(m_vBase[3], DXCOLOR_RED);
		InsertVertex(m_vBase[4], DXCOLOR_GREEN);
		InsertVertex(m_vBase[5], DXCOLOR_BLUE);
		InsertVertex(m_vBase[6], DXCOLOR_RED);
		InsertVertex(m_vBase[7], DXCOLOR_GREEN);

		// ===============================================
		// �ε��� �����Դϴ�.
		// ���� �ﰢ�� 2��
		InsertIndex(0, 1, 5);
		InsertIndex(0, 5, 4);

		// ===============================================
		// ���� ���� �ﰢ�� 2��
		InsertIndex(0, 4, 3);
		InsertIndex(3, 4, 7);

		// ===============================================
		// ������ ���� �ﰢ�� 2��
		InsertIndex(1, 2, 5);
		InsertIndex(2, 6, 5);

		// ===============================================
		// ���� �ﰢ�� 2��
		InsertIndex(0, 3, 2);
		InsertIndex(0, 2, 1);

		// ===============================================
		// �Ʒ��� �ﰢ�� 2��
		InsertIndex(4, 6, 7);
		InsertIndex(4, 5, 6);

		// ===============================================
		// �޸� �ﰢ�� 2��
		InsertIndex(3, 6, 2);
		InsertIndex(3, 7, 6);

		// �ε��� ���۷� ť�� �ϳ��� �������ϱ� ���� ���� 8���� �ʿ��մϴ�.
		// ���� 8���� �̿��ؼ� �׸��� ������ �ε����� �־��ָ� �˴ϴ�.
		// �ε��� ���۴� D3DPT_TRIANGLELIST�� �⺻�̰� ������ ������ ��������� �մϴ�.
		// ť�� �ϳ��� �������ϱ� ���ؼ��� �������� �ּ� 12�� �ʿ��մϴ�.

		CreateVertexBuffer();
		CreateIndexBuffer(12);
		return S_OK;
	}

	void RXCubeDX9::InitBaseVertex(const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax)
	{
		// �� �� �������� ���̸� ���մϴ�.
		FLOAT rDistX = std::abs(vMax.x - vMin.x);
		FLOAT rDistY = std::abs(vMax.y - vMin.y);
		FLOAT rDistZ = std::abs(vMax.z - vMin.z);

		// ===============================================
		// ť�긦 �������Ϸ��� �ﰢ�� 12���� �ʿ��մϴ�.
		// �� �ﰢ���� ���ϱ� ���ؼ��� ���� 8���� �ʿ��մϴ�.
		// ���� ���� 8������ ���س��� �� ���մϴ�.
		// �� �κ��� �ּ����� ǥ���س����Կ�.
		// ===============================================

		// ���� ���� 4��
		::ZeroMemory(m_vBase, sizeof(D3DXVECTOR3) * _countof(m_vBase));
		RX::SetVector(&m_vBase[0], vMax.x - rDistX, vMax.y, vMax.z - rDistZ); // 0
		RX::SetVector(&m_vBase[1], vMax.x, vMax.y, vMax.z - rDistZ);          // 1
		RX::SetVector(&m_vBase[2], vMax.x, vMax.y, vMax.z);                   // 2
		RX::SetVector(&m_vBase[3], vMax.x - rDistX, vMax.y, vMax.z);          // 3

		// �Ʒ��� ���� 4��
		RX::SetVector(&m_vBase[4], vMin.x, vMin.y, vMin.z);                   // 4
		RX::SetVector(&m_vBase[5], vMin.x + rDistX, vMin.y, vMin.z);          // 5
		RX::SetVector(&m_vBase[6], vMin.x + rDistX, vMin.y, vMin.z + rDistZ); // 6
		RX::SetVector(&m_vBase[7], vMin.x, vMin.y, vMin.z + rDistZ);          // 7
	}

	void RXCubeDX9::DrawCubeByVertex()
	{
		DrawPrimitive();
	}

	void RXCubeDX9::DrawCubeByIndex()
	{
		DrawIdxedPrimitive();
	}

} // namespace RX end
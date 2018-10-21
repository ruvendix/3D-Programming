/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-10-14
 *
 * <파일 내용>
 * 테스트용 큐브를 다룹니다.
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
		// 정면 삼각형 2개
		InsertVertex(m_vBase[0], DXCOLOR_RED);
		InsertVertex(m_vBase[1], DXCOLOR_GREEN);
		InsertVertex(m_vBase[5], DXCOLOR_BLUE);

		InsertVertex(m_vBase[0], DXCOLOR_RED);
		InsertVertex(m_vBase[5], DXCOLOR_GREEN);
		InsertVertex(m_vBase[4], DXCOLOR_BLUE);

		// ===============================================
		// 왼쪽 측면 삼각형 2개
		InsertVertex(m_vBase[0], DXCOLOR_RED);
		InsertVertex(m_vBase[4], DXCOLOR_GREEN);
		InsertVertex(m_vBase[3], DXCOLOR_BLUE);
					
		InsertVertex(m_vBase[3], DXCOLOR_RED);
		InsertVertex(m_vBase[4], DXCOLOR_GREEN);
		InsertVertex(m_vBase[7], DXCOLOR_BLUE);

		// ===============================================
		// 오른쪽 측면 삼각형 2개
		InsertVertex(m_vBase[1], DXCOLOR_RED);
		InsertVertex(m_vBase[2], DXCOLOR_GREEN);
		InsertVertex(m_vBase[5], DXCOLOR_BLUE);

		InsertVertex(m_vBase[2], DXCOLOR_RED);
		InsertVertex(m_vBase[6], DXCOLOR_GREEN);
		InsertVertex(m_vBase[5], DXCOLOR_BLUE);

		// ===============================================
		// 윗면 삼각형 2개
		InsertVertex(m_vBase[0], DXCOLOR_RED);
		InsertVertex(m_vBase[3], DXCOLOR_GREEN);
		InsertVertex(m_vBase[2], DXCOLOR_BLUE);

		InsertVertex(m_vBase[0], DXCOLOR_RED);
		InsertVertex(m_vBase[2], DXCOLOR_GREEN);
		InsertVertex(m_vBase[1], DXCOLOR_BLUE);

		// ===============================================
		// 아랫면 삼각형 2개
		InsertVertex(m_vBase[4], DXCOLOR_RED);
		InsertVertex(m_vBase[6], DXCOLOR_GREEN);
		InsertVertex(m_vBase[7], DXCOLOR_BLUE);

		InsertVertex(m_vBase[4], DXCOLOR_RED);
		InsertVertex(m_vBase[5], DXCOLOR_GREEN);
		InsertVertex(m_vBase[6], DXCOLOR_BLUE);

		// ===============================================
		// 뒷면 삼각형 2개
		InsertVertex(m_vBase[3], DXCOLOR_RED);
		InsertVertex(m_vBase[6], DXCOLOR_GREEN);
		InsertVertex(m_vBase[2], DXCOLOR_BLUE);

		InsertVertex(m_vBase[3], DXCOLOR_RED);
		InsertVertex(m_vBase[7], DXCOLOR_GREEN);
		InsertVertex(m_vBase[6], DXCOLOR_BLUE);

		// 정점 버퍼로 큐브 하나를 렌더링하기 위해 정점 36개가 필요합니다.
		// 중복되는 정점이 많으므로 최적화가 필요합니다.

		CreateVertexBuffer();
		return S_OK;
	}

	HRESULT RXCubeDX9::CreateCubeByIndex(const D3DXVECTOR3 & vMin, const D3DXVECTOR3 & vMax)
	{
		InitBaseVertex(vMin, vMax);
		AllocVertexBuffer();
		AllocIndexBuffer();		

		// 인덱스일 때는 정점 8개면 됩니다.
		InsertVertex(m_vBase[0], DXCOLOR_RED);
		InsertVertex(m_vBase[1], DXCOLOR_GREEN);
		InsertVertex(m_vBase[2], DXCOLOR_BLUE);
		InsertVertex(m_vBase[3], DXCOLOR_RED);
		InsertVertex(m_vBase[4], DXCOLOR_GREEN);
		InsertVertex(m_vBase[5], DXCOLOR_BLUE);
		InsertVertex(m_vBase[6], DXCOLOR_RED);
		InsertVertex(m_vBase[7], DXCOLOR_GREEN);

		// ===============================================
		// 인덱스 설정입니다.
		// 정면 삼각형 2개
		InsertIndex(0, 1, 5);
		InsertIndex(0, 5, 4);

		// ===============================================
		// 왼쪽 측면 삼각형 2개
		InsertIndex(0, 4, 3);
		InsertIndex(3, 4, 7);

		// ===============================================
		// 오른쪽 측면 삼각형 2개
		InsertIndex(1, 2, 5);
		InsertIndex(2, 6, 5);

		// ===============================================
		// 윗면 삼각형 2개
		InsertIndex(0, 3, 2);
		InsertIndex(0, 2, 1);

		// ===============================================
		// 아랫면 삼각형 2개
		InsertIndex(4, 6, 7);
		InsertIndex(4, 5, 6);

		// ===============================================
		// 뒷면 삼각형 2개
		InsertIndex(3, 6, 2);
		InsertIndex(3, 7, 6);

		// 인덱스 버퍼로 큐브 하나를 렌더링하기 위해 정점 8개가 필요합니다.
		// 정점 8개를 이용해서 그리는 순서를 인덱스에 넣어주면 됩니다.
		// 인덱스 버퍼는 D3DPT_TRIANGLELIST가 기본이고 폴리곤 개수를 설정해줘야 합니다.
		// 큐브 하나를 렌더링하기 위해서는 폴리곤이 최소 12개 필요합니다.

		CreateVertexBuffer();
		CreateIndexBuffer(12);
		return S_OK;
	}

	void RXCubeDX9::InitBaseVertex(const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax)
	{
		// 각 축 기준으로 길이를 구합니다.
		FLOAT rDistX = std::abs(vMax.x - vMin.x);
		FLOAT rDistY = std::abs(vMax.y - vMin.y);
		FLOAT rDistZ = std::abs(vMax.z - vMin.z);

		// ===============================================
		// 큐브를 렌더링하려면 삼각형 12개가 필요합니다.
		// 그 삼각형을 구하기 위해서는 정점 8개가 필요합니다.
		// 먼저 정점 8개부터 구해놓는 게 편합니다.
		// 각 부분을 주석으로 표시해놓을게요.
		// ===============================================

		// 윗면 정점 4개
		::ZeroMemory(m_vBase, sizeof(D3DXVECTOR3) * _countof(m_vBase));
		RX::SetVector(&m_vBase[0], vMax.x - rDistX, vMax.y, vMax.z - rDistZ); // 0
		RX::SetVector(&m_vBase[1], vMax.x, vMax.y, vMax.z - rDistZ);          // 1
		RX::SetVector(&m_vBase[2], vMax.x, vMax.y, vMax.z);                   // 2
		RX::SetVector(&m_vBase[3], vMax.x - rDistX, vMax.y, vMax.z);          // 3

		// 아랫면 정점 4개
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
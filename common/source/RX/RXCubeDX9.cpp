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

	RXCubeDX9::~RXCubeDX9()
	{
		SAFE_DELTE(m_pVB);
	}

	HRESULT RXCubeDX9::CreateCube(const D3DXVECTOR3& vMin,
		const D3DXVECTOR3& vMax, CUBE_DRAW_TYPE type)
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
		D3DXVECTOR3 vCube[8];
		::ZeroMemory(vCube, sizeof(D3DXVECTOR3) * _countof(vCube));
		RX::SetVector(&vCube[0], vMax.x - rDistX, vMax.y, vMax.z - rDistZ); // 0
		RX::SetVector(&vCube[1], vMax.x, vMax.y, vMax.z - rDistZ);          // 1
		RX::SetVector(&vCube[2], vMax.x, vMax.y, vMax.z);                   // 2
		RX::SetVector(&vCube[3], vMax.x - rDistX, vMax.y, vMax.z);          // 3

		// 아랫면 정점 4개
		RX::SetVector(&vCube[4], vMin.x, vMin.y, vMin.z);                   // 4
		RX::SetVector(&vCube[5], vMin.x + rDistX, vMin.y, vMin.z);          // 5
		RX::SetVector(&vCube[6], vMin.x + rDistX, vMin.y, vMin.z + rDistZ); // 6
		RX::SetVector(&vCube[7], vMin.x, vMin.y, vMin.z + rDistZ);          // 7

		m_pVB = RXNew RX::RXVertexBufferDX9;
		NULLCHK_HEAPALLOC(m_pVB);

		if (type == CUBE_DRAW_TYPE::INDEX)
		{
			// 인덱스일 때는 정점 8개면 됩니다.
			m_pVB->InsertVertex(vCube[0], DXCOLOR_RED);
			m_pVB->InsertVertex(vCube[1], DXCOLOR_GREEN);
			m_pVB->InsertVertex(vCube[2], DXCOLOR_BLUE);
			m_pVB->InsertVertex(vCube[3], DXCOLOR_RED);
			m_pVB->InsertVertex(vCube[4], DXCOLOR_GREEN);
			m_pVB->InsertVertex(vCube[5], DXCOLOR_BLUE);
			m_pVB->InsertVertex(vCube[6], DXCOLOR_RED);
			m_pVB->InsertVertex(vCube[7], DXCOLOR_GREEN);

			// ===============================================
			// 인덱스 설정입니다.
			// 정면 삼각형 2개
			m_pVB->InsertIndex(0, 1, 5);
			m_pVB->InsertIndex(0, 5, 4);
			// ===============================================
			// 왼쪽 측면 삼각형 2개
			m_pVB->InsertIndex(0, 4, 3);
			m_pVB->InsertIndex(3, 4, 7);
			// ===============================================
			// 오른쪽 측면 삼각형 2개
			m_pVB->InsertIndex(1, 2, 5);
			m_pVB->InsertIndex(2, 6, 5);
			// ===============================================
			// 윗면 삼각형 2개
			m_pVB->InsertIndex(0, 3, 2);
			m_pVB->InsertIndex(0, 2, 1);
			// ===============================================
			// 아랫면 삼각형 2개
			m_pVB->InsertIndex(4, 6, 7);
			m_pVB->InsertIndex(4, 5, 6);
			// ===============================================
			// 뒷면 삼각형 2개
			m_pVB->InsertIndex(3, 6, 2);
			m_pVB->InsertIndex(3, 7, 6);
			// ===============================================
			// 큐브는 삼각형이 12개 있습니다.
			m_pVB->CreateIndexBuffer(12);
		}
		else
		{
			// ===============================================
			// 정면 삼각형 2개
			m_pVB->InsertVertex(vCube[0], DXCOLOR_RED);
			m_pVB->InsertVertex(vCube[1], DXCOLOR_GREEN);
			m_pVB->InsertVertex(vCube[5], DXCOLOR_BLUE);

			m_pVB->InsertVertex(vCube[0], DXCOLOR_RED);
			m_pVB->InsertVertex(vCube[5], DXCOLOR_GREEN);
			m_pVB->InsertVertex(vCube[4], DXCOLOR_BLUE);
			// ===============================================
			// 왼쪽 측면 삼각형 2개
			m_pVB->InsertVertex(vCube[0], DXCOLOR_RED);
			m_pVB->InsertVertex(vCube[4], DXCOLOR_GREEN);
			m_pVB->InsertVertex(vCube[3], DXCOLOR_BLUE);

			m_pVB->InsertVertex(vCube[3], DXCOLOR_RED);
			m_pVB->InsertVertex(vCube[4], DXCOLOR_GREEN);
			m_pVB->InsertVertex(vCube[7], DXCOLOR_BLUE);
			// ===============================================
			// 오른쪽 측면 삼각형 2개
			m_pVB->InsertVertex(vCube[1], DXCOLOR_RED);
			m_pVB->InsertVertex(vCube[2], DXCOLOR_GREEN);
			m_pVB->InsertVertex(vCube[5], DXCOLOR_BLUE);

			m_pVB->InsertVertex(vCube[2], DXCOLOR_RED);
			m_pVB->InsertVertex(vCube[6], DXCOLOR_GREEN);
			m_pVB->InsertVertex(vCube[5], DXCOLOR_BLUE);
			// ===============================================
			// 윗면 삼각형 2개
			m_pVB->InsertVertex(vCube[0], DXCOLOR_RED);
			m_pVB->InsertVertex(vCube[3], DXCOLOR_GREEN);
			m_pVB->InsertVertex(vCube[2], DXCOLOR_BLUE);

			m_pVB->InsertVertex(vCube[0], DXCOLOR_RED);
			m_pVB->InsertVertex(vCube[2], DXCOLOR_GREEN);
			m_pVB->InsertVertex(vCube[1], DXCOLOR_BLUE);
			// ===============================================
			// 아랫면 삼각형 2개
			m_pVB->InsertVertex(vCube[4], DXCOLOR_RED);
			m_pVB->InsertVertex(vCube[6], DXCOLOR_GREEN);
			m_pVB->InsertVertex(vCube[7], DXCOLOR_BLUE);

			m_pVB->InsertVertex(vCube[4], DXCOLOR_RED);
			m_pVB->InsertVertex(vCube[5], DXCOLOR_GREEN);
			m_pVB->InsertVertex(vCube[6], DXCOLOR_BLUE);
			// ===============================================
			// 뒷면 삼각형 2개
			m_pVB->InsertVertex(vCube[3], DXCOLOR_RED);
			m_pVB->InsertVertex(vCube[6], DXCOLOR_GREEN);
			m_pVB->InsertVertex(vCube[2], DXCOLOR_BLUE);

			m_pVB->InsertVertex(vCube[3], DXCOLOR_RED);
			m_pVB->InsertVertex(vCube[7], DXCOLOR_GREEN);
			m_pVB->InsertVertex(vCube[6], DXCOLOR_BLUE);
		}

		m_pVB->CreateVertexBuffer();
		return S_OK;
	}

	void RXCubeDX9::DrawCube(CUBE_DRAW_TYPE type)
	{
		if (type == CUBE_DRAW_TYPE::INDEX)
		{
			m_pVB->DrawIndexedPrimitive();
		}
		else
		{
			m_pVB->DrawPrimitive(D3DPT_TRIANGLELIST);
		}
	}

} // namespace RX end
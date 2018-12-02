/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-11-05
 *
 * <파일 내용>
 * 이전 프로젝트에 사용된 코드를 모아놓은 곳입니다.
 *
 ====================================================================================*/
#include "base_project.h"
#include "old_main.h"

void InputOldKeyboard()
{
	static FLOAT rAngleZ = 0.0f;
	static FLOAT rAngleX = 0.0f;
	static FLOAT rAngleY = 0.0f;

	FLOAT rDeltaSeconds = RX::RXProgramFPSMgr::Instance()->getTimer()->getDeltaSeconds();
	if (::GetAsyncKeyState('A') & 0x8000)
	{
		rAngleZ += 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('D') & 0x8000)
	{
		rAngleZ -= 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('W') & 0x8000)
	{
		rAngleX += 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('S') & 0x8000)
	{
		rAngleX -= 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('Q') & 0x8000)
	{
		rAngleY += 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('E') & 0x8000)
	{
		rAngleY -= 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('R') & 0x8000)
	{
		rAngleZ = 0.0f;
		rAngleX = 0.0f;
		rAngleY = 0.0f;
	}

	// 각도 보정
	rAngleZ = RX::AdjustAngle(rAngleZ);
	rAngleX = RX::AdjustAngle(rAngleX);
	rAngleY = RX::AdjustAngle(rAngleY);

	// 회전행렬입니다. 순서는 Z -> X -> Y입니다.
	// 즉, Roll -> Pitch -> Yaw입니다.
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot,
		D3DXToRadian(rAngleY), D3DXToRadian(rAngleX), D3DXToRadian(rAngleZ));
	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &matRot);

	// 전체변환행렬을 구합니다.
	g_matAll = matRot * g_matViewAndProjection;

	// 법선벡터 형식을 선택합니다.
	if (::GetAsyncKeyState('Z') & 0x8000)
	{
		ON_BIT(g_dwNormalVectorRenderingFlag, NORMAL_VECTOR_RENDERING_VERTEX);
	}

	if (::GetAsyncKeyState('X') & 0x8000)
	{
		OFF_BIT(g_dwNormalVectorRenderingFlag, NORMAL_VECTOR_RENDERING_VERTEX);
	}

	if (::GetAsyncKeyState('C') & 0x8000)
	{
		ON_BIT(g_dwNormalVectorRenderingFlag, NORMAL_VECTOR_RENDERING_TRIANGLE);
	}

	if (::GetAsyncKeyState('V') & 0x8000)
	{
		OFF_BIT(g_dwNormalVectorRenderingFlag, NORMAL_VECTOR_RENDERING_TRIANGLE);
	}

	if (::GetAsyncKeyState('B') & 0x8000)
	{
		ON_BIT(g_dwNormalVectorRenderingFlag, NORMAL_VECTOR_RENDERING_PLANE);
	}

	if (::GetAsyncKeyState('N') & 0x8000)
	{
		OFF_BIT(g_dwNormalVectorRenderingFlag, NORMAL_VECTOR_RENDERING_PLANE);
	}

	// 필모드를 선택합니다.
	if (::GetAsyncKeyState('F') & 0x8000)
	{
		g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	if (::GetAsyncKeyState('G') & 0x8000)
	{
		g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}
}

void CalcTriangleNormal()
{
	::ZeroMemory(g_vCubeTriangleNormal, sizeof(D3DXVECTOR3) * 12);

	INT32 index = -1;
	for (INT32 i = 0; i < 12; ++i)
	{
		D3DXVECTOR3 v1 = g_vBaseVertex[g_vecIndex16[++index].index];
		D3DXVECTOR3 v2 = g_vBaseVertex[g_vecIndex16[++index].index];
		D3DXVECTOR3 v3 = g_vBaseVertex[g_vecIndex16[++index].index];
		g_vCubeTriangleNormal[i] = RX::CalcNormalVector(v1, v2, v3);
	}
}

void RenderNormalVector()
{
	D3DXVECTOR3 vList[2];

	if (IS_BIT(g_dwNormalVectorRenderingFlag, NORMAL_VECTOR_RENDERING_VERTEX))
	{
		INT32 normalVectorTable[6][4] =
		{
			{ 0, 1, 4, 5 },
			{ 0, 3, 4, 7 },
			{ 1, 2, 5, 6 },
			{ 0, 1, 2, 3 },
			{ 4, 5, 6, 7 },
			{ 3, 2, 6, 7 },
		};

		for (INT32 i = 0; i < 6; ++i)
		{
			g_pLine->Begin();
			vList[0] = g_vBaseVertex[normalVectorTable[i][0]];
			vList[1] = vList[0] + g_vCubeTriangleNormal[2];
			g_pLine->DrawTransform(vList, 2, &g_matAll, DXCOLOR_WHITE);
			g_pLine->End();

			g_pLine->Begin();
			vList[0] = g_vBaseVertex[normalVectorTable[i][1]];
			vList[1] = vList[0] + g_vCubeTriangleNormal[2];
			g_pLine->DrawTransform(vList, 2, &g_matAll, DXCOLOR_WHITE);
			g_pLine->End();

			g_pLine->Begin();
			vList[0] = g_vBaseVertex[normalVectorTable[i][2]];
			vList[1] = vList[0] + g_vCubeTriangleNormal[2];
			g_pLine->DrawTransform(vList, 2, &g_matAll, DXCOLOR_WHITE);
			g_pLine->End();

			g_pLine->Begin();
			vList[0] = g_vBaseVertex[normalVectorTable[i][3]];
			vList[1] = vList[0] + g_vCubeTriangleNormal[2];
			g_pLine->DrawTransform(vList, 2, &g_matAll, DXCOLOR_WHITE);
			g_pLine->End();
		}
	}

	if (IS_BIT(g_dwNormalVectorRenderingFlag, NORMAL_VECTOR_RENDERING_TRIANGLE))
	{
		INT32 normalVectorTable[12][3] =
		{
			{ 0, 1, 5 },
			{ 0, 5, 4 },
			{ 0, 4, 3 },
			{ 3, 4, 7 },
			{ 1, 2, 5 },
			{ 2, 6, 5 },
			{ 0, 3, 2 },
			{ 0, 2, 1 },
			{ 4, 6, 7 },
			{ 4, 5, 6 },
			{ 3, 6, 2 },
			{ 3, 7, 6 },
		};

		for (INT32 i = 0; i < 12; ++i)
		{
			g_pLine->Begin();

			// 중점을 구합니다.
			// 중점은 각 좌표를 더해서 원하는만큼 나눠주면 됩니다.
			vList[0] =
				(g_vBaseVertex[normalVectorTable[i][0]] +
					g_vBaseVertex[normalVectorTable[i][1]] +
					g_vBaseVertex[normalVectorTable[i][2]]) / 3.0f;

			vList[1] = vList[0] + g_vCubeTriangleNormal[2];
			g_pLine->DrawTransform(vList, 2, &g_matAll, DXCOLOR_WHITE);
			g_pLine->End();
		}
	}

	if (IS_BIT(g_dwNormalVectorRenderingFlag, NORMAL_VECTOR_RENDERING_PLANE))
	{
		INT32 normalVectorTable[6][4] =
		{
			{ 0, 1, 4, 5 },
			{ 0, 3, 4, 7 },
			{ 1, 2, 5, 6 },
			{ 0, 1, 2, 3 },
			{ 4, 5, 6, 7 },
			{ 3, 2, 6, 7 },
		};

		for (INT32 i = 0; i < 6; ++i)
		{
			g_pLine->Begin();

			// 중점을 구합니다.
			// 중점은 각 좌표를 더해서 원하는만큼 나눠주면 됩니다.
			vList[0] =
				(g_vBaseVertex[normalVectorTable[i][0]] +
					g_vBaseVertex[normalVectorTable[i][1]] +
					g_vBaseVertex[normalVectorTable[i][2]] +
					g_vBaseVertex[normalVectorTable[i][3]]) / 4.0f;

			vList[1] = vList[0] + g_vCubeTriangleNormal[2];
			g_pLine->DrawTransform(vList, 2, &g_matAll, DXCOLOR_WHITE);
			g_pLine->End();
		}
	}
}

void CreateCube(FLOAT rPoint1, FLOAT rPoint2)
{
	if (rPoint1 > rPoint2) // rPoint1이 최솟값, rPoint2가 최댓값입니다.
	{
		std::swap(rPoint1, rPoint2); // 두 값을 바꿔줍니다.
	}

	InsertBaseVertex(rPoint1, rPoint2);
	InitCubeVertexAndIndex(rPoint1, rPoint2);

	INT32 vertexCnt = g_vecP3N.size();
	g_DXResult = g_pD3DDevice9->CreateVertexBuffer(
		sizeof(VertexP3N) * vertexCnt, // 정점 총 용량
		D3DUSAGE_WRITEONLY, // 정점 버퍼의 용도
		VertexP3N::format,  // 정점 형식
		D3DPOOL_MANAGED,    // 원하는 메모리풀
		&g_pVertexBuffer,   // 정점 버퍼의 주소
		nullptr); // 안 쓰는 파라미터
	DXERR_HANDLER(g_DXResult);
	NULLCHK_RETURN(g_pVertexBuffer, "정점 버퍼 생성 실패했습니다!");

	void* pVertexData = nullptr;
	g_pVertexBuffer->Lock(
		0, // 락을 시작할 오프셋
		sizeof(VertexP3N) * vertexCnt, // 락을 할 정점 총 용량
		&pVertexData,  // 락된 메모리의 위치
		D3DFLAG_NONE); // 락 플래그

	// 벡터를 이렇게 복사할 수 있는데 표준 정의입니다.
	// 즉, 벡터는 연속된 메모리라는 보장이 있는 건데
	// 만약 불안하다면 배열로 변경해도 괜찮습니다.
	::CopyMemory(pVertexData, &g_vecP3N[0], sizeof(VertexP3N) * vertexCnt);

	g_pVertexBuffer->Unlock();

	// ================================================================================

	INT32 indexCnt = g_vecIndex16.size();
	g_DXResult = g_pD3DDevice9->CreateIndexBuffer(
		sizeof(Index16) * indexCnt, // 인덱스 총 용량
		D3DUSAGE_WRITEONLY, // 인덱스 버퍼의 용도
		Index16::format, // 인덱스 형식
		D3DPOOL_MANAGED, // 원하는 메모리풀
		&g_pIndexBuffer, // 인덱스 버퍼의 주소
		nullptr);        // 안 쓰는 파라미터
	DXERR_HANDLER(g_DXResult);
	NULLCHK_RETURN(g_pIndexBuffer, "인덱스 버퍼 생성 실패했습니다!");

	void* pIndexData = nullptr;
	g_pIndexBuffer->Lock(
		0, // 락을 시작할 오프셋
		sizeof(Index16) * indexCnt, // 락을 할 인덱스 총 용량
		&pIndexData,   // 락된 메모리의 위치
		D3DFLAG_NONE); // 락 플래그

	// 벡터를 이렇게 복사할 수 있는데 표준 정의입니다.
	// 즉, 벡터는 연속된 메모리라는 보장이 있는 건데
	// 만약 불안하다면 배열로 변경해도 괜찮습니다.
	::CopyMemory(pIndexData, &g_vecIndex16[0], sizeof(Index16) * indexCnt);

	g_pIndexBuffer->Unlock();
}

void InsertBaseVertex(FLOAT rPoint1, FLOAT rPoint2)
{
	// 축 기준으로 길이를 구합니다.
	FLOAT rDistance = rPoint2 - rPoint1;

	// ===============================================
	// 큐브를 렌더링하려면 삼각형 12개가 필요합니다.
	// 그 삼각형을 구하기 위해서는 정점 8개가 필요합니다.
	// 먼저 정점 8개부터 구해놓는 게 편합니다.
	// 각 부분을 주석으로 표시해놓을게요.
	// ===============================================
	::ZeroMemory(g_vBaseVertex, sizeof(D3DXVECTOR3) * _countof(g_vBaseVertex));

	// 윗면 정점 4개
	RX::SetVector(&g_vBaseVertex[0], rPoint2 - rDistance, rPoint2, rPoint2 - rDistance); // 0
	RX::SetVector(&g_vBaseVertex[1], rPoint2, rPoint2, rPoint2 - rDistance);             // 1
	RX::SetVector(&g_vBaseVertex[2], rPoint2, rPoint2, rPoint2);                         // 2
	RX::SetVector(&g_vBaseVertex[3], rPoint2 - rDistance, rPoint2, rPoint2);             // 3

	// 아랫면 정점 4개
	RX::SetVector(&g_vBaseVertex[4], rPoint1, rPoint1, rPoint1);                         // 4
	RX::SetVector(&g_vBaseVertex[5], rPoint1 + rDistance, rPoint1, rPoint1);             // 5
	RX::SetVector(&g_vBaseVertex[6], rPoint1 + rDistance, rPoint1, rPoint1 + rDistance); // 6
	RX::SetVector(&g_vBaseVertex[7], rPoint1, rPoint1, rPoint1 + rDistance);             // 7
}

// 정점 8개를 설정합니다.
// 인덱스를 이용해서 렌더링할 것이므로 필요한 인덱스는 36개!
void InitCubeVertexAndIndex(FLOAT rPoint1, FLOAT rPoint2)
{
	// ===============================================
	// 인덱스일 때는 정점 8개면 됩니다.
	// 미리 계산해둔 정점
	VertexP3N vertex;
	vertex.vPos = g_vBaseVertex[0];
	g_vecP3N.push_back(vertex);

	vertex.vPos = g_vBaseVertex[1];
	g_vecP3N.push_back(vertex);

	vertex.vPos = g_vBaseVertex[2];
	g_vecP3N.push_back(vertex);

	vertex.vPos = g_vBaseVertex[3];
	g_vecP3N.push_back(vertex);

	vertex.vPos = g_vBaseVertex[4];
	g_vecP3N.push_back(vertex);

	vertex.vPos = g_vBaseVertex[5];
	g_vecP3N.push_back(vertex);

	vertex.vPos = g_vBaseVertex[6];
	g_vecP3N.push_back(vertex);

	vertex.vPos = g_vBaseVertex[7];
	g_vecP3N.push_back(vertex);

	// ===============================================
	// 인덱스 설정입니다.
	// 정면 삼각형 2개
	Index16 index;
	index.index = 0;
	g_vecIndex16.push_back(index);

	index.index = 1;
	g_vecIndex16.push_back(index);

	index.index = 5;
	g_vecIndex16.push_back(index);
	// -----------------------------------------------
	index.index = 0;
	g_vecIndex16.push_back(index);

	index.index = 5;
	g_vecIndex16.push_back(index);

	index.index = 4;
	g_vecIndex16.push_back(index);
	// ===============================================
	// 왼쪽 측면 삼각형 2개	
	index.index = 0;
	g_vecIndex16.push_back(index);

	index.index = 4;
	g_vecIndex16.push_back(index);

	index.index = 3;
	g_vecIndex16.push_back(index);
	// -----------------------------------------------
	index.index = 3;
	g_vecIndex16.push_back(index);

	index.index = 4;
	g_vecIndex16.push_back(index);

	index.index = 7;
	g_vecIndex16.push_back(index);
	// ===============================================
	// 오른쪽 측면 삼각형 2개
	index.index = 1;
	g_vecIndex16.push_back(index);

	index.index = 2;
	g_vecIndex16.push_back(index);

	index.index = 5;
	g_vecIndex16.push_back(index);
	// -----------------------------------------------
	index.index = 2;
	g_vecIndex16.push_back(index);

	index.index = 6;
	g_vecIndex16.push_back(index);

	index.index = 5;
	g_vecIndex16.push_back(index);
	// ===============================================
	// 윗면 삼각형 2개
	index.index = 0;
	g_vecIndex16.push_back(index);

	index.index = 3;
	g_vecIndex16.push_back(index);

	index.index = 2;
	g_vecIndex16.push_back(index);
	// -----------------------------------------------
	index.index = 0;
	g_vecIndex16.push_back(index);

	index.index = 2;
	g_vecIndex16.push_back(index);

	index.index = 1;
	g_vecIndex16.push_back(index);
	// ===============================================
	// 아랫면 삼각형 2개
	index.index = 4;
	g_vecIndex16.push_back(index);

	index.index = 6;
	g_vecIndex16.push_back(index);

	index.index = 7;
	g_vecIndex16.push_back(index);
	// -----------------------------------------------
	index.index = 4;
	g_vecIndex16.push_back(index);

	index.index = 5;
	g_vecIndex16.push_back(index);

	index.index = 6;
	g_vecIndex16.push_back(index);
	// ===============================================
	// 뒷면 삼각형 2개
	index.index = 3;
	g_vecIndex16.push_back(index);

	index.index = 6;
	g_vecIndex16.push_back(index);

	index.index = 2;
	g_vecIndex16.push_back(index);
	// -----------------------------------------------
	index.index = 3;
	g_vecIndex16.push_back(index);

	index.index = 7;
	g_vecIndex16.push_back(index);

	index.index = 6;
	g_vecIndex16.push_back(index);

	// 인덱스 버퍼로 큐브 하나를 렌더링하기 위해 정점 8개가 필요합니다.
	// 정점 8개를 이용해서 그리는 순서를 인덱스에 넣어주면 됩니다.
	// 인덱스 버퍼는 D3DPT_TRIANGLELIST가 기본이고 폴리곤 개수를 설정해줘야 합니다.
	// 큐브 하나를 렌더링하기 위해서는 폴리곤이 최소 12개 필요합니다.

	// 삼각형에서의 법선벡터를 구합니다.
	CalcTriangleNormal();

	INT32 size = g_vecP3N.size();
	for (INT32 i = 0; i < size; ++i)
	{
		g_vecP3N[i].vN = g_vCubeTriangleNormal[2];
	}
}
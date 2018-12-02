/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <�ۼ� ��¥>
 * 2018-11-05
 *
 * <���� ����>
 * ���� ������Ʈ�� ���� �ڵ带 ��Ƴ��� ���Դϴ�.
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

	// ���� ����
	rAngleZ = RX::AdjustAngle(rAngleZ);
	rAngleX = RX::AdjustAngle(rAngleX);
	rAngleY = RX::AdjustAngle(rAngleY);

	// ȸ������Դϴ�. ������ Z -> X -> Y�Դϴ�.
	// ��, Roll -> Pitch -> Yaw�Դϴ�.
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot,
		D3DXToRadian(rAngleY), D3DXToRadian(rAngleX), D3DXToRadian(rAngleZ));
	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &matRot);

	// ��ü��ȯ����� ���մϴ�.
	g_matAll = matRot * g_matViewAndProjection;

	// �������� ������ �����մϴ�.
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

	// �ʸ�带 �����մϴ�.
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

			// ������ ���մϴ�.
			// ������ �� ��ǥ�� ���ؼ� ���ϴ¸�ŭ �����ָ� �˴ϴ�.
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

			// ������ ���մϴ�.
			// ������ �� ��ǥ�� ���ؼ� ���ϴ¸�ŭ �����ָ� �˴ϴ�.
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
	if (rPoint1 > rPoint2) // rPoint1�� �ּڰ�, rPoint2�� �ִ��Դϴ�.
	{
		std::swap(rPoint1, rPoint2); // �� ���� �ٲ��ݴϴ�.
	}

	InsertBaseVertex(rPoint1, rPoint2);
	InitCubeVertexAndIndex(rPoint1, rPoint2);

	INT32 vertexCnt = g_vecP3N.size();
	g_DXResult = g_pD3DDevice9->CreateVertexBuffer(
		sizeof(VertexP3N) * vertexCnt, // ���� �� �뷮
		D3DUSAGE_WRITEONLY, // ���� ������ �뵵
		VertexP3N::format,  // ���� ����
		D3DPOOL_MANAGED,    // ���ϴ� �޸�Ǯ
		&g_pVertexBuffer,   // ���� ������ �ּ�
		nullptr); // �� ���� �Ķ����
	DXERR_HANDLER(g_DXResult);
	NULLCHK_RETURN(g_pVertexBuffer, "���� ���� ���� �����߽��ϴ�!");

	void* pVertexData = nullptr;
	g_pVertexBuffer->Lock(
		0, // ���� ������ ������
		sizeof(VertexP3N) * vertexCnt, // ���� �� ���� �� �뷮
		&pVertexData,  // ���� �޸��� ��ġ
		D3DFLAG_NONE); // �� �÷���

	// ���͸� �̷��� ������ �� �ִµ� ǥ�� �����Դϴ�.
	// ��, ���ʹ� ���ӵ� �޸𸮶�� ������ �ִ� �ǵ�
	// ���� �Ҿ��ϴٸ� �迭�� �����ص� �������ϴ�.
	::CopyMemory(pVertexData, &g_vecP3N[0], sizeof(VertexP3N) * vertexCnt);

	g_pVertexBuffer->Unlock();

	// ================================================================================

	INT32 indexCnt = g_vecIndex16.size();
	g_DXResult = g_pD3DDevice9->CreateIndexBuffer(
		sizeof(Index16) * indexCnt, // �ε��� �� �뷮
		D3DUSAGE_WRITEONLY, // �ε��� ������ �뵵
		Index16::format, // �ε��� ����
		D3DPOOL_MANAGED, // ���ϴ� �޸�Ǯ
		&g_pIndexBuffer, // �ε��� ������ �ּ�
		nullptr);        // �� ���� �Ķ����
	DXERR_HANDLER(g_DXResult);
	NULLCHK_RETURN(g_pIndexBuffer, "�ε��� ���� ���� �����߽��ϴ�!");

	void* pIndexData = nullptr;
	g_pIndexBuffer->Lock(
		0, // ���� ������ ������
		sizeof(Index16) * indexCnt, // ���� �� �ε��� �� �뷮
		&pIndexData,   // ���� �޸��� ��ġ
		D3DFLAG_NONE); // �� �÷���

	// ���͸� �̷��� ������ �� �ִµ� ǥ�� �����Դϴ�.
	// ��, ���ʹ� ���ӵ� �޸𸮶�� ������ �ִ� �ǵ�
	// ���� �Ҿ��ϴٸ� �迭�� �����ص� �������ϴ�.
	::CopyMemory(pIndexData, &g_vecIndex16[0], sizeof(Index16) * indexCnt);

	g_pIndexBuffer->Unlock();
}

void InsertBaseVertex(FLOAT rPoint1, FLOAT rPoint2)
{
	// �� �������� ���̸� ���մϴ�.
	FLOAT rDistance = rPoint2 - rPoint1;

	// ===============================================
	// ť�긦 �������Ϸ��� �ﰢ�� 12���� �ʿ��մϴ�.
	// �� �ﰢ���� ���ϱ� ���ؼ��� ���� 8���� �ʿ��մϴ�.
	// ���� ���� 8������ ���س��� �� ���մϴ�.
	// �� �κ��� �ּ����� ǥ���س����Կ�.
	// ===============================================
	::ZeroMemory(g_vBaseVertex, sizeof(D3DXVECTOR3) * _countof(g_vBaseVertex));

	// ���� ���� 4��
	RX::SetVector(&g_vBaseVertex[0], rPoint2 - rDistance, rPoint2, rPoint2 - rDistance); // 0
	RX::SetVector(&g_vBaseVertex[1], rPoint2, rPoint2, rPoint2 - rDistance);             // 1
	RX::SetVector(&g_vBaseVertex[2], rPoint2, rPoint2, rPoint2);                         // 2
	RX::SetVector(&g_vBaseVertex[3], rPoint2 - rDistance, rPoint2, rPoint2);             // 3

	// �Ʒ��� ���� 4��
	RX::SetVector(&g_vBaseVertex[4], rPoint1, rPoint1, rPoint1);                         // 4
	RX::SetVector(&g_vBaseVertex[5], rPoint1 + rDistance, rPoint1, rPoint1);             // 5
	RX::SetVector(&g_vBaseVertex[6], rPoint1 + rDistance, rPoint1, rPoint1 + rDistance); // 6
	RX::SetVector(&g_vBaseVertex[7], rPoint1, rPoint1, rPoint1 + rDistance);             // 7
}

// ���� 8���� �����մϴ�.
// �ε����� �̿��ؼ� �������� ���̹Ƿ� �ʿ��� �ε����� 36��!
void InitCubeVertexAndIndex(FLOAT rPoint1, FLOAT rPoint2)
{
	// ===============================================
	// �ε����� ���� ���� 8���� �˴ϴ�.
	// �̸� ����ص� ����
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
	// �ε��� �����Դϴ�.
	// ���� �ﰢ�� 2��
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
	// ���� ���� �ﰢ�� 2��	
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
	// ������ ���� �ﰢ�� 2��
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
	// ���� �ﰢ�� 2��
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
	// �Ʒ��� �ﰢ�� 2��
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
	// �޸� �ﰢ�� 2��
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

	// �ε��� ���۷� ť�� �ϳ��� �������ϱ� ���� ���� 8���� �ʿ��մϴ�.
	// ���� 8���� �̿��ؼ� �׸��� ������ �ε����� �־��ָ� �˴ϴ�.
	// �ε��� ���۴� D3DPT_TRIANGLELIST�� �⺻�̰� ������ ������ ��������� �մϴ�.
	// ť�� �ϳ��� �������ϱ� ���ؼ��� �������� �ּ� 12�� �ʿ��մϴ�.

	// �ﰢ�������� �������͸� ���մϴ�.
	CalcTriangleNormal();

	INT32 size = g_vecP3N.size();
	for (INT32 i = 0; i < size; ++i)
	{
		g_vecP3N[i].vN = g_vCubeTriangleNormal[2];
	}
}
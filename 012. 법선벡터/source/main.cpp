#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


// ====================================================================================
// ��ũ�� ���Ǻ��Դϴ�.
#define NORMAL_VECTOR_RENDERING_VERTEX   0x0001
#define NORMAL_VECTOR_RENDERING_TRIANGLE 0x0010
#define NORMAL_VECTOR_RENDERING_PLANE    0x0100

// ====================================================================================
// ���� ���� ������Դϴ�.
HRESULT g_DXResult = S_OK;

ID3DXLine* g_pLine = nullptr; // ���� �׸��� ���� ��

D3DXMATRIXA16 g_matViewAndProjection; // �̸� ����ص� (����� * �������)
D3DXMATRIXA16 g_matAllTransform;      // ��ü��ȯ��� (���� �ִ� g_matViewAndProjection �̿�)

namespace
{
	IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
	IDirect3DIndexBuffer9*  g_pIndexBuffer  = nullptr;

	std::vector<VertexP3D>  g_vecP3D;         // ���� �����Դϴ�.
	std::vector<Index16>    g_vecIndex16;     // �ε��� �����Դϴ�.
	D3DXVECTOR3             g_vBaseVertex[8]; // ���� ���� �����Դϴ�.

	D3DXVECTOR3 g_vCubeTriangleNormal[12]; // �ﰢ�������� ��������
	DWORD       g_dwNormalVectorRenderingFlag = 0;

	D3DXVECTOR3 g_rotateAngle;
}


// ====================================================================================
// �Լ� ������Դϴ�.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

// Ű���� �Է��� ó�����ִ� �Լ��Դϴ�.
void InputKeyboard();

// �ﰢ�������� �������͸� ���ϴ� �Լ��Դϴ�.
void CalcTriangleNormal();

// �������͸� �������ϴ� �Լ��Դϴ�.
void RenderNormalVector();

void CreateCube(FLOAT rPoint1, FLOAT rPoint2);
void InsertBaseVertex(FLOAT rPoint1, FLOAT rPoint2);
void InitCubeVertexAndIndex(FLOAT rPoint1, FLOAT rPoint2);

// ====================================================================================
// <Win32 API�� WinMain()�� �������Դϴ�>
INT32 APIENTRY _tWinMain(HINSTANCE hInstance,
	                     HINSTANCE hPrevInstance,
	                     LPTSTR    szCmdLine,
	                     INT32     cmdShow)
{
	// ������� �ʴ� �Ű������� ǥ�����ݴϴ�.
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(szCmdLine);
	UNREFERENCED_PARAMETER(cmdShow);

	RXMAIN_DX9->setSubFunc(OnInit,    SUBFUNC_TYPE::INIT);
	RXMAIN_DX9->setSubFunc(OnUpdate,  SUBFUNC_TYPE::UPDATE);
	RXMAIN_DX9->setSubFunc(OnRender,  SUBFUNC_TYPE::RENDER);
	RXMAIN_DX9->setSubFunc(OnRelease, SUBFUNC_TYPE::RELEASE);

	RXMAIN_DX9->RunMainRoutine(hInstance, IDI_RUVENDIX_ICO);
	return RXMAIN_DX9->getMessageCode();
}

HRESULT CALLBACK OnInit()
{
	CreateCube(-1.0f, 1.0f);

	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, false);
	D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	// ==========================================================
	// ������� �����մϴ�.
	D3DXVECTOR3 vEye(4.0f, 4.0f, -4.0f);   // ī�޶��� ��ġ
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // ī�޶� ���� ����
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // ī�޶��� �� ����

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	D3DDEVICE9->SetTransform(D3DTS_VIEW, &matView);
	// ==========================================================
	// ��������� �����մϴ�.
	D3DXMATRIXA16 matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection, (D3DX_PI / 4.0f),
		(static_cast<FLOAT>(RXMAIN_DX9->getClientRect()->CalcWidth()) /
		                   (RXMAIN_DX9->getClientRect()->CalcHeight())), 1.0f, 1000.0f);
	D3DDEVICE9->SetTransform(D3DTS_PROJECTION, &matProjection);
	// ==========================================================

	// ���� ���� �ʱ�ȭ�Դϴ�.
	::ZeroMemory(g_vCubeTriangleNormal, sizeof(D3DXVECTOR3) * 12);

	// ������� �ʱ�ȭ�Դϴ�.
	D3DXMatrixIdentity(&g_matAllTransform);
	g_matViewAndProjection = matView * matProjection;

	// ���� �׸��� ��ü�� �����մϴ�.
	D3DXCreateLine(D3DDEVICE9, &g_pLine);

	// �ﰢ�������� �������͸� ���մϴ�.
	CalcTriangleNormal();

	RX::ShowMouseCursor(true);

	return S_OK;
}

HRESULT CALLBACK OnUpdate()
{
	InputKeyboard();
	return S_OK;
}

HRESULT CALLBACK OnRender()
{
	D3DDEVICE9->SetFVF(VertexP3D::format);
	D3DDEVICE9->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(VertexP3D));
	D3DDEVICE9->SetIndices(g_pIndexBuffer);
	
	D3DDEVICE9->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
	RenderNormalVector();

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	SAFE_RELEASE(g_pIndexBuffer);
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pLine);
	return S_OK;
}

void InputKeyboard()
{
	FLOAT rDeltaSeconds = RX::RXProgramFPSMgr::Instance()->getTimer()->getDeltaSeconds();
	if (::GetAsyncKeyState('A') & 0x8000)
	{
		g_rotateAngle.z += 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('D') & 0x8000)
	{
		g_rotateAngle.z -= 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('W') & 0x8000)
	{
		g_rotateAngle.x += 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('S') & 0x8000)
	{
		g_rotateAngle.x -= 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('Q') & 0x8000)
	{
		g_rotateAngle.y += 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('E') & 0x8000)
	{
		g_rotateAngle.y -= 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('R') & 0x8000)
	{
		RX::ZeroVector(&g_rotateAngle);
	}

	g_rotateAngle.z = RX::AdjustAngle(g_rotateAngle.z);
	g_rotateAngle.x = RX::AdjustAngle(g_rotateAngle.x);
	g_rotateAngle.y = RX::AdjustAngle(g_rotateAngle.y);

	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot,
		D3DXToRadian(g_rotateAngle.y), D3DXToRadian(g_rotateAngle.x),
		D3DXToRadian(g_rotateAngle.z));
	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matRot);

	// ��ü��ȯ����� ���մϴ�.
	g_matAllTransform = matRot * g_matViewAndProjection;

	// �������� ������ �����մϴ�.
	if (::GetAsyncKeyState('Z') & 0x0001)
	{
		TOGGLE_BIT(g_dwNormalVectorRenderingFlag, NORMAL_VECTOR_RENDERING_VERTEX);
	}

	if (::GetAsyncKeyState('X') & 0x0001)
	{
		TOGGLE_BIT(g_dwNormalVectorRenderingFlag, NORMAL_VECTOR_RENDERING_TRIANGLE);
	}

	if (::GetAsyncKeyState('C') & 0x0001)
	{
		TOGGLE_BIT(g_dwNormalVectorRenderingFlag, NORMAL_VECTOR_RENDERING_PLANE);
	}

	if (::GetAsyncKeyState('V') & 0x8000)
	{
		D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	if (::GetAsyncKeyState('B') & 0x8000)
	{
		D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}
}

void CalcTriangleNormal()
{
	::ZeroMemory(g_vCubeTriangleNormal, sizeof(D3DXVECTOR3) * 12);

	INT32 index = -1;
	for (INT32 i = 0; i < 12; ++i)
	{
		// �ε��� ���ۿ� ���ϴ� ������ ������� �������Ƿ�
		// �ε����� �ϳ��� ������Ű�� ���ϴ� ������ �˾Ƴ� �� �ֽ��ϴ�.
		// ���� ������ ������ ���� 2���� �˾Ƴ��� �������͸� ���� �� �ֽ��ϴ�.
		D3DXVECTOR3 v1 = g_vBaseVertex[g_vecIndex16[++index].index];
		D3DXVECTOR3 v2 = g_vBaseVertex[g_vecIndex16[++index].index];
		D3DXVECTOR3 v3 = g_vBaseVertex[g_vecIndex16[++index].index];
		g_vCubeTriangleNormal[i] = RX::CalcNormalVector(v1, v2, v3);
	}
}

// ť�� ���� �ε����Դϴ�.
// �� ���� �ε����� ���� �ҽ� �ڵ�� ���纸����.

//    3---------2
//   /|        /|
//  / |       / |
// 0---------1  |
// |  7------|--6
// | /       | /
// |/        |/
// 4---------5

void RenderNormalVector()
{
	D3DXVECTOR3 vList[2];

	if (IS_BIT(g_dwNormalVectorRenderingFlag, NORMAL_VECTOR_RENDERING_VERTEX))
	{
		// ���������� ���� �����Դϴ�.
		// ���� �ִ� ť���� ���� ��ġ�� �����Ƿ� �������ּ���.
		// ť�� �� �ϳ����� ���� 4���� ���Ƿ� ���� �ִ� �ּ��� ���纸����.
		INT32 normalVectorTable[6][4] =
		{
			{ 0, 1, 4, 5 },
			{ 0, 3, 4, 7 },
			{ 1, 2, 5, 6 },
			{ 0, 1, 2, 3 },
			{ 4, 5, 6, 7 },
			{ 3, 2, 6, 7 },
		};

		// ���������� ���� ������ �˾Ƴ����� �� ��������
		// �̸� ����ص� ���������� ��ġ��ŭ ���� ���� ���� �׸��� �˴ϴ�. ��, �����Դϴ�.
		// ����ص� �������ʹ� �ﰢ���� �������� ��ұ� ������ ������ �鿡���� 2���� �ǳʶپ�� �մϴ�.
		// ������ �������Ͱ� 2���� ��ģ�ٴ� ���� �����ϼ���. �ﰢ���� ���� �״�� ����ϸ� �˴ϴ�.
		for (INT32 i = 0; i < 6; ++i) // ť���� ���� 6��
		{
			g_pLine->Begin(); // ���� �׸��ڴٰ� �˸�
			vList[0] = g_vBaseVertex[normalVectorTable[i][0]];  // ���� ����
			vList[1] = vList[0] + g_vCubeTriangleNormal[i * 2]; // ���� ���� + ��������
			g_pLine->DrawTransform(vList, 2, &g_matAllTransform, DXCOLOR_WHITE); // ��ȯ �����ؼ� ���� �׸�
			g_pLine->End(); // ���� �� �׷ȴٰ� �˸�

			g_pLine->Begin();
			vList[0] = g_vBaseVertex[normalVectorTable[i][1]];
			vList[1] = vList[0] + g_vCubeTriangleNormal[i * 2];
			g_pLine->DrawTransform(vList, 2, &g_matAllTransform, DXCOLOR_WHITE);
			g_pLine->End();

			g_pLine->Begin();
			vList[0] = g_vBaseVertex[normalVectorTable[i][2]];
			vList[1] = vList[0] + g_vCubeTriangleNormal[i * 2];
			g_pLine->DrawTransform(vList, 2, &g_matAllTransform, DXCOLOR_WHITE);
			g_pLine->End();

			g_pLine->Begin();
			vList[0] = g_vBaseVertex[normalVectorTable[i][3]];
			vList[1] = vList[0] + g_vCubeTriangleNormal[i * 2];
			g_pLine->DrawTransform(vList, 2, &g_matAllTransform, DXCOLOR_WHITE);
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
			// �ﰢ���� ������ ���� �������� ��ƾ� �մϴ�.
			vList[0] =
				(g_vBaseVertex[normalVectorTable[i][0]] +
				 g_vBaseVertex[normalVectorTable[i][1]] +
				 g_vBaseVertex[normalVectorTable[i][2]]) / 3.0f;

			vList[1] = vList[0] + g_vCubeTriangleNormal[i];
			g_pLine->DrawTransform(vList, 2, &g_matAllTransform, DXCOLOR_WHITE);
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

			// ���� ������ ���� �������� ��ƾ� �մϴ�.
			vList[0] =
				(g_vBaseVertex[normalVectorTable[i][0]] +
				 g_vBaseVertex[normalVectorTable[i][1]] +
				 g_vBaseVertex[normalVectorTable[i][2]] +
				 g_vBaseVertex[normalVectorTable[i][3]]) / 4.0f;

			vList[1] = vList[0] + g_vCubeTriangleNormal[i * 2];
			g_pLine->DrawTransform(vList, 2, &g_matAllTransform, DXCOLOR_WHITE);
			g_pLine->End();
		}
	}
}

#pragma region ť�� ����� ����
void CreateCube(FLOAT rPoint1, FLOAT rPoint2)
{
	if (rPoint1 > rPoint2)
	{
		std::swap(rPoint1, rPoint2);
	}

	InsertBaseVertex(rPoint1, rPoint2);
	InitCubeVertexAndIndex(rPoint1, rPoint2);

	INT32 vertexCnt = g_vecP3D.size();
	g_DXResult = D3DDEVICE9->CreateVertexBuffer(sizeof(VertexP3D) * vertexCnt, D3DUSAGE_WRITEONLY,
		VertexP3D::format, D3DPOOL_MANAGED, &g_pVertexBuffer, nullptr);
	DXERR_HANDLER(g_DXResult);
	NULLCHK_RETURN(g_pVertexBuffer, "���� ���� ���� �����߽��ϴ�!");

	void* pVertexData = nullptr;
	g_pVertexBuffer->Lock(0, sizeof(VertexP3D) * vertexCnt, &pVertexData, D3DLOCK_READONLY);
	::CopyMemory(pVertexData, &g_vecP3D[0], sizeof(VertexP3D) * vertexCnt);
	g_pVertexBuffer->Unlock();

	// ================================================================================
	
	INT32 indexCnt = g_vecIndex16.size();
	g_DXResult = D3DDEVICE9->CreateIndexBuffer(sizeof(Index16) * indexCnt, D3DUSAGE_WRITEONLY,
		Index16::format, D3DPOOL_MANAGED, &g_pIndexBuffer, nullptr);
	DXERR_HANDLER(g_DXResult);
	NULLCHK_RETURN(g_pIndexBuffer, "�ε��� ���� ���� �����߽��ϴ�!");

	void* pIndexData = nullptr;
	g_pIndexBuffer->Lock(0, sizeof(Index16) * indexCnt, &pIndexData, D3DLOCK_READONLY);
	::CopyMemory(pIndexData, &g_vecIndex16[0], sizeof(Index16) * indexCnt);
	g_pIndexBuffer->Unlock();
}

void InsertBaseVertex(FLOAT rPoint1, FLOAT rPoint2)
{
	FLOAT rDistance = rPoint2 - rPoint1;

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

void InitCubeVertexAndIndex(FLOAT rPoint1, FLOAT rPoint2)
{
	// ===============================================
	// �ε����� ���� ���� 8���� �˴ϴ�.
	VertexP3D vertex;
	vertex.vPos    = g_vBaseVertex[0];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[1];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[2];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[3];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[4];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[5];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[6];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[7];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);

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
}
#pragma endregion
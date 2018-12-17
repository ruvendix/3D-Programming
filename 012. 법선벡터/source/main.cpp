#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"

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

// ====================================================================================
// ��ũ�� ���Ǻ��Դϴ�.
#define NORMAL_VECTOR_RENDERING_VERTEX   0x0001
#define NORMAL_VECTOR_RENDERING_TRIANGLE 0x0010
#define NORMAL_VECTOR_RENDERING_PLANE    0x0100

// ====================================================================================
// ���� ���� ������Դϴ�.
IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
IDirect3DIndexBuffer9*  g_pIndexBuffer  = nullptr;
ID3DXLine*              g_pLine         = nullptr;     // ���� �׸��� ���� ��
D3DXVECTOR3 g_vCubeTriangleNormal[12]; // �ﰢ�������� ��������

D3DXMATRIXA16 g_matViewAndProjection; // �̸� ����ص� ����� * �������
D3DXMATRIXA16 g_matProjection;        // �̸� ����ص� �������
D3DXMATRIXA16 g_matAll; // ������İ� �����ϱ� ���� ��ü��ȯ���

DWORD   g_dwNormalVectorRenderingFlag = 0;
HRESULT g_DXResult = S_OK;

std::vector<VertexP3D> g_vecP3D;
std::vector<Index16>   g_vecIndex16;
D3DXVECTOR3            g_vBaseVertex[8];


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

// ť�긦 �����ϴ� �Լ��Դϴ�.
// ť��� �� 2���� ������ ������ �� �ֽ��ϴ�.
// ���ο��� �� 2���� ���� ���� �ڿ� �ּڰ��� �ּڰ����� �з��մϴ�.
void CreateCube(FLOAT rPoint1, FLOAT rPoint2);

// ������ �Ǵ� ���� 8���� �ֽ��ϴ�.
void InsertBaseVertex(FLOAT rPoint1, FLOAT rPoint2);

// ť�긦 �����ϱ� ���� ���� ������ �ε��� ������ �����մϴ�.
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

// �ʱ�ȭ �Լ��Դϴ�.
// 3D �������� ������ ���� ���Ƿ� ������ �۾��� �ʱ�ȭ����� �մϴ�.
// �������ϸ鼭 �ǽð����� ���굵 ���������� �׷��� �Ǹ� �������� �������� �˴ϴ�.
// �Ϲ������� �������� ���� ������ �۾��� ó���մϴ�.
HRESULT CALLBACK OnInit()
{
	CreateCube(-1.0f, 1.0f);

	// rhw�� ������� �ʴ´ٸ� ��ȯ ������ ������ǥ�� ����ϰ� �ǹǷ�
	// ���� ��ȯ ������ ���ľ� �մϴ�. ����(������, Lighting)�� ���� �ϳ��ε�
	// ���� ���� ������ ���� ���� �ʾ����Ƿ� ������ ����� �մϴ�.
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, false);

	// �ø� ��带 �����մϴ�. ����Ʈ�� �ݽð���� �ø��Դϴ�.
	// ť�긦 Ȯ���ϱ� ���ؼ��� �ø� ��带 �����ؾ� �մϴ�.
	//D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// ������� �����մϴ�.
	D3DXVECTOR3 vEye(4.0f, 4.0f, -4.0f);   // ī�޶��� ��ġ
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // ī�޶� ���� ����
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // ī�޶��� �� ����

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	D3DDEVICE9->SetTransform(D3DTS_VIEW, &matView);

	// ��������� �����մϴ�.
	D3DXMATRIXA16 matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection, (D3DX_PI / 4.0f),
		(static_cast<FLOAT>(RXMAIN_DX9->getClientRect()->CalcWidth()) /
		                   (RXMAIN_DX9->getClientRect()->CalcHeight())), 1.0f, 1000.0f);
	D3DDEVICE9->SetTransform(D3DTS_PROJECTION, &g_matProjection);

	// ���� ���� �ʱ�ȭ�Դϴ�.
	::ZeroMemory(g_vCubeTriangleNormal, sizeof(D3DXVECTOR3) * 12);

	// ������� �ʱ�ȭ�Դϴ�.
	D3DXMatrixIdentity(&g_matAll);
	g_matViewAndProjection = matView * g_matProjection;

	// ���� �׸��� ��ü�� �����մϴ�.
	D3DXCreateLine(D3DDEVICE9, &g_pLine);

	// �ﰢ�������� �������͸� ���մϴ�.
	CalcTriangleNormal();

	// ���콺 Ŀ���� �����ݴϴ�.
	RX::ShowMouseCursor(true);

	return S_OK;
}

// ������Ʈ �Լ��Դϴ�.
// �������� ������ �ְų� ���α׷��� ������ �ִ�
// �������� �������Ӹ��� ������Ʈ�մϴ�.
HRESULT CALLBACK OnUpdate()
{
	InputKeyboard();
	return S_OK;
}

// ������ �Լ��Դϴ�.
// ���� ������ �۾��� Draw Call�� ó���˴ϴ�.
// Draw Call�� �����Ӵ� ȣ��Ǵ� ������ �Լ��� ���ϴµ� ȣ��Ǵ� �󵵼���
// �����ϸ� Draw Call Count�� �˾Ƴ� �� �ֽ��ϴ�.
HRESULT CALLBACK OnRender()
{
	D3DDEVICE9->SetFVF(VertexP3D::format); // ���� ���� ����
	D3DDEVICE9->SetStreamSource(
		0,                  // ����� ��Ʈ�� �ε���
		g_pVertexBuffer,    // ������ ���� ����
		0,                  // ���� ������ ������
		sizeof(VertexP3D)); // ���� �뷮

	D3DDEVICE9->SetIndices(g_pIndexBuffer); // �ε��� ���� ����
	D3DDEVICE9->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, // �ε��� ���۴� Ʈ���̾ޱ۸���Ʈ�� ����
		0,   // ù �ε����� �� ���� ������ ���� �ε���
		0,   // ����� ù �ε���(�ε����� 0, 1, 2, 3�� ���� �� 3�̸� 3���� ����)
		8,   // ����� ���� ����(2��° ���� + 3��° ���ڸ�ŭ �� ��)
		0,   // �ε��� ������ ������
		12); // �������� ����(ť��� �ﰢ�� 12��)

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
	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matRot);

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
		D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	if (::GetAsyncKeyState('G') & 0x8000)
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
			vList[1] = vList[0] + g_vCubeTriangleNormal[i * 2];
			g_pLine->DrawTransform(vList, 2, &g_matAll, DXCOLOR_WHITE);
			g_pLine->End();

			g_pLine->Begin();
			vList[0] = g_vBaseVertex[normalVectorTable[i][1]];
			vList[1] = vList[0] + g_vCubeTriangleNormal[i * 2];
			g_pLine->DrawTransform(vList, 2, &g_matAll, DXCOLOR_WHITE);
			g_pLine->End();

			g_pLine->Begin();
			vList[0] = g_vBaseVertex[normalVectorTable[i][2]];
			vList[1] = vList[0] + g_vCubeTriangleNormal[i * 2];
			g_pLine->DrawTransform(vList, 2, &g_matAll, DXCOLOR_WHITE);
			g_pLine->End();

			g_pLine->Begin();
			vList[0] = g_vBaseVertex[normalVectorTable[i][3]];
			vList[1] = vList[0] + g_vCubeTriangleNormal[i * 2];
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

			vList[1] = vList[0] + g_vCubeTriangleNormal[i];
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

			vList[1] = vList[0] + g_vCubeTriangleNormal[i * 2];
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

	INT32 vertexCnt = g_vecP3D.size();
	g_DXResult = D3DDEVICE9->CreateVertexBuffer(
		sizeof(VertexP3D) * vertexCnt, // ���� �� �뷮
		D3DUSAGE_WRITEONLY, // ���� ������ �뵵
		VertexP3D::format,  // ���� ����
		D3DPOOL_MANAGED,    // ���ϴ� �޸�Ǯ
		&g_pVertexBuffer,   // ���� ������ �ּ�
		nullptr); // �� ���� �Ķ����
	DXERR_HANDLER(g_DXResult);
	NULLCHK_RETURN(g_pVertexBuffer, "���� ���� ���� �����߽��ϴ�!");

	void* pVertexData = nullptr;
	g_pVertexBuffer->Lock(
		0, // ���� ������ ������
		sizeof(VertexP3D) * vertexCnt, // ���� �� ���� �� �뷮
		&pVertexData,  // ���� �޸��� ��ġ
		D3DFLAG_NONE); // �� �÷���

	// ���͸� �̷��� ������ �� �ִµ� ǥ�� �����Դϴ�.
	// ��, ���ʹ� ���ӵ� �޸𸮶�� ������ �ִ� �ǵ�
	// ���� �Ҿ��ϴٸ� �迭�� �����ص� �������ϴ�.
	::CopyMemory(pVertexData, &g_vecP3D[0], sizeof(VertexP3D) * vertexCnt);

	g_pVertexBuffer->Unlock();

	// ================================================================================
	
	INT32 indexCnt = g_vecIndex16.size();
	g_DXResult = D3DDEVICE9->CreateIndexBuffer(
		sizeof(Index16) * indexCnt, // �ε��� �� �뷮
		D3DUSAGE_WRITEONLY, // �ε��� ������ �뵵
		Index16::format,    // �ε��� ����
		D3DPOOL_MANAGED,    // ���ϴ� �޸�Ǯ
		&g_pIndexBuffer,    // �ε��� ������ �ּ�
		nullptr);           // �� ���� �Ķ����
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

	// �ε��� ���۷� ť�� �ϳ��� �������ϱ� ���� ���� 8���� �ʿ��մϴ�.
	// ���� 8���� �̿��ؼ� �׸��� ������ �ε����� �־��ָ� �˴ϴ�.
	// �ε��� ���۴� D3DPT_TRIANGLELIST�� �⺻�̰� ������ ������ ��������� �մϴ�.
	// ť�� �ϳ��� �������ϱ� ���ؼ��� �������� �ּ� 12�� �ʿ��մϴ�.
}
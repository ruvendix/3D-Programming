#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


// ====================================================================================
// 매크로 정의부입니다.
#define NORMAL_VECTOR_RENDERING_VERTEX   0x0001
#define NORMAL_VECTOR_RENDERING_TRIANGLE 0x0010
#define NORMAL_VECTOR_RENDERING_PLANE    0x0100

// ====================================================================================
// 전역 변수 선언부입니다.
HRESULT g_DXResult = S_OK;

ID3DXLine* g_pLine = nullptr; // 선을 그리기 위한 것

D3DXMATRIXA16 g_matViewAndProjection; // 미리 계산해둔 (뷰행렬 * 투영행렬)
D3DXMATRIXA16 g_matAllTransform;      // 전체변환행렬 (위에 있는 g_matViewAndProjection 이용)

namespace
{
	IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
	IDirect3DIndexBuffer9*  g_pIndexBuffer  = nullptr;

	std::vector<VertexP3D>  g_vecP3D;         // 정점 정보입니다.
	std::vector<Index16>    g_vecIndex16;     // 인덱스 정보입니다.
	D3DXVECTOR3             g_vBaseVertex[8]; // 기준 정점 정보입니다.

	D3DXVECTOR3 g_vCubeTriangleNormal[12]; // 삼각형에서의 법선벡터
	DWORD       g_dwNormalVectorRenderingFlag = 0;

	D3DXVECTOR3 g_rotateAngle;
}


// ====================================================================================
// 함수 선언부입니다.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

// 키보드 입력을 처리해주는 함수입니다.
void InputKeyboard();

// 삼각형에서의 법선벡터를 구하는 함수입니다.
void CalcTriangleNormal();

// 법선벡터를 렌더링하는 함수입니다.
void RenderNormalVector();

void CreateCube(FLOAT rPoint1, FLOAT rPoint2);
void InsertBaseVertex(FLOAT rPoint1, FLOAT rPoint2);
void InitCubeVertexAndIndex(FLOAT rPoint1, FLOAT rPoint2);

// ====================================================================================
// <Win32 API는 WinMain()이 진입점입니다>
INT32 APIENTRY _tWinMain(HINSTANCE hInstance,
	                     HINSTANCE hPrevInstance,
	                     LPTSTR    szCmdLine,
	                     INT32     cmdShow)
{
	// 사용하지 않는 매개변수를 표시해줍니다.
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
	// 뷰행렬을 설정합니다.
	D3DXVECTOR3 vEye(4.0f, 4.0f, -4.0f);   // 카메라의 위치
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // 카메라가 보는 지점
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // 카메라의 업 벡터

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	D3DDEVICE9->SetTransform(D3DTS_VIEW, &matView);
	// ==========================================================
	// 투영행렬을 설정합니다.
	D3DXMATRIXA16 matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection, (D3DX_PI / 4.0f),
		(static_cast<FLOAT>(RXMAIN_DX9->getClientRect()->CalcWidth()) /
		                   (RXMAIN_DX9->getClientRect()->CalcHeight())), 1.0f, 1000.0f);
	D3DDEVICE9->SetTransform(D3DTS_PROJECTION, &matProjection);
	// ==========================================================

	// 정점 정보 초기화입니다.
	::ZeroMemory(g_vCubeTriangleNormal, sizeof(D3DXVECTOR3) * 12);

	// 전역행렬 초기화입니다.
	D3DXMatrixIdentity(&g_matAllTransform);
	g_matViewAndProjection = matView * matProjection;

	// 선을 그리는 객체를 생성합니다.
	D3DXCreateLine(D3DDEVICE9, &g_pLine);

	// 삼각형에서의 법선벡터를 구합니다.
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

	// 전체변환행렬을 구합니다.
	g_matAllTransform = matRot * g_matViewAndProjection;

	// 법선벡터 형식을 선택합니다.
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
		// 인덱스 버퍼에 원하는 정점이 순서대로 들어가있으므로
		// 인덱스를 하나씩 증가시키면 원하는 정점을 알아낼 수 있습니다.
		// 기준 정점과 인접한 정점 2개를 알아내면 법선벡터를 구할 수 있습니다.
		D3DXVECTOR3 v1 = g_vBaseVertex[g_vecIndex16[++index].index];
		D3DXVECTOR3 v2 = g_vBaseVertex[g_vecIndex16[++index].index];
		D3DXVECTOR3 v3 = g_vBaseVertex[g_vecIndex16[++index].index];
		g_vCubeTriangleNormal[i] = RX::CalcNormalVector(v1, v2, v3);
	}
}

// 큐브 정점 인덱스입니다.
// 각 정점 인덱스를 보고 소스 코드랑 맞춰보세요.

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
		// 법선벡터의 시작 정점입니다.
		// 위에 있는 큐브의 정점 위치와 같으므로 참고해주세요.
		// 큐브 면 하나에는 정점 4개가 들어가므로 위에 있는 주석과 맞춰보세요.
		INT32 normalVectorTable[6][4] =
		{
			{ 0, 1, 4, 5 },
			{ 0, 3, 4, 7 },
			{ 1, 2, 5, 6 },
			{ 0, 1, 2, 3 },
			{ 4, 5, 6, 7 },
			{ 3, 2, 6, 7 },
		};

		// 법선벡터의 시작 정점을 알아냈으면 그 정점에서
		// 미리 계산해둔 법선벡터의 위치만큼 더한 다음 선을 그리면 됩니다. 즉, 선분입니다.
		// 계산해둔 법선벡터는 삼각형을 기준으로 잡았기 때문에 정점과 면에서는 2개씩 건너뛰어야 합니다.
		// 동일한 법선벡터가 2개씩 겹친다는 것을 주의하세요. 삼각형일 때는 그대로 계산하면 됩니다.
		for (INT32 i = 0; i < 6; ++i) // 큐브의 면은 6개
		{
			g_pLine->Begin(); // 선을 그리겠다고 알림
			vList[0] = g_vBaseVertex[normalVectorTable[i][0]];  // 기준 정점
			vList[1] = vList[0] + g_vCubeTriangleNormal[i * 2]; // 기준 정점 + 법선벡터
			g_pLine->DrawTransform(vList, 2, &g_matAllTransform, DXCOLOR_WHITE); // 변환 적용해서 선을 그림
			g_pLine->End(); // 선을 다 그렸다고 알림

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

			// 중점을 구합니다.
			// 중점은 각 좌표를 더해서 원하는만큼 나눠주면 됩니다.
			// 삼각형의 중점을 기준 정점으로 잡아야 합니다.
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

			// 면의 중점을 기준 정점으로 잡아야 합니다.
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

#pragma region 큐브 만드는 과정
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
	NULLCHK_RETURN(g_pVertexBuffer, "정점 버퍼 생성 실패했습니다!");

	void* pVertexData = nullptr;
	g_pVertexBuffer->Lock(0, sizeof(VertexP3D) * vertexCnt, &pVertexData, D3DLOCK_READONLY);
	::CopyMemory(pVertexData, &g_vecP3D[0], sizeof(VertexP3D) * vertexCnt);
	g_pVertexBuffer->Unlock();

	// ================================================================================
	
	INT32 indexCnt = g_vecIndex16.size();
	g_DXResult = D3DDEVICE9->CreateIndexBuffer(sizeof(Index16) * indexCnt, D3DUSAGE_WRITEONLY,
		Index16::format, D3DPOOL_MANAGED, &g_pIndexBuffer, nullptr);
	DXERR_HANDLER(g_DXResult);
	NULLCHK_RETURN(g_pIndexBuffer, "인덱스 버퍼 생성 실패했습니다!");

	void* pIndexData = nullptr;
	g_pIndexBuffer->Lock(0, sizeof(Index16) * indexCnt, &pIndexData, D3DLOCK_READONLY);
	::CopyMemory(pIndexData, &g_vecIndex16[0], sizeof(Index16) * indexCnt);
	g_pIndexBuffer->Unlock();
}

void InsertBaseVertex(FLOAT rPoint1, FLOAT rPoint2)
{
	FLOAT rDistance = rPoint2 - rPoint1;

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

void InitCubeVertexAndIndex(FLOAT rPoint1, FLOAT rPoint2)
{
	// ===============================================
	// 인덱스일 때는 정점 8개면 됩니다.
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
}
#pragma endregion
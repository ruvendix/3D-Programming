#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"

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

// ====================================================================================
// 매크로 정의부입니다.
#define NORMAL_VECTOR_RENDERING_VERTEX   0x0001
#define NORMAL_VECTOR_RENDERING_TRIANGLE 0x0010
#define NORMAL_VECTOR_RENDERING_PLANE    0x0100

// ====================================================================================
// 전역 변수 선언부입니다.
IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
IDirect3DIndexBuffer9*  g_pIndexBuffer  = nullptr;
ID3DXLine*              g_pLine         = nullptr;     // 선을 그리기 위한 것
D3DXVECTOR3 g_vCubeTriangleNormal[12]; // 삼각형에서의 법선벡터

D3DXMATRIXA16 g_matViewAndProjection; // 미리 계산해둔 뷰행렬 * 투영행렬
D3DXMATRIXA16 g_matProjection;        // 미리 계산해둔 투영행렬
D3DXMATRIXA16 g_matAll; // 월드행렬과 결합하기 위한 전체변환행렬

DWORD   g_dwNormalVectorRenderingFlag = 0;
HRESULT g_DXResult = S_OK;

std::vector<VertexP3D> g_vecP3D;
std::vector<Index16>   g_vecIndex16;
D3DXVECTOR3            g_vBaseVertex[8];


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

// 큐브를 생성하는 함수입니다.
// 큐브는 점 2개만 있으면 생성할 수 있습니다.
// 내부에서 점 2개의 값을 비교한 뒤에 최솟값과 최솟값으로 분류합니다.
void CreateCube(FLOAT rPoint1, FLOAT rPoint2);

// 기준이 되는 정점 8개를 넣습니다.
void InsertBaseVertex(FLOAT rPoint1, FLOAT rPoint2);

// 큐브를 생성하기 위한 정점 정보와 인덱스 정보를 설정합니다.
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

// 초기화 함수입니다.
// 3D 렌더링은 연산이 많이 들어가므로 웬만한 작업은 초기화해줘야 합니다.
// 렌더링하면서 실시간으로 연산도 가능하지만 그렇게 되면 프레임이 떨어지게 됩니다.
// 일반적으로 렌더링할 때는 렌더링 작업만 처리합니다.
HRESULT CALLBACK OnInit()
{
	CreateCube(-1.0f, 1.0f);

	// rhw를 사용하지 않는다면 변환 이전의 공간좌표를 사용하게 되므로
	// 각종 변환 과정을 거쳐야 합니다. 조명(라이팅, Lighting)도 그중 하나인데
	// 조명에 관한 연산을 따로 하지 않았으므로 조명은 꺼줘야 합니다.
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, false);

	// 컬링 모드를 설정합니다. 디폴트는 반시계방향 컬링입니다.
	// 큐브를 확인하기 위해서는 컬링 모드를 무시해야 합니다.
	//D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// 뷰행렬을 설정합니다.
	D3DXVECTOR3 vEye(4.0f, 4.0f, -4.0f);   // 카메라의 위치
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // 카메라가 보는 지점
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // 카메라의 업 벡터

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	D3DDEVICE9->SetTransform(D3DTS_VIEW, &matView);

	// 투영행렬을 설정합니다.
	D3DXMATRIXA16 matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection, (D3DX_PI / 4.0f),
		(static_cast<FLOAT>(RXMAIN_DX9->getClientRect()->CalcWidth()) /
		                   (RXMAIN_DX9->getClientRect()->CalcHeight())), 1.0f, 1000.0f);
	D3DDEVICE9->SetTransform(D3DTS_PROJECTION, &g_matProjection);

	// 정점 정보 초기화입니다.
	::ZeroMemory(g_vCubeTriangleNormal, sizeof(D3DXVECTOR3) * 12);

	// 전역행렬 초기화입니다.
	D3DXMatrixIdentity(&g_matAll);
	g_matViewAndProjection = matView * g_matProjection;

	// 선을 그리는 객체를 생성합니다.
	D3DXCreateLine(D3DDEVICE9, &g_pLine);

	// 삼각형에서의 법선벡터를 구합니다.
	CalcTriangleNormal();

	// 마우스 커서를 보여줍니다.
	RX::ShowMouseCursor(true);

	return S_OK;
}

// 업데이트 함수입니다.
// 렌더링에 영향을 주거나 프로그램에 영향을 주는
// 정보들을 매프레임마다 업데이트합니다.
HRESULT CALLBACK OnUpdate()
{
	InputKeyboard();
	return S_OK;
}

// 렌더링 함수입니다.
// 실제 렌더링 작업인 Draw Call이 처리됩니다.
// Draw Call은 프레임당 호출되는 렌더링 함수를 뜻하는데 호출되는 빈도수를
// 조사하면 Draw Call Count를 알아낼 수 있습니다.
HRESULT CALLBACK OnRender()
{
	D3DDEVICE9->SetFVF(VertexP3D::format); // 정점 형식 연결
	D3DDEVICE9->SetStreamSource(
		0,                  // 사용할 스트림 인덱스
		g_pVertexBuffer,    // 연결할 정점 버퍼
		0,                  // 정점 버퍼의 오프셋
		sizeof(VertexP3D)); // 정점 용량

	D3DDEVICE9->SetIndices(g_pIndexBuffer); // 인덱스 버퍼 연결
	D3DDEVICE9->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, // 인덱스 버퍼는 트라이앵글리스트로 고정
		0,   // 첫 인덱스가 될 정점 버퍼의 정점 인덱스
		0,   // 사용할 첫 인덱스(인덱스가 0, 1, 2, 3이 있을 때 3이면 3부터 시작)
		8,   // 사용할 정점 개수(2번째 인자 + 3번째 인자만큼 뺀 값)
		0,   // 인덱스 버퍼의 오프셋
		12); // 렌더링할 개수(큐브는 삼각형 12개)

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

	// 각도 보정
	rAngleZ = RX::AdjustAngle(rAngleZ);
	rAngleX = RX::AdjustAngle(rAngleX);
	rAngleY = RX::AdjustAngle(rAngleY);

	// 회전행렬입니다. 순서는 Z -> X -> Y입니다.
	// 즉, Roll -> Pitch -> Yaw입니다.
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot,
		D3DXToRadian(rAngleY), D3DXToRadian(rAngleX), D3DXToRadian(rAngleZ));
	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matRot);

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

			// 중점을 구합니다.
			// 중점은 각 좌표를 더해서 원하는만큼 나눠주면 됩니다.
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

			// 중점을 구합니다.
			// 중점은 각 좌표를 더해서 원하는만큼 나눠주면 됩니다.
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
	if (rPoint1 > rPoint2) // rPoint1이 최솟값, rPoint2가 최댓값입니다.
	{
		std::swap(rPoint1, rPoint2); // 두 값을 바꿔줍니다.
	}

	InsertBaseVertex(rPoint1, rPoint2);
	InitCubeVertexAndIndex(rPoint1, rPoint2);

	INT32 vertexCnt = g_vecP3D.size();
	g_DXResult = D3DDEVICE9->CreateVertexBuffer(
		sizeof(VertexP3D) * vertexCnt, // 정점 총 용량
		D3DUSAGE_WRITEONLY, // 정점 버퍼의 용도
		VertexP3D::format,  // 정점 형식
		D3DPOOL_MANAGED,    // 원하는 메모리풀
		&g_pVertexBuffer,   // 정점 버퍼의 주소
		nullptr); // 안 쓰는 파라미터
	DXERR_HANDLER(g_DXResult);
	NULLCHK_RETURN(g_pVertexBuffer, "정점 버퍼 생성 실패했습니다!");

	void* pVertexData = nullptr;
	g_pVertexBuffer->Lock(
		0, // 락을 시작할 오프셋
		sizeof(VertexP3D) * vertexCnt, // 락을 할 정점 총 용량
		&pVertexData,  // 락된 메모리의 위치
		D3DFLAG_NONE); // 락 플래그

	// 벡터를 이렇게 복사할 수 있는데 표준 정의입니다.
	// 즉, 벡터는 연속된 메모리라는 보장이 있는 건데
	// 만약 불안하다면 배열로 변경해도 괜찮습니다.
	::CopyMemory(pVertexData, &g_vecP3D[0], sizeof(VertexP3D) * vertexCnt);

	g_pVertexBuffer->Unlock();

	// ================================================================================
	
	INT32 indexCnt = g_vecIndex16.size();
	g_DXResult = D3DDEVICE9->CreateIndexBuffer(
		sizeof(Index16) * indexCnt, // 인덱스 총 용량
		D3DUSAGE_WRITEONLY, // 인덱스 버퍼의 용도
		Index16::format,    // 인덱스 형식
		D3DPOOL_MANAGED,    // 원하는 메모리풀
		&g_pIndexBuffer,    // 인덱스 버퍼의 주소
		nullptr);           // 안 쓰는 파라미터
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

	// 인덱스 버퍼로 큐브 하나를 렌더링하기 위해 정점 8개가 필요합니다.
	// 정점 8개를 이용해서 그리는 순서를 인덱스에 넣어주면 됩니다.
	// 인덱스 버퍼는 D3DPT_TRIANGLELIST가 기본이고 폴리곤 개수를 설정해줘야 합니다.
	// 큐브 하나를 렌더링하기 위해서는 폴리곤이 최소 12개 필요합니다.
}
#include "base_project.h"
#include "global_variable_declaration.h"
#include "main.h"
#include "RX\RXCubeDX9.h"

// ====================================================================================
// 매크로 정의부입니다.


// ====================================================================================
// 구조체 및 공용체 선언부입니다.

// 정점 정보입니다.
struct CustomVertex
{
	D3DXVECTOR3 vPos;    // 위치
	DWORD       dwColor; // 색상
	const static DWORD format = D3DFVF_XYZ | D3DFVF_DIFFUSE; // 형식
};

// 인덱스 정보입니다.
struct CustomIndex
{
	WORD index; // 인덱스값
	const static D3DFORMAT format = D3DFMT_INDEX16; // 형식은 2바이트를 사용합니다.
};

// ====================================================================================
// 전역 변수 선언부입니다.
namespace
{
	RX::RXMain_DX9*           g_pMainDX       = nullptr;
	IDirect3DVertexBuffer9*   g_pVertexBuffer = nullptr;
	IDirect3DIndexBuffer9*    g_pIndexBuffer  = nullptr;
	D3DXVECTOR3               g_vBaseVertex[8];
	std::vector<CustomVertex> g_vecVertexData;
	std::vector<CustomIndex>  g_vecIndexData;
}

extern IDirect3DDevice9* g_pD3DDevice9 = nullptr;
extern HRESULT           g_DXResult    = S_OK;


// ====================================================================================
// 함수 선언부입니다.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

// 큐브를 생성하는 함수입니다.
// 큐브는 점 2개만 있으면 생성할 수 있습니다.
// 내부에서 점 2개의 값을 비교한 뒤에 최솟값과 최솟값으로 분류합니다.
void CreateCube(FLOAT rPoint1, FLOAT rPoint2);

// 기준이 되는 정점 8개를 넣습니다.
void InsertBaseVertex(FLOAT rPoint1, FLOAT rPoint2);

// 큐브를 생성하기 위한 정점 정보를 설정합니다.
void InitOnlyCubeVertex(FLOAT rPoint1, FLOAT rPoint2);

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

	g_pMainDX = RXNew RX::RXMain_DX9;
	NULLCHK(g_pMainDX);

	g_pMainDX->setSubFunc(OnInit,    SUBFUNC_TYPE::INIT);
	g_pMainDX->setSubFunc(OnUpdate,  SUBFUNC_TYPE::UPDATE);
	g_pMainDX->setSubFunc(OnRender,  SUBFUNC_TYPE::RENDER);
	g_pMainDX->setSubFunc(OnRelease, SUBFUNC_TYPE::RELEASE);

	g_pMainDX->RunMainRoutine(hInstance, IDI_RUVENDIX_ICO);

	INT32 messageCode = g_pMainDX->getMessageCode();
	SAFE_DELTE(g_pMainDX);
	return messageCode;
}

// 초기화 함수입니다.
// 3D 렌더링은 연산이 많이 들어가므로 웬만한 작업은 초기화해줘야 합니다.
// 렌더링하면서 실시간으로 연산도 가능하지만 그렇게 되면 프레임이 떨어지게 됩니다.
// 일반적으로 렌더링할 때는 렌더링 작업만 처리합니다.
HRESULT CALLBACK OnInit()
{
	g_pD3DDevice9 = RX::RXRendererDX9::Instance()->getD3DDevice9();
	NULLCHK(g_pD3DDevice9);

	CreateCube(-1.0f, 1.0f);

	// rhw를 사용하지 않는다면 변환 이전의 공간좌표를 사용하게 되므로
	// 각종 변환 과정을 거쳐야 합니다. 조명(라이팅, Lighting)도 그중 하나인데
	// 조명에 관한 연산을 따로 하지 않았으므로 조명은 꺼줘야 합니다.
	g_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, false);

	// 렌더링 모드를 설정합니다. 디폴트는 솔리드입니다.
	// 큐브를 확인하기 위해서는 와이어프레임 모드로 바꿔야 합니다.
	g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	//g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);

	// 정점 크기 설정입니다.
	// FLOAT을 DWORD로 해석해야 하므로 DWORD*로 변경한 뒤에 값을 참조해야 합니다.
	//FLOAT rPointSize = 20.0f;
	//g_pD3DDevice9->SetRenderState(D3DRS_POINTSIZE, *reinterpret_cast<DWORD*>(&rPointSize));
		
	// 컬링 모드를 설정합니다. 디폴트는 반시계방향 컬링입니다.
	// 큐브를 확인하기 위해서는 컬링 모드를 무시해야 합니다.
	g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	//g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// 뷰행렬을 설정합니다.
	D3DXVECTOR3 vEye(4.0f, 4.0f, -4.0f);   // 카메라의 위치
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // 카메라가 보는 지점
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // 카메라의 업 벡터

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	g_pD3DDevice9->SetTransform(D3DTS_VIEW, &matView);

	// 투영행렬을 설정합니다.
	D3DXMATRIXA16 matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection, (D3DX_PI / 4.0f),
		(g_pMainDX->getClientWidth() / g_pMainDX->getClientHeight()),
		1.0f, 1000.0f);
	g_pD3DDevice9->SetTransform(D3DTS_PROJECTION, &matProjection);

	return S_OK;
}

// 업데이트 함수입니다.
// 렌더링에 영향을 주거나 프로그램에 영향을 주는
// 정보들을 매프레임마다 업데이트합니다.
HRESULT CALLBACK OnUpdate()
{
	static FLOAT rAngleZ = 0.0f;
	static FLOAT rAngleX = 0.0f;
	static FLOAT rAngleY = 0.0f;

	if (::GetAsyncKeyState('A'))
	{
		rAngleZ += 4.0f;
	}

	if (::GetAsyncKeyState('D'))
	{
		rAngleZ -= 4.0f;
	}

	if (::GetAsyncKeyState('W'))
	{
		rAngleX += 4.0f;
	}

	if (::GetAsyncKeyState('S'))
	{
		rAngleX -= 4.0f;
	}

	if (::GetAsyncKeyState('Q'))
	{
		rAngleY += 4.0f;
	}

	if (::GetAsyncKeyState('E'))
	{
		rAngleY -= 4.0f;
	}

	if (::GetAsyncKeyState('R'))
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

	return S_OK;
}

// 렌더링 함수입니다.
// 실제 렌더링 작업인 Draw Call이 처리됩니다.
// Draw Call은 프레임당 호출되는 렌더링 함수를 뜻하는데 호출되는 빈도수를
// 조사하면 Draw Call Count를 알아낼 수 있습니다.
HRESULT CALLBACK OnRender()
{
	g_pD3DDevice9->SetFVF(CustomVertex::format); // 정점 형식 연결
	g_pD3DDevice9->SetStreamSource(
		0,                     // 사용할 스트림 인덱스
		g_pVertexBuffer,       // 연결할 정점 버퍼
		0,                     // 정점 버퍼의 오프셋
		sizeof(CustomVertex)); // 정점 용량

	if (g_pIndexBuffer == nullptr)
	{
		g_DXResult = g_pD3DDevice9->DrawPrimitive(
			D3DPT_TRIANGLELIST, // 렌더링할 기본 단위
			0,   // 렌더링을 시작할 정점 번호,
			12); // 렌더링할 개수(큐브는 삼각형 12개)
	}
	else
	{
		g_pD3DDevice9->SetIndices(g_pIndexBuffer); // 인덱스 버퍼 연결
		g_pD3DDevice9->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST, // 인덱스 버퍼는 트라이앵글리스트로 고정
			0,   // 첫 인덱스가 될 정점 버퍼의 정점 인덱스
			0,   // 사용할 첫 인덱스(인덱스가 0, 1, 2, 3이 있을 때 3이면 3부터 시작)
			8,   // 사용할 정점 개수(2번째 인자 + 3번째 인자만큼 뺀 값)
			0,   // 인덱스 버퍼의 오프셋
			12); // 렌더링할 개수(큐브는 삼각형 12개)
	}

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	SAFE_RELEASE(g_pIndexBuffer);
	SAFE_RELEASE(g_pVertexBuffer);
	return S_OK;
}

void CreateCube(FLOAT rPoint1, FLOAT rPoint2)
{
	if (rPoint1 > rPoint2) // rPoint1이 최솟값, rPoint2가 최댓값입니다.
	{
		std::swap(rPoint1, rPoint2); // 두 값을 바꿔줍니다.
	}

	InsertBaseVertex(rPoint1, rPoint2);
	InitOnlyCubeVertex(rPoint1, rPoint2);
	//InitCubeVertexAndIndex(rPoint1, rPoint2);

	INT32 vertexCnt = g_vecVertexData.size();
	if (vertexCnt > 0)
	{
		g_DXResult = g_pD3DDevice9->CreateVertexBuffer(
			sizeof(CustomVertex) * vertexCnt, // 정점 총 용량
			D3DUSAGE_NONE,        // 정점 버퍼의 용도
			CustomVertex::format, // 정점 형식
			D3DPOOL_MANAGED,      // 원하는 메모리풀
			&g_pVertexBuffer,     // 정점 버퍼의 주소
			nullptr);             // 안 쓰는 파라미터
		DXERR_HANDLER(g_DXResult);
		NULLCHK_RETURN(g_pVertexBuffer, "정점 버퍼 생성 실패했습니다!");

		void* pVertexData = nullptr;
		g_pVertexBuffer->Lock(
			0, // 락을 시작할 오프셋
			sizeof(CustomVertex) * vertexCnt, // 락을 할 정점 총 용량
			&pVertexData,  // 락된 메모리의 위치
			D3DFLAG_NONE); // 락 플래그

		// 벡터를 이렇게 복사할 수 있는데 표준 정의입니다.
		// 즉, 벡터는 연속된 메모리라는 보장이 있는 건데
		// 만약 불안하다면 배열로 변경해도 괜찮습니다.
		::CopyMemory(pVertexData, &g_vecVertexData[0], sizeof(CustomVertex) * vertexCnt);

		g_pVertexBuffer->Unlock();
	}
	
	INT32 indexCnt = g_vecIndexData.size();
	if (indexCnt > 0)
	{
		g_DXResult = g_pD3DDevice9->CreateIndexBuffer(
			sizeof(CustomIndex) * indexCnt, // 인덱스 총 용량
			D3DUSAGE_NONE,       // 인덱스 버퍼의 용도
			CustomIndex::format, // 인덱스 형식
			D3DPOOL_MANAGED,     // 원하는 메모리풀
			&g_pIndexBuffer,     // 인덱스 버퍼의 주소
			nullptr);            // 안 쓰는 파라미터
		DXERR_HANDLER(g_DXResult);
		NULLCHK_RETURN(g_pIndexBuffer, "인덱스 버퍼 생성 실패했습니다!");

		void* pIndexData = nullptr;
		g_pIndexBuffer->Lock(
			0, // 락을 시작할 오프셋
			sizeof(CustomIndex) * indexCnt, // 락을 할 인덱스 총 용량
			&pIndexData,   // 락된 메모리의 위치
			D3DFLAG_NONE); // 락 플래그

		// 벡터를 이렇게 복사할 수 있는데 표준 정의입니다.
		// 즉, 벡터는 연속된 메모리라는 보장이 있는 건데
		// 만약 불안하다면 배열로 변경해도 괜찮습니다.
		::CopyMemory(pIndexData, &g_vecIndexData[0], sizeof(CustomIndex) * indexCnt);

		g_pIndexBuffer->Unlock();
	}
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

// 정점 36개 설정합니다.
// 3 * 2 * 6 = 36
// 한 면을 렌더링하기 위해 삼각형 2개가 필요하고
// 삼각형 2개는 정점 6개입니다. 면이 6개이므로 필요한 정점은 36개!
void InitOnlyCubeVertex(FLOAT rPoint1, FLOAT rPoint2)
{
	// ===============================================
	// 정면 삼각형 2개
	CustomVertex vertex;
	vertex.vPos    = g_vBaseVertex[0];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[1];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[5];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecVertexData.push_back(vertex);
	// -----------------------------------------------
	vertex.vPos    = g_vBaseVertex[0];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[5];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[4];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);
	// ===============================================
	// 왼쪽 측면 삼각형 2개
	vertex.vPos    = g_vBaseVertex[0];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[4];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[3];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);
	// -----------------------------------------------
	vertex.vPos    = g_vBaseVertex[3];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[4];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[7];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);
	// ===============================================
	// 오른쪽 측면 삼각형 2개
	vertex.vPos    = g_vBaseVertex[1];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[2];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[5];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecVertexData.push_back(vertex);
	// -----------------------------------------------
	vertex.vPos    = g_vBaseVertex[2];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[6];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[5];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecVertexData.push_back(vertex);
	// ===============================================
	// 윗면 삼각형 2개
	vertex.vPos    = g_vBaseVertex[0];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[3];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[2];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecVertexData.push_back(vertex);
	// -----------------------------------------------
	vertex.vPos    = g_vBaseVertex[0];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[2];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[1];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);
	// ===============================================
	// 아랫면 삼각형 2개
	vertex.vPos    = g_vBaseVertex[4];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[6];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[7];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);
	// -----------------------------------------------
	vertex.vPos    = g_vBaseVertex[4];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[5];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[6];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);
	// ===============================================
	// 뒷면 삼각형 2개
	vertex.vPos    = g_vBaseVertex[3];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[6];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[2];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecVertexData.push_back(vertex);
	// -----------------------------------------------
	vertex.vPos    = g_vBaseVertex[3];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[7];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[6];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	// 정점 버퍼로 큐브 하나를 렌더링하기 위해 정점 36개가 필요합니다.
	// 중복되는 정점이 많으므로 최적화가 필요합니다.
}

// 정점 8개를 설정합니다.
// 인덱스를 이용해서 렌더링할 것이므로 필요한 인덱스는 36개!
void InitCubeVertexAndIndex(FLOAT rPoint1, FLOAT rPoint2)
{
	// ===============================================
	// 인덱스일 때는 정점 8개면 됩니다.
	CustomVertex vertex;
	vertex.vPos    = g_vBaseVertex[0];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[1];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[2];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[3];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[4];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[5];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[6];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[7];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);

	// ===============================================
	// 인덱스 설정입니다.
	// 정면 삼각형 2개
	CustomIndex index;
	index.index = 0;
	g_vecIndexData.push_back(index);

	index.index = 1;
	g_vecIndexData.push_back(index);

	index.index = 5;
	g_vecIndexData.push_back(index);
	// -----------------------------------------------
	index.index = 0;
	g_vecIndexData.push_back(index);

	index.index = 5;
	g_vecIndexData.push_back(index);

	index.index = 4;
	g_vecIndexData.push_back(index);
	// ===============================================
	// 왼쪽 측면 삼각형 2개	
	index.index = 0;
	g_vecIndexData.push_back(index);

	index.index = 4;
	g_vecIndexData.push_back(index);

	index.index = 3;
	g_vecIndexData.push_back(index);
	// -----------------------------------------------
	index.index = 3;
	g_vecIndexData.push_back(index);

	index.index = 4;
	g_vecIndexData.push_back(index);

	index.index = 7;
	g_vecIndexData.push_back(index);
	// ===============================================
	// 오른쪽 측면 삼각형 2개
	index.index = 1;
	g_vecIndexData.push_back(index);

	index.index = 2;
	g_vecIndexData.push_back(index);

	index.index = 5;
	g_vecIndexData.push_back(index);
	// -----------------------------------------------
	index.index = 2;
	g_vecIndexData.push_back(index);

	index.index = 6;
	g_vecIndexData.push_back(index);

	index.index = 5;
	g_vecIndexData.push_back(index);
	// ===============================================
	// 윗면 삼각형 2개
	index.index = 0;
	g_vecIndexData.push_back(index);

	index.index = 3;
	g_vecIndexData.push_back(index);

	index.index = 2;
	g_vecIndexData.push_back(index);
	// -----------------------------------------------
	index.index = 0;
	g_vecIndexData.push_back(index);

	index.index = 2;
	g_vecIndexData.push_back(index);

	index.index = 1;
	g_vecIndexData.push_back(index);
	// ===============================================
	// 아랫면 삼각형 2개
	index.index = 4;
	g_vecIndexData.push_back(index);

	index.index = 6;
	g_vecIndexData.push_back(index);

	index.index = 7;
	g_vecIndexData.push_back(index);
	// -----------------------------------------------
	index.index = 4;
	g_vecIndexData.push_back(index);

	index.index = 5;
	g_vecIndexData.push_back(index);

	index.index = 6;
	g_vecIndexData.push_back(index);
	// ===============================================
	// 뒷면 삼각형 2개
	index.index = 3;
	g_vecIndexData.push_back(index);

	index.index = 6;
	g_vecIndexData.push_back(index);

	index.index = 2;
	g_vecIndexData.push_back(index);
	// -----------------------------------------------
	index.index = 3;
	g_vecIndexData.push_back(index);

	index.index = 7;
	g_vecIndexData.push_back(index);

	index.index = 6;
	g_vecIndexData.push_back(index);

	// 인덱스 버퍼로 큐브 하나를 렌더링하기 위해 정점 8개가 필요합니다.
	// 정점 8개를 이용해서 그리는 순서를 인덱스에 넣어주면 됩니다.
	// 인덱스 버퍼는 D3DPT_TRIANGLELIST가 기본이고 폴리곤 개수를 설정해줘야 합니다.
	// 큐브 하나를 렌더링하기 위해서는 폴리곤이 최소 12개 필요합니다.
}
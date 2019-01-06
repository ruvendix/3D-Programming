#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


// ====================================================================================
// 전역 변수 선언부입니다.
HRESULT g_DXResult = S_OK;

namespace
{
	IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
	IDirect3DIndexBuffer9*  g_pIndexBuffer  = nullptr;

	std::vector<VertexP3D>  g_vecP3D;         // 정점 정보입니다.
	std::vector<Index16>    g_vecIndex16;     // 인덱스 정보입니다.
	D3DXVECTOR3             g_vBaseVertex[8]; // 기준 정점 정보입니다.

	// 원래는 벡터로만 사용되는데 이번에는 FLOAT 3개를 묶은 것으로 봅니다.
	D3DXVECTOR3 g_rotateAngle;
}

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
	D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	//D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);

	// 정점 크기 설정입니다.
	// FLOAT을 DWORD로 해석해야 하므로 DWORD*로 변경한 뒤에 값을 참조해야 합니다.
	//FLOAT rPointSize = 20.0f;
	//D3DDEVICE9->SetRenderState(D3DRS_POINTSIZE, *reinterpret_cast<DWORD*>(&rPointSize));

	D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	//D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	
	// ==========================================================
	// 뷰행렬을 설정합니다.
	D3DXVECTOR3 vEye(4.0f, 4.0f, -4.0f);
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);

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

	return S_OK;
}

HRESULT CALLBACK OnUpdate()
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

	// 각도 보정
	g_rotateAngle.z = RX::AdjustAngle(g_rotateAngle.z);
	g_rotateAngle.x = RX::AdjustAngle(g_rotateAngle.x);
	g_rotateAngle.y = RX::AdjustAngle(g_rotateAngle.y);

	// 회전행렬입니다. 순서는 Z -> X -> Y입니다.
	// 즉, Roll -> Pitch -> Yaw입니다.
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot,
		D3DXToRadian(g_rotateAngle.y), D3DXToRadian(g_rotateAngle.x),
		D3DXToRadian(g_rotateAngle.z));
	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matRot);

	return S_OK;
}

HRESULT CALLBACK OnRender()
{
	D3DDEVICE9->SetFVF(VertexP3D::format);
	D3DDEVICE9->SetStreamSource(
		0,
		g_pVertexBuffer,
		0,
		sizeof(VertexP3D));

	if (g_pIndexBuffer == nullptr)
	{
		// 렌더링할 개수(큐브는 삼각형 12개)
		g_DXResult = D3DDEVICE9->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 12);
	}
	else
	{
		D3DDEVICE9->SetIndices(g_pIndexBuffer); // 인덱스 버퍼 연결
		D3DDEVICE9->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST, // 인덱스 버퍼는 D3DPT_TRIANGLELIST로 고정
			0,   // 첫 인덱스가 될 정점 버퍼의 정점 인덱스
			0,   // 사용할 첫 인덱스(인덱스가 0, 1, 2, 3이 있을 때 3이면 3부터 시작)
			8,   // 사용할 정점 개수(전체 정점 개수 - (2번째 인자 + 3번째 인자))
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

	INT32 vertexCnt = g_vecP3D.size();
	if (vertexCnt > 0)
	{
		g_DXResult = D3DDEVICE9->CreateVertexBuffer(sizeof(VertexP3D) * vertexCnt,
			D3DUSAGE_WRITEONLY, VertexP3D::format, D3DPOOL_MANAGED, &g_pVertexBuffer, nullptr);
		DXERR_HANDLER(g_DXResult);
		NULLCHK_RETURN(g_pVertexBuffer, "정점 버퍼 생성 실패했습니다!");

		void* pVertexData = nullptr;
		g_pVertexBuffer->Lock(0, sizeof(VertexP3D) * vertexCnt,
			&pVertexData, D3DLOCK_READONLY);

		// 벡터를 이렇게 복사할 수 있는데 표준 정의입니다.
		// 즉, 벡터는 연속된 메모리라는 보장이 있는 건데
		// 만약 불안하다면 배열로 변경해도 괜찮습니다.
		::CopyMemory(pVertexData, &g_vecP3D[0], sizeof(VertexP3D) * vertexCnt);

		g_pVertexBuffer->Unlock();
	}
	
	INT32 indexCnt = g_vecIndex16.size();
	if (indexCnt > 0)
	{
		g_DXResult = D3DDEVICE9->CreateIndexBuffer(
			sizeof(Index16) * indexCnt, // 인덱스 버퍼의 용량입니다.
			D3DUSAGE_WRITEONLY, // 사용법인데 0이 기본이지만 최적화를 위해 쓰기 전용으로 설정합니다.
			Index16::format,    // 인덱스 형식
			D3DPOOL_MANAGED,    // 메모리풀 설정입니다.
			&g_pIndexBuffer,    // 인덱스 버퍼의 포인터를 넘깁니다.
			nullptr);           // nullptr로 설정합니다.
		DXERR_HANDLER(g_DXResult);
		NULLCHK_RETURN(g_pIndexBuffer, "인덱스 버퍼 생성 실패했습니다!");

		void* pIndexData = nullptr;
		g_pIndexBuffer->Lock(
			0, // 오프셋(Offset), 즉 시작 위치를 의미하는데 전체 잠금은 0입니다.
			sizeof(Index16) * indexCnt, // 복사할 인덱스 정보의 용량을 넘겨줍니다.
			&pIndexData, // 복사된 정점 정보를 다룰 수 있는 포인터를 설정해줍니다.
			D3DLOCK_READONLY); // 잠금 플래그는 D3DLOCK_READONLY로 설정합니다.

		// 벡터를 이렇게 복사할 수 있는데 표준 정의입니다.
		// 즉, 벡터는 연속된 메모리라는 보장이 있는 건데
		// 만약 불안하다면 배열로 변경해도 괜찮습니다.
		::CopyMemory(pIndexData, &g_vecIndex16[0], sizeof(Index16) * indexCnt);

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
	VertexP3D vertex;
	vertex.vPos    = g_vBaseVertex[0];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[1];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[5];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);
	// -----------------------------------------------
	vertex.vPos    = g_vBaseVertex[0];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[5];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[4];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);
	// ===============================================
	// 왼쪽 측면 삼각형 2개
	vertex.vPos    = g_vBaseVertex[0];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[4];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[3];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);
	// -----------------------------------------------
	vertex.vPos    = g_vBaseVertex[3];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[4];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[7];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);
	// ===============================================
	// 오른쪽 측면 삼각형 2개
	vertex.vPos    = g_vBaseVertex[1];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[2];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[5];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);
	// -----------------------------------------------
	vertex.vPos    = g_vBaseVertex[2];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[6];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[5];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);
	// ===============================================
	// 윗면 삼각형 2개
	vertex.vPos    = g_vBaseVertex[0];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[3];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[2];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);
	// -----------------------------------------------
	vertex.vPos    = g_vBaseVertex[0];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[2];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[1];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);
	// ===============================================
	// 아랫면 삼각형 2개
	vertex.vPos    = g_vBaseVertex[4];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[6];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[7];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);
	// -----------------------------------------------
	vertex.vPos    = g_vBaseVertex[4];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[5];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[6];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);
	// ===============================================
	// 뒷면 삼각형 2개
	vertex.vPos    = g_vBaseVertex[3];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[6];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[2];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);
	// -----------------------------------------------
	vertex.vPos    = g_vBaseVertex[3];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[7];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[6];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	// 정점 버퍼로 큐브 하나를 렌더링하기 위해 정점 36개가 필요합니다.
	// 중복되는 정점이 많으므로 최적화가 필요합니다.
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
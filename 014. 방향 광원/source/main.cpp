#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"

// ====================================================================================
// 전역 변수 선언부입니다.
HRESULT g_DXResult = S_OK;

namespace
{
	IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
	IDirect3DIndexBuffer9*  g_pIndexBuffer = nullptr;

	std::vector<VertexP3N> g_vecP3N;
	std::vector<Index16>   g_vecIndex16;
	D3DXVECTOR3            g_vBaseVertex[8];
	D3DXVECTOR3            g_vCubeTriangleNormal[12];

	// 재질은 빛의 반사 및 흡수를 설정하는 개념입니다.
	// 검정(0.0f)부터 흰색(1.0f)까지 색상의 비율을 설정할 수 있습니다.
	D3DXMATERIAL g_mtrl;

	// 광원 구조체입니다.
	// 이번에는 방향 광원으로 사용합니다.
	D3DLIGHT9 g_directionalLight;
}


// ====================================================================================
// 함수 선언부입니다.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

void InputKeyboard();
void CalcTriangleNormal();
void CreateCube(FLOAT rPoint1, FLOAT rPoint2);
void InsertBaseVertex(FLOAT rPoint1, FLOAT rPoint2);
void InitCubeVertexAndIndex(FLOAT rPoint1, FLOAT rPoint2);
void AdjustColorRange(D3DCOLORVALUE* pColor);


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
	
	// ==========================================================
	// 뷰행렬을 설정합니다.
	D3DXVECTOR3 vEye(0.0f, 0.0f, -8.0f);   // 카메라의 위치
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

	RX::ShowMouseCursor(true);

	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, true);

	// 법선벡터를 자동으로 계산해주는 설정입니다.
	// 단! 이 설정을 이용하게 되면 사양을 좀 탑니다...
	// 디폴트는 FALSE입니다.
	D3DDEVICE9->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// 광원을 초기화합니다.
	g_directionalLight.Type = D3DLIGHT_DIRECTIONAL;
	g_directionalLight.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// 빛의 방향은 -> 이렇게 됩니다. 빛의 방향을 잘 이해해야 해요.
	D3DXVECTOR3 vDir = { 1.0f, 0.0f, 0.0f };
	D3DXVec3Normalize(&vDir, &vDir);
	g_directionalLight.Direction = vDir;

	// 광원은 여러 개 설치 가능합니다.
	// 하나만 설치하므로 0번 인덱스의 광원을 활성화시킵니다.
	D3DDEVICE9->LightEnable(99, TRUE);

	// 광원을 가상 디바이스에 등록해줍니다.
	D3DDEVICE9->SetLight(100, &g_directionalLight);

	// 재질을 빨강으로 초기화합니다.
	::ZeroMemory(&g_mtrl, sizeof(g_mtrl));
	g_mtrl.MatD3D.Diffuse.r = 1.0f;

	return S_OK;
}

HRESULT CALLBACK OnUpdate()
{
	InputKeyboard();
	return S_OK;
}

HRESULT CALLBACK OnRender()
{
	D3DDEVICE9->SetMaterial(&g_mtrl.MatD3D);

	D3DDEVICE9->SetFVF(VertexP3N::format);
	D3DDEVICE9->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(VertexP3N));
	D3DDEVICE9->SetIndices(g_pIndexBuffer);

	D3DDEVICE9->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	SAFE_RELEASE(g_pIndexBuffer);
	SAFE_RELEASE(g_pVertexBuffer);
	return S_OK;
}

#pragma region 중복되는 이전 코드
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

		D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	rAngleZ = RX::AdjustAngle(rAngleZ);
	rAngleX = RX::AdjustAngle(rAngleX);
	rAngleY = RX::AdjustAngle(rAngleY);

	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot,
		D3DXToRadian(rAngleY), D3DXToRadian(rAngleX), D3DXToRadian(rAngleZ));
	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matRot);

	if (::GetAsyncKeyState('Z') & 0x8000)
	{
		D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	if (::GetAsyncKeyState('X') & 0x8000)
	{
		D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}
}

void CreateCube(FLOAT rPoint1, FLOAT rPoint2)
{
	if (rPoint1 > rPoint2)
	{
		std::swap(rPoint1, rPoint2);
	}

	InsertBaseVertex(rPoint1, rPoint2);
	InitCubeVertexAndIndex(rPoint1, rPoint2);

	INT32 vertexCnt = g_vecP3N.size();
	g_DXResult = D3DDEVICE9->CreateVertexBuffer(sizeof(VertexP3N) * vertexCnt, D3DUSAGE_WRITEONLY,
		VertexP3N::format, D3DPOOL_MANAGED, &g_pVertexBuffer, nullptr);
	DXERR_HANDLER(g_DXResult);
	NULLCHK_RETURN(g_pVertexBuffer, "정점 버퍼 생성 실패했습니다!");

	void* pVertexData = nullptr;
	g_pVertexBuffer->Lock(0, sizeof(VertexP3N) * vertexCnt, &pVertexData, D3DLOCK_READONLY);
	::CopyMemory(pVertexData, &g_vecP3N[0], sizeof(VertexP3N) * vertexCnt);
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
	// -----------------------------------------------

	// 삼각형에서의 법선벡터를 구합니다.
	CalcTriangleNormal();

	// 정점의 법선벡터는 여러 개를 구한 뒤에 평균을 구해야 하지만
	// 지금은 정점에 법선벡터 하나만 넣어봅니다.
	INT32 size = g_vecP3N.size();
	for (INT32 i = 0; i < size; ++i)
	{
		g_vecP3N[i].vN = g_vCubeTriangleNormal[2];
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
#pragma endregion
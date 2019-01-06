#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"
#include "RX\RX3DAxisDX9.h"


// ====================================================================================
// 전역 상수 선언부입니다.
const INT32 TERRAIN_ROW      = 15;   // 지형의 행(격자 기준입니다)
const INT32 TERRAIN_COLUMN   = 15;   // 지형의 열(격자 기준입니다)
const FLOAT TERRAIN_DISTANCE = 0.4f; // 격자 간의 거리


// ====================================================================================
// 전역 변수 선언부입니다.
HRESULT g_DXResult = S_OK;

namespace
{
	IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
	IDirect3DIndexBuffer9*  g_pIndexBuffer = nullptr;

	INT32 g_terrainVertexCnt; // 지형의 정점 개수
	INT32 g_terrainIndexCnt;  // 지형의 인덱스 개수

	std::vector<VertexP3N> g_vecTerrainVertex; // 지형의 정점 정보
	std::vector<Index16>   g_vecTerrainIndex;  // 지형의 인덱스 정보

	RX::RX3DAxisDX9 g_axis; // 3D축을 다루기 위한 것

	D3DLIGHT9 g_light;
}


// ====================================================================================
// 함수 선언부입니다.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

void DefaultViewAndProjection();
void DefaultLight();
void DefaultRenderState();
void CreateTerrainVertex();
void CreateTerrainIndex();
void CalcTerrainNormalVector();
void OnUserInput();


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

	// 메모리 할당 순서를 이용해서 메모리 누수를 찾습니다.
	// 평소에는 주석 처리하면 됩니다.
	//_CrtSetBreakAlloc(451);

	RXMAIN_DX9->RunMainRoutine(hInstance, IDI_RUVENDIX_ICO);
	return RXMAIN_DX9->getMessageCode();
}

HRESULT CALLBACK OnInit()
{
	CreateTerrainVertex();
	CreateTerrainIndex();

	DefaultViewAndProjection();
	DefaultLight();
	DefaultRenderState();

	g_axis.CreateAxis(20.0f);

	RX::ShowMouseCursor(true);

	return S_OK;
}

HRESULT CALLBACK OnUpdate()
{
	OnUserInput();

	D3DDEVICE9->SetLight(0, &g_light);

	return S_OK;
}

HRESULT CALLBACK OnRender()
{
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_axis.DrawAxis();
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, TRUE);

	D3DDEVICE9->SetFVF(VertexP3N::format);
	D3DDEVICE9->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(VertexP3N));
	D3DDEVICE9->SetIndices(g_pIndexBuffer);

	D3DDEVICE9->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
		g_terrainVertexCnt, 0, g_terrainIndexCnt / 3);

	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matTrans;
	D3DXMATRIXA16 matRotateZ;

	// 현재 적용된 월드행렬을 가져옵니다.
	D3DDEVICE9->GetTransform(D3DTS_WORLD, &matWorld);

	// 왼쪽 벽
	D3DXMatrixTranslation(&matTrans, -3.1f, 3.1f, 0.0f);
	D3DXMatrixRotationZ(&matRotateZ, D3DXToRadian(-90.0f));
	D3DDEVICE9->SetTransform(D3DTS_WORLD, &(matRotateZ * matTrans));
	D3DDEVICE9->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
		g_terrainVertexCnt, 0, g_terrainIndexCnt / 3);

	// 위쪽 벽
	D3DXMatrixTranslation(&matTrans, 0.0f, 6.2f, 0.0f);
	D3DXMatrixRotationZ(&matRotateZ, D3DXToRadian(180.0f));
	D3DDEVICE9->SetTransform(D3DTS_WORLD, &(matRotateZ * matTrans));
	D3DDEVICE9->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
		g_terrainVertexCnt, 0, g_terrainIndexCnt / 3);

	// 오른쪽 벽
	D3DXMatrixTranslation(&matTrans, 3.1f, 3.1f, 0.0f);
	D3DXMatrixRotationZ(&matRotateZ, D3DXToRadian(90.0f));
	D3DDEVICE9->SetTransform(D3DTS_WORLD, &(matRotateZ * matTrans));
	D3DDEVICE9->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
		g_terrainVertexCnt, 0, g_terrainIndexCnt / 3);

	// 현재 적용된 월드행렬을 가져옵니다.
	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matWorld);

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	g_vecTerrainVertex.clear();
	g_vecTerrainIndex.clear();

	g_axis.Release();

	SAFE_RELEASE(g_pIndexBuffer);
	SAFE_RELEASE(g_pVertexBuffer);
	return S_OK;
}

void DefaultLight()
{
	// 조명을 생성하고 등록합니다.
	// 이번에는 점 광원을 이용합니다.
	::ZeroMemory(&g_light, sizeof(g_light));
	g_light.Type = D3DLIGHT_POINT;
	g_light.Position = D3DXVECTOR3(0.0f, 3.1f, 0.0f); // 점 광원은 위치가 존재합니다.
	g_light.Range = 3.6f; // 점 광원은 범위가 존재합니다.
	g_light.Diffuse = D3DXCOLOR(DXCOLOR_WHITE);

	// 점 광원에는 빛이 약해지는 비율인 감쇠라는 게 있습니다.
	g_light.Attenuation0 = 0.15f; // 감쇠 1단계입니다.
	g_light.Attenuation1 = 0.15f; // 감쇠 2단계입니다.
	g_light.Attenuation2 = 0.0f;  // 감쇠 3단계입니다.

	D3DDEVICE9->SetLight(0, &g_light);
	D3DDEVICE9->LightEnable(0, TRUE);

	D3DXMATERIAL mtrl;
	::ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.MatD3D.Ambient = D3DXCOLOR(DXCOLOR_BLUE);
	mtrl.MatD3D.Diffuse = D3DXCOLOR(DXCOLOR_RED);

	D3DDEVICE9->SetMaterial(&(mtrl.MatD3D));
}

void OnUserInput()
{
	if (::GetAsyncKeyState('Z') & 0x8000)
	{
		D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	if (::GetAsyncKeyState('X') & 0x8000)
	{
		D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}

	if (::GetAsyncKeyState('C') & 0x8000)
	{
		D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
	}

	if (::GetAsyncKeyState('F') & 0x0001)
	{
		g_axis.FlipEnable();
	}

	FLOAT rDeltaSeconds = RX::RXProgramFPSMgr::Instance()->getTimer()->getDeltaSeconds();
	if (::GetAsyncKeyState('D') & 0x8000)
	{
		g_light.Position.x += 6.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('A') & 0x8000)
	{
		g_light.Position.x -= 6.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('W') & 0x8000)
	{
		g_light.Position.y += 6.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('S') & 0x8000)
	{
		g_light.Position.y -= 6.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('Q') & 0x8000)
	{
		g_light.Position.z += 6.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('E') & 0x8000)
	{
		g_light.Position.z -= 6.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('R') & 0x8000)
	{
		g_light.Position = D3DXVECTOR3(0.0f, 3.1f, 0.0f);
	}
}

#pragma region 중복되는 이전 코드
void DefaultViewAndProjection()
{
	// ==========================================================
	// 뷰행렬을 설정합니다.
	D3DXVECTOR3 vEye(0.0f, 3.0f, -11.0f);  // 카메라의 위치
	D3DXVECTOR3 vLookAt(0.0f, 3.1f, 0.0f); // 카메라가 보는 지점
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
}

void DefaultRenderState()
{
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, TRUE);
	D3DDEVICE9->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	D3DDEVICE9->SetRenderState(D3DRS_AMBIENT, DXCOLOR_MAGENTA);
}

void CreateTerrainVertex()
{
	D3DXVECTOR3 vStart;
	vStart.x = -(TERRAIN_COLUMN * TERRAIN_DISTANCE * 0.5f);
	vStart.y = 0.1f;
	vStart.z = (TERRAIN_COLUMN * TERRAIN_DISTANCE * 0.5f);

	g_terrainVertexCnt = (TERRAIN_ROW + 1) * (TERRAIN_COLUMN + 1);
	g_vecTerrainVertex.reserve(g_terrainVertexCnt);

	for (INT32 row = 0; row < TERRAIN_ROW + 1; ++row)
	{
		for (INT32 col = 0; col < TERRAIN_COLUMN + 1; ++col)
		{
			INT32 idx = row * (TERRAIN_COLUMN + 1) + col;
			VertexP3N vertex;

			vertex.vPos.x = vStart.x + (col * TERRAIN_DISTANCE);
			vertex.vPos.y = vStart.y;
			//vertex.vPos.y = vStart.y + ((rand() / (float)RAND_MAX) * 0.4f);
			vertex.vPos.z = vStart.z + -(row * TERRAIN_DISTANCE);

			g_vecTerrainVertex.push_back(vertex);
		}
	}

	CalcTerrainNormalVector();

	g_DXResult = D3DDEVICE9->CreateVertexBuffer(g_terrainVertexCnt * sizeof(VertexP3N),
		D3DUSAGE_WRITEONLY, VertexP3N::format, D3DPOOL_MANAGED, &g_pVertexBuffer, nullptr);
	DXERR_HANDLER(g_DXResult);
	NULLCHK_RETURN(g_pVertexBuffer, "정점 버퍼 생성 실패했습니다!");

	void* pVertices = nullptr;
	g_pVertexBuffer->Lock(0, g_terrainVertexCnt * sizeof(VertexP3N), &pVertices, D3DLOCK_READONLY);
	::CopyMemory(pVertices, &g_vecTerrainVertex[0], g_terrainVertexCnt * sizeof(VertexP3N));
	g_pVertexBuffer->Unlock();
}

void CreateTerrainIndex()
{
	g_terrainIndexCnt = TERRAIN_ROW * TERRAIN_COLUMN * 6;
	g_vecTerrainIndex.reserve(g_terrainIndexCnt);

	for (INT32 row = 0; row < TERRAIN_ROW; ++row)
	{
		for (INT32 col = 0; col < TERRAIN_COLUMN; ++col)
		{
			Index16 idx;
			WORD baseIndices[6];

			baseIndices[0] = row * (TERRAIN_COLUMN + 1) + col;
			idx.index = baseIndices[0];
			g_vecTerrainIndex.push_back(idx);

			baseIndices[1] = (row + 1) * (TERRAIN_COLUMN + 1) + col + 1;
			idx.index = baseIndices[1];
			g_vecTerrainIndex.push_back(idx);

			baseIndices[2] = (row + 1) * (TERRAIN_COLUMN + 1) + col;
			idx.index = baseIndices[2];
			g_vecTerrainIndex.push_back(idx);

			baseIndices[3] = baseIndices[0];
			idx.index = baseIndices[3];
			g_vecTerrainIndex.push_back(idx);

			baseIndices[4] = baseIndices[3] + 1;
			idx.index = baseIndices[4];
			g_vecTerrainIndex.push_back(idx);

			baseIndices[5] = baseIndices[1];
			idx.index = baseIndices[5];
			g_vecTerrainIndex.push_back(idx);
		}
	}

	g_DXResult = D3DDEVICE9->CreateIndexBuffer(g_terrainIndexCnt * sizeof(Index16),
		D3DUSAGE_WRITEONLY, Index16::format, D3DPOOL_MANAGED, &g_pIndexBuffer, nullptr);
	DXERR_HANDLER(g_DXResult);
	NULLCHK_RETURN(g_pIndexBuffer, "인덱스 버퍼 생성 실패했습니다!");

	void* pIndices = nullptr;
	g_pIndexBuffer->Lock(0, g_terrainIndexCnt * sizeof(Index16), &pIndices, D3DLOCK_READONLY);
	::CopyMemory(pIndices, &g_vecTerrainIndex[0], g_terrainIndexCnt * sizeof(Index16));
	g_pIndexBuffer->Unlock();
}

void CalcTerrainNormalVector()
{
	for (INT32 row = 0; row < TERRAIN_ROW; ++row)
	{
		for (INT32 col = 0; col < TERRAIN_COLUMN; ++col)
		{
			D3DXVECTOR3 vNormal;
			INT32 baseIdx   = row * (TERRAIN_COLUMN + 1) + col;
			INT32 firstIdx  = (row + 1) * (TERRAIN_COLUMN + 1) + col + 1;
			INT32 secondIdx = (row + 1) * (TERRAIN_COLUMN + 1) + col;

			vNormal = RX::CalcNormalVector(g_vecTerrainVertex[baseIdx].vPos,
				g_vecTerrainVertex[firstIdx].vPos, g_vecTerrainVertex[secondIdx].vPos);
			
			g_vecTerrainVertex[baseIdx].vN   = vNormal;
			g_vecTerrainVertex[firstIdx].vN  = vNormal;
			g_vecTerrainVertex[secondIdx].vN = vNormal;

			secondIdx = firstIdx;
			firstIdx  = baseIdx + 1;

			vNormal = RX::CalcNormalVector(g_vecTerrainVertex[baseIdx].vPos,
				g_vecTerrainVertex[firstIdx].vPos, g_vecTerrainVertex[secondIdx].vPos);
		}
	}
}
#pragma endregion
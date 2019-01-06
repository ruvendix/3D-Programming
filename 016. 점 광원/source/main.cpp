#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


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

	std::vector<VertexP3N>   g_vecTerrainVertex; // 지형의 정점 정보
	std::vector<Index16>     g_vecTerrainIndex;  // 지형의 인덱스 정보

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

	// 광원을 등록하고 활성화시킵니다.
	D3DDEVICE9->SetLight(0, &g_light);
	D3DDEVICE9->LightEnable(0, TRUE);

	// 재질은 하나만 등록 가능합니다.
	// 따라서 재질을 자주 바꿔가며 렌더링할 때가 많습니다.
	D3DXMATERIAL mtrl;
	::ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.MatD3D.Ambient = D3DXCOLOR(DXCOLOR_BLUE);
	mtrl.MatD3D.Diffuse = D3DXCOLOR(DXCOLOR_RED);

	D3DDEVICE9->SetMaterial(&(mtrl.MatD3D));
}

void DefaultRenderState()
{
	// rhw를 사용하지 않는다면 변환 이전의 공간좌표를
	// 사용하게 되므로 각종 변환 과정을 거쳐야 합니다.
	// 조명(라이팅, Lighting)도 그중 하나인데
	// 이번에는 조명을 사용하므로 조명을 켜줍니다.
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, TRUE);

	// 필 모드를 설정합니다. 디폴트는 솔리드입니다.
	D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	//D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);

	// 컬링 모드를 설정합니다. 디폴트는 반시계방향 컬링입니다.
	// 큐브를 확인하기 위해서는 컬링 모드를 무시해야 합니다.
	D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	//D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// 법선벡터를 자동으로 계산해주는 설정입니다.
	// 단! 이 설정을 이용하게 되면 사양을 좀 탑니다...
	// 디폴트는 FALSE입니다.
	D3DDEVICE9->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// 전역 주변광을 설정합니다.
	D3DDEVICE9->SetRenderState(D3DRS_AMBIENT, DXCOLOR_MAGENTA);
}

void CreateTerrainVertex()
{
	// 초기 정점 위치를 설정하는데 Y축을 제외한 X, Z축으로만 판단합니다.
	// Y축은 높이 개념으로 사용되기 때문인데 정해진 건 아닙니다.
	// 참고로 언리얼 엔진에서는 Z축이 높이 개념으로 사용됩니다.
	//
	// 초기 정점 위치는 다음과 같이 설정됩니다.
	// X = -1 * (격자 열 개수 * 격자 간의 거리 * 0.5)
	// Y = 0 또는 높이값
	// Z = +1 * (격자 행 개수 * 격자 간의 거리 * 0.5)
	// X, Z축 기준으로 지형의 중점을 원점에 맞추는 공식입니다.
	// 왼손좌표계의 Z축을 생각하면 Z축이 +가 되는 이유를 알 수 있습니다.
	//
	// 지형의 중점을 원점이 아닌 곳으로 두고 싶으면
	// 초기 정점 위치를 구한 뒤에 조정해주면 됩니다.
	D3DXVECTOR3 vStart;
	vStart.x = -(TERRAIN_COLUMN * TERRAIN_DISTANCE * 0.5f);
	vStart.y = 0.1f;
	vStart.z = (TERRAIN_COLUMN * TERRAIN_DISTANCE * 0.5f);

	// 지형으로 등록할 정점 개수를 구합니다.
	// 격자 기준으로 ((행 + 1) * (열 + 1))입니다.
	// 인덱스 버퍼를 사용하지 않는다면 (행 * 열 * 3 * 2)이렇게 해야 합니다.
	// 사각형 면을 렌더링하려면 정점 6개가 필요한데 이렇게 하면 중복된 정점들이 생깁니다.
	g_terrainVertexCnt = (TERRAIN_ROW + 1) * (TERRAIN_COLUMN + 1);

	// 정점 정보를 정점 버퍼에 등록하므로
	// 정점 정보는 1회용으로만 사용해도 됩니다.
	g_vecTerrainVertex.reserve(g_terrainVertexCnt);

	// 초기 정점 위치를 이용해서 정점 정보를 넣습니다.
	// 격자의 행과 열에 격자 간의 거리를 적용하면 쉽게 넣을 수 있습니다.
	// 정점 개수는 격자 개수보다 1만큼 많으므로 반복문 횟수에 주의해야 합니다.
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

	// 정점의 법선벡터를 구합니다.
	CalcTerrainNormalVector();

	// 정점 버퍼를 생성합니다.
	g_DXResult = D3DDEVICE9->CreateVertexBuffer(
		g_terrainVertexCnt * sizeof(VertexP3N), // 용량
		D3DUSAGE_WRITEONLY, // 쓰기전용(속도 향상)
		VertexP3N::format,  // FVF
		D3DPOOL_MANAGED,    // 메모리풀(시스템 + VRAM)
		&g_pVertexBuffer,   // 정점 버퍼 포인터의 주소
		nullptr); // 안 쓰는 값
	DXERR_HANDLER(g_DXResult);

	void* pVertices = nullptr;

	// 정점 정보를 정점 버퍼에 넣습니다.
	g_DXResult = g_pVertexBuffer->Lock(
		0, // 오프셋
		g_terrainVertexCnt * sizeof(VertexP3N), // 용량
		&pVertices,    // 정점 버퍼의 정점 정보를 넣을 시작 주소
		D3DLOCK_READONLY); // 현재는 사용하지 않음
	DXERR_HANDLER(g_DXResult);
	::CopyMemory(pVertices, &g_vecTerrainVertex[0], g_terrainVertexCnt * sizeof(VertexP3N));
	g_pVertexBuffer->Unlock();
}

void CreateTerrainIndex()
{
	// 지형으로 등록할 인덱스 개수를 구합니다.
	// 격자 기준으로 (행 * 열 * 6)입니다.
	// 하나의 면을 그리기 위해서는 인덱스가 6개 필요하기 때문입니다.
	g_terrainIndexCnt = TERRAIN_ROW * TERRAIN_COLUMN * 6;

	// 인덱스 정보를 인덱스 버퍼에 등록하므로
	// 인덱스 정보는 1회용으로만 사용해도 됩니다.
	// 인덱스 위치가 들쑥날쑥이므로 std::vector를 이용합니다.
	g_vecTerrainIndex.reserve(g_terrainIndexCnt);

	// 정점에 들어간 순서를 이용해서 인덱스를 맞춰줍니다.
	// 삼각형 2개를 그려야 하는데 열뿐만 아니라 행도 영향을 끼칩니다.
	// 참고로 대각선 위치는 겹친다는 걸 주의해야 합니다.
	// 인덱스를 설정하는 방법은 다음과 같습니다.
	//
	// 현재 열의 첫번째 기준 위치 -> 다음 행의 두번째 기준 위치 -> 다음 행의 첫번째 기준 위치
	// 현재 열의 첫번째 기준 위치 -> 현재 열의 두번째 기준 위치 -> 다음 행의 두번째 기준 위치
	// 
	// 인덱스는 정점 때와는 달리 격자의 행과 열만큼만 반복해주면 됩니다.
	for (INT32 row = 0; row < TERRAIN_ROW; ++row)
	{
		for (INT32 col = 0; col < TERRAIN_COLUMN; ++col)
		{
			Index16 idx;
			WORD baseIndices[6];

			// 첫번째 인덱스(현재 열의 첫번째 기준 위치)
			baseIndices[0] = row * (TERRAIN_COLUMN + 1) + col;
			idx.index = baseIndices[0];
			g_vecTerrainIndex.push_back(idx);

			// 두번째 인덱스(다음 행의 두번째 기준 위치)
			baseIndices[1] = (row + 1) * (TERRAIN_COLUMN + 1) + col + 1;
			idx.index = baseIndices[1];
			g_vecTerrainIndex.push_back(idx);

			// 세번째 인덱스(다음 행의 첫번째 기준 위치)
			baseIndices[2] = (row + 1) * (TERRAIN_COLUMN + 1) + col;
			idx.index = baseIndices[2];
			g_vecTerrainIndex.push_back(idx);

			// 네번째 인덱스(현재 열의 첫번째 기준 위치)
			baseIndices[3] = baseIndices[0];
			idx.index = baseIndices[3];
			g_vecTerrainIndex.push_back(idx);

			// 다섯번째 인덱스(현재 열의 두번째 기준 위치)
			baseIndices[4] = baseIndices[3] + 1;
			idx.index = baseIndices[4];
			g_vecTerrainIndex.push_back(idx);

			// 여섯번째 인덱스(다음 행의 두번째 기준 위치)
			baseIndices[5] = baseIndices[1];
			idx.index = baseIndices[5];
			g_vecTerrainIndex.push_back(idx);
		}
	}

	// 인덱스 버퍼를 생성합니다.
	g_DXResult = D3DDEVICE9->CreateIndexBuffer(
		g_terrainIndexCnt * sizeof(Index16), // 용량
		D3DUSAGE_WRITEONLY, // 쓰기전용(속도 향상)
		Index16::format,    // 형식
		D3DPOOL_MANAGED,    // 메모리풀(시스템 + VRAM)
		&g_pIndexBuffer,    // 인덱스 버퍼 포인터의 주소
		nullptr); // 안 쓰는 값
	DXERR_HANDLER(g_DXResult);

	void* pIndices = nullptr;

	// 인덱스 정보를 인덱스 버퍼에 넣습니다.
	g_DXResult = g_pIndexBuffer->Lock(
		0, // 오프셋
		g_terrainIndexCnt * sizeof(Index16), // 용량
		&pIndices,     // 인덱스 버퍼의 인덱스 정보를 넣을 시작 주소
		D3DLOCK_READONLY); // 현재는 사용하지 않음
	DXERR_HANDLER(g_DXResult);
	::CopyMemory(pIndices, &g_vecTerrainIndex[0], g_terrainIndexCnt * sizeof(Index16));
	g_pIndexBuffer->Unlock();
}

void CalcTerrainNormalVector()
{
	// 지형의 높이를 생각해서 법선벡터는 정점과 삼각형만 구합니다.
	// 원래 법선벡터의 정의가 한 면, 즉 삼각형에서의 법선벡터를 의미합니다.
	// 한 면에서의 법선벡터는 그 면에 속한 모든 정점의 법선벡터와 동일합니다.
	// 즉, 정점 기준이 아니라 면을 기준으로 봐야 합니다..
	// 삼각형에서의 법선벡터는 (격자 행 개수 * 격자 열 개수 * 2)만큼 존재합니다.
	// 법선벡터의 정의에 의해 삼각형에서의 법선벡터만 구해서 각 정점에 넣어주면
	// 그게 정점에서의 법선벡터가 됩니다. 따라서 정점에서의 법선벡터는 따로 구해줄 필요가 없습니다.
	//
	// 하지만 정점에서의 법선벡터 개수는 인덱스 개수와 똑같다는 것에 주의해야 합니다.
	// 원래 정점을 찍는 방식대로 정점들이 겹치는 형태이기 때문입니다.
	// 지금은 인덱스 버퍼를 사용하므로 정점에서의 법선벡터는 하나만 적용할 수 있지만...
	// 법선벡터 렌더링은 모든 법선벡터를 렌더링해보겠습니다.
	for (INT32 row = 0; row < TERRAIN_ROW; ++row)
	{
		for (INT32 col = 0; col < TERRAIN_COLUMN; ++col)
		{
			// ============================================================================
			// 첫번째 면에서의 법선벡터를 구합니다.
			D3DXVECTOR3 vNormal;
			INT32 baseIdx   = row * (TERRAIN_COLUMN + 1) + col;
			INT32 firstIdx  = (row + 1) * (TERRAIN_COLUMN + 1) + col + 1;
			INT32 secondIdx = (row + 1) * (TERRAIN_COLUMN + 1) + col;

			vNormal = RX::CalcNormalVector(g_vecTerrainVertex[baseIdx].vPos,
				g_vecTerrainVertex[firstIdx].vPos, g_vecTerrainVertex[secondIdx].vPos);
			
			g_vecTerrainVertex[baseIdx].vN   = vNormal;
			g_vecTerrainVertex[firstIdx].vN  = vNormal;
			g_vecTerrainVertex[secondIdx].vN = vNormal;
			// ============================================================================
			// 두번째 면에서의 법선벡터를 구합니다.
			secondIdx = firstIdx;
			firstIdx  = baseIdx + 1;

			vNormal = RX::CalcNormalVector(g_vecTerrainVertex[baseIdx].vPos,
				g_vecTerrainVertex[firstIdx].vPos, g_vecTerrainVertex[secondIdx].vPos);

			// 현재는 첫번째 면에서의 법선벡터만 적용합니다.
			//g_vecTerrainVertex[baseIdx].vN   = vNormal;
			//g_vecTerrainVertex[firstIdx].vN  = vNormal;
			//g_vecTerrainVertex[secondIdx].vN = vNormal;
		}
	}
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
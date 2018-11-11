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
IDirect3DDevice9*       g_pD3DDevice9   = nullptr;
RX::RXMain_DX9*         g_pMainDX       = nullptr;
IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
IDirect3DIndexBuffer9*  g_pIndexBuffer  = nullptr;

D3DXMATRIXA16 g_matViewAndProjection; // 미리 계산해둔 뷰행렬 * 투영행렬
D3DXMATRIXA16 g_matProjection;        // 미리 계산해둔 투영행렬

HRESULT g_DXResult = S_OK;

namespace
{
	INT32 g_terrainVertexCnt; // 지형의 정점 개수
	INT32 g_terrainIndexCnt;  // 지형의 인덱스 개수

	std::vector<VertexP3N>   g_vecTerrainVertex; // 지형의 정점 정보
	std::vector<Index16>     g_vecTerrainIndex;  // 지형의 인덱스 정보

	RX::RX3DAxisDX9 g_axis; // 3D축을 다루기 위한 것
}

// ====================================================================================
// 함수 선언부입니다.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

// 기본 뷰행렬 및 프로젝션행렬을 설정합니다.
void DefaultViewAndProjection();

// 기본 조명을 설정합니다.
void DefaultLight();

// 기본 렌더 스테이트를 설정합니다.
void DefaultRenderState();

// 지형 렌더링을 위한 정점 버퍼를 생성합니다.
void CreateTerrainVertex();

// 지형 렌더링을 위한 인덱스 버퍼를 생성합니다.
void CreateTerrainIndex();

// 지형에서의 법선벡터를 구합니다.
void CalcTerrainNormalVector();

// 사용자의 키보드 또는 마우스 입력에 따른 처리를 합니다.
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

	g_pMainDX = RXNew RX::RXMain_DX9;
	NULLCHK(g_pMainDX);

	g_pMainDX->setSubFunc(OnInit,    SUBFUNC_TYPE::INIT);
	g_pMainDX->setSubFunc(OnUpdate,  SUBFUNC_TYPE::UPDATE);
	g_pMainDX->setSubFunc(OnRender,  SUBFUNC_TYPE::RENDER);
	g_pMainDX->setSubFunc(OnRelease, SUBFUNC_TYPE::RELEASE);

	// 메모리 할당 순서를 이용해서 메모리 누수를 찾습니다.
	// 평소에는 주석 처리하면 됩니다.
	//_CrtSetBreakAlloc(451);

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

	CreateTerrainVertex();
	CreateTerrainIndex();

	DefaultViewAndProjection();

	DefaultLight();
	DefaultRenderState();

	// 축을 생성합니다.
	g_axis.CreateAxis(20.0f);

	// 마우스 커서를 보여줍니다.
	RX::ShowMouseCursor(true);

	return S_OK;
}

// 업데이트 함수입니다.
// 렌더링에 영향을 주거나 프로그램에 영향을 주는
// 정보들을 매프레임마다 업데이트합니다.
HRESULT CALLBACK OnUpdate()
{
	OnUserInput();
	return S_OK;
}

// 렌더링 함수입니다.
// 실제 렌더링 작업인 Draw Call이 처리됩니다.
// Draw Call은 프레임당 호출되는 렌더링 함수를 뜻하는데 호출되는 빈도수를
// 조사하면 Draw Call Count를 알아낼 수 있습니다.
HRESULT CALLBACK OnRender()
{
	g_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_axis.DrawAxis();
	g_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, TRUE);

	g_pD3DDevice9->SetFVF(VertexP3N::format); // 정점 형식 연결
	g_pD3DDevice9->SetStreamSource(
		0,                  // 사용할 스트림 인덱스
		g_pVertexBuffer,    // 연결할 정점 버퍼
		0,                  // 정점 버퍼의 오프셋
		sizeof(VertexP3N)); // 정점 용량

	g_pD3DDevice9->SetIndices(g_pIndexBuffer); // 인덱스 버퍼 연결
	g_pD3DDevice9->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, // 인덱스 버퍼는 트라이앵글리스트로 고정
		0, // 첫 인덱스가 될 정점 버퍼의 정점 인덱스
		0, // 사용할 첫 인덱스(인덱스가 0, 1, 2, 3이 있을 때 3이면 3부터 시작)
		g_terrainVertexCnt, // 사용할 정점 개수(2번째 인자 + 3번째 인자만큼 뺀 값)
		0, // 인덱스 버퍼의 오프셋
		g_terrainIndexCnt / 3); // 렌더링할 개수(큐브는 삼각형 12개)

	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matTrans;
	D3DXMATRIXA16 matRotateZ;

	// 현재 적용된 월드행렬을 가져옵니다.
	g_pD3DDevice9->GetTransform(D3DTS_WORLD, &matWorld);

	// 왼쪽 벽
	D3DXMatrixTranslation(&matTrans, -3.1f, 3.1f, 0.0f);
	D3DXMatrixRotationZ(&matRotateZ, D3DXToRadian(-90.0f));
	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &(matRotateZ * matTrans));
	g_pD3DDevice9->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, // 인덱스 버퍼는 트라이앵글리스트로 고정
		0, // 첫 인덱스가 될 정점 버퍼의 정점 인덱스
		0, // 사용할 첫 인덱스(인덱스가 0, 1, 2, 3이 있을 때 3이면 3부터 시작)
		g_terrainVertexCnt, // 사용할 정점 개수(2번째 인자 + 3번째 인자만큼 뺀 값)
		0, // 인덱스 버퍼의 오프셋
		g_terrainIndexCnt / 3); // 렌더링할 개수(큐브는 삼각형 12개)

	// 위쪽 벽
	D3DXMatrixTranslation(&matTrans, 0.0f, 6.2f, 0.0f);
	D3DXMatrixRotationZ(&matRotateZ, D3DXToRadian(180.0f));
	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &(matRotateZ * matTrans));
	g_pD3DDevice9->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, // 인덱스 버퍼는 트라이앵글리스트로 고정
		0, // 첫 인덱스가 될 정점 버퍼의 정점 인덱스
		0, // 사용할 첫 인덱스(인덱스가 0, 1, 2, 3이 있을 때 3이면 3부터 시작)
		g_terrainVertexCnt, // 사용할 정점 개수(2번째 인자 + 3번째 인자만큼 뺀 값)
		0, // 인덱스 버퍼의 오프셋
		g_terrainIndexCnt / 3); // 렌더링할 개수(큐브는 삼각형 12개)

	// 오른쪽 벽
	D3DXMatrixTranslation(&matTrans, 3.1f, 3.1f, 0.0f);
	D3DXMatrixRotationZ(&matRotateZ, D3DXToRadian(90.0f));
	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &(matRotateZ * matTrans));
	g_pD3DDevice9->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, // 인덱스 버퍼는 트라이앵글리스트로 고정
		0, // 첫 인덱스가 될 정점 버퍼의 정점 인덱스
		0, // 사용할 첫 인덱스(인덱스가 0, 1, 2, 3이 있을 때 3이면 3부터 시작)
		g_terrainVertexCnt, // 사용할 정점 개수(2번째 인자 + 3번째 인자만큼 뺀 값)
		0, // 인덱스 버퍼의 오프셋
		g_terrainIndexCnt / 3); // 렌더링할 개수(큐브는 삼각형 12개)

	// 현재 적용된 월드행렬을 가져옵니다.
	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &matWorld);

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
	// 뷰행렬을 설정합니다.
	D3DXVECTOR3 vEye(0.0f, 3.0f, -11.0f);  // 카메라의 위치
	D3DXVECTOR3 vLookAt(0.0f, 3.1f, 0.0f); // 카메라가 보는 지점
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // 카메라의 업 벡터

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	g_pD3DDevice9->SetTransform(D3DTS_VIEW, &matView);

	// 투영행렬을 설정합니다.
	D3DXMatrixPerspectiveFovLH(&g_matProjection, (D3DX_PI / 4.0f),
		(static_cast<FLOAT>(g_pMainDX->getClientWidth()) / (g_pMainDX->getClientHeight())),
		1.0f, 1000.0f);
	g_pD3DDevice9->SetTransform(D3DTS_PROJECTION, &g_matProjection);

	// 전역행렬 초기화입니다.
	g_matViewAndProjection = matView * g_matProjection;
}

void DefaultLight()
{
	// 조명을 생성하고 등록합니다.
	// 이번에는 점 광원을 이용합니다.
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_POINT;
	light.Position = D3DXVECTOR3(0.0f, 3.1f, 0.0f); // 점 광원은 위치가 존재합니다.
	light.Range = 3.6f; // 점 광원은 범위가 존재합니다.
	light.Diffuse = D3DXCOLOR(DXCOLOR_WHITE);
	
	// 점 광원에는 빛이 약해지는 비율인 감쇠라는 게 있습니다.
	light.Attenuation0 = 0.15f; // 감쇠 1단계입니다.
	light.Attenuation1 = 0.15f; // 감쇠 2단계입니다.
	light.Attenuation2 = 0.0f;  // 감쇠 3단계입니다.

	// 광원을 등록하고 활성화시킵니다.
	g_pD3DDevice9->SetLight(0, &light);
	g_pD3DDevice9->LightEnable(0, TRUE);

	// 재질은 하나만 등록 가능합니다.
	// 따라서 재질을 자주 바꿔가며 렌더링할 때가 많습니다.
	D3DXMATERIAL mtrl;
	::ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.MatD3D.Ambient = D3DXCOLOR(DXCOLOR_BLUE);
	mtrl.MatD3D.Diffuse = D3DXCOLOR(DXCOLOR_RED);

	g_pD3DDevice9->SetMaterial(&(mtrl.MatD3D));
}

void DefaultRenderState()
{
	// rhw를 사용하지 않는다면 변환 이전의 공간좌표를
	// 사용하게 되므로 각종 변환 과정을 거쳐야 합니다.
	// 조명(라이팅, Lighting)도 그중 하나인데
	// 이번에는 조명을 사용하므로 조명을 켜줍니다.
	g_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, TRUE);

	// 필 모드를 설정합니다. 디폴트는 솔리드입니다.
	g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	//g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);

	// 컬링 모드를 설정합니다. 디폴트는 반시계방향 컬링입니다.
	// 큐브를 확인하기 위해서는 컬링 모드를 무시해야 합니다.
	g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	//g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// 법선벡터를 자동으로 계산해주는 설정입니다.
	// 단! 이 설정을 이용하게 되면 사양을 좀 탑니다...
	// 디폴트는 FALSE입니다.
	g_pD3DDevice9->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// 전역 주변광을 설정합니다.
	g_pD3DDevice9->SetRenderState(D3DRS_AMBIENT, DXCOLOR_MAGENTA);
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
	g_DXResult = g_pD3DDevice9->CreateVertexBuffer(
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
		D3DFLAG_NONE); // 현재는 사용하지 않음
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
	g_DXResult = g_pD3DDevice9->CreateIndexBuffer(
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
		D3DFLAG_NONE); // 현재는 사용하지 않음
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
	if (::GetAsyncKeyState('A') & 0x8000)
	{
		g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	if (::GetAsyncKeyState('S') & 0x8000)
	{
		g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}

	if (::GetAsyncKeyState('D') & 0x8000)
	{
		g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
	}

	if (::GetAsyncKeyState('F') & 0x0001)
	{
		g_axis.FlipEnable();
	}
}
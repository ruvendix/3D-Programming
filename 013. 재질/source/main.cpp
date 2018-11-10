#include "base_project.h"
#include "old_main.h"
#include "global_variable_declaration.h"

// ====================================================================================
// 전역 변수 선언부입니다.
IDirect3DDevice9*       g_pD3DDevice9   = nullptr;
RX::RXMain_DX9*         g_pMainDX       = nullptr;
IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
IDirect3DIndexBuffer9*  g_pIndexBuffer  = nullptr;
ID3DXLine*              g_pLine         = nullptr; // 선을 그리기 위한 것

D3DXMATRIXA16 g_matViewAndProjection; // 미리 계산해둔 뷰행렬 * 투영행렬
D3DXMATRIXA16 g_matProjection;        // 미리 계산해둔 투영행렬
D3DXMATRIXA16 g_matAll; // 월드행렬과 결합하기 위한 전체변환행렬

DWORD   g_dwNormalVectorRenderingFlag = 0;
HRESULT g_DXResult = S_OK;

std::vector<VertexP3D> g_vecP3D;
std::vector<Index16>   g_vecIndex16;
D3DXVECTOR3            g_vBaseVertex[8];
D3DXVECTOR3            g_vCubeTriangleNormal[12]; // 삼각형에서의 법선벡터

// 재질은 빛의 반사 및 흡수를 설정하는 개념입니다.
// 검정(0.0f)부터 흰색(1.0f)까지 색상의 비율을 설정할 수 있습니다.
D3DXMATERIAL g_mtrl;


// ====================================================================================
// 함수 선언부입니다.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

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

	// 뷰행렬을 설정합니다.
	D3DXVECTOR3 vEye(4.0f, 4.0f, -4.0f);   // 카메라의 위치
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // 카메라가 보는 지점
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
	D3DXMatrixIdentity(&g_matAll);
	g_matViewAndProjection = matView * g_matProjection;

	// 선을 그리는 객체를 생성합니다.
	D3DXCreateLine(g_pD3DDevice9, &g_pLine);

	// 삼각형에서의 법선벡터를 구합니다.
	CalcTriangleNormal();

	// 마우스 커서를 보여줍니다.
	RX::ShowMouseCursor(true);

	// rhw를 사용하지 않는다면 변환 이전의 공간좌표를
	// 사용하게 되므로 각종 변환 과정을 거쳐야 합니다.
	// 조명(라이팅, Lighting)도 그중 하나인데
	// 이번에는 조명을 사용하므로 조명을 켜줍니다.
	g_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, true);

	// 컬링 모드를 설정합니다. 디폴트는 반시계방향 컬링입니다.
	// 큐브를 확인하기 위해서는 컬링 모드를 무시해야 합니다.
	//g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// 조명을 확인하기 위해 전역 주변광을 설정합니다.
	g_pD3DDevice9->SetRenderState(D3DRS_AMBIENT, DXCOLOR_WHITE);

	// 재질을 초기화합니다.
	::ZeroMemory(&g_mtrl, sizeof(g_mtrl));

	return S_OK;
}

// 업데이트 함수입니다.
// 렌더링에 영향을 주거나 프로그램에 영향을 주는
// 정보들을 매프레임마다 업데이트합니다.
HRESULT CALLBACK OnUpdate()
{
	InputOldKeyboard();

	// 빨강 ===============================
	if (::GetAsyncKeyState('T') & 0x8000)
	{
		g_mtrl.MatD3D.Ambient.r += 0.05f;
	}

	if (::GetAsyncKeyState('Y') & 0x8000)
	{
		g_mtrl.MatD3D.Ambient.r -= 0.05f;
	}
	// ====================================

	// 초록 ===============================
	if (::GetAsyncKeyState('U') & 0x8000)
	{
		g_mtrl.MatD3D.Ambient.g += 0.05f;
	}

	if (::GetAsyncKeyState('I') & 0x8000)
	{
		g_mtrl.MatD3D.Ambient.g -= 0.05f;
	}
	// ====================================

	// 파랑 ===============================
	if (::GetAsyncKeyState('O') & 0x8000)
	{
		g_mtrl.MatD3D.Ambient.b += 0.05f;
	}

	if (::GetAsyncKeyState('P') & 0x8000)
	{
		g_mtrl.MatD3D.Ambient.b -= 0.05f;
	}
	// ====================================

	AdjustColorRange(&g_mtrl.MatD3D.Ambient);

	return S_OK;
}

// 렌더링 함수입니다.
// 실제 렌더링 작업인 Draw Call이 처리됩니다.
// Draw Call은 프레임당 호출되는 렌더링 함수를 뜻하는데 호출되는 빈도수를
// 조사하면 Draw Call Count를 알아낼 수 있습니다.
HRESULT CALLBACK OnRender()
{
	// 재질을 등록합니다.
	// 재질은 하나만 등록 가능합니다.
	// 따라서 재질을 자주 바꾸면서 렌더링해야 합니다.
	g_pD3DDevice9->SetMaterial(&g_mtrl.MatD3D);

	g_pD3DDevice9->SetFVF(VertexP3D::format); // 정점 형식 연결
	g_pD3DDevice9->SetStreamSource(
		0,                  // 사용할 스트림 인덱스
		g_pVertexBuffer,    // 연결할 정점 버퍼
		0,                  // 정점 버퍼의 오프셋
		sizeof(VertexP3D)); // 정점 용량

	g_pD3DDevice9->SetIndices(g_pIndexBuffer); // 인덱스 버퍼 연결
	g_pD3DDevice9->DrawIndexedPrimitive(
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

void AdjustColorRange(D3DCOLORVALUE* pColor)
{
	if (pColor == nullptr)
	{
		return;
	}

	if (pColor->a < 0.0f)
	{
		pColor->a = 0.0f;
	}
	else if (pColor->a > 1.0f)
	{
		pColor->a = 1.0f;
	}

	if (pColor->r < 0.0f)
	{
		pColor->r = 0.0f;
	}
	else if (pColor->r > 1.0f)
	{
		pColor->r = 1.0f;
	}

	if (pColor->g < 0.0f)
	{
		pColor->g = 0.0f;
	}
	else if (pColor->g > 1.0f)
	{
		pColor->g = 1.0f;
	}

	if (pColor->b < 0.0f)
	{
		pColor->b = 0.0f;
	}
	else if (pColor->b > 1.0f)
	{
		pColor->b = 1.0f;
	}
}

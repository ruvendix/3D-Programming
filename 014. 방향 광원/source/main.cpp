#include "base_project.h"
#include "old_main.h"
#include "global_variable_declaration.h"

// ====================================================================================
// 전역 변수 선언부입니다.
IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
IDirect3DIndexBuffer9*  g_pIndexBuffer  = nullptr;
ID3DXLine*              g_pLine         = nullptr; // 선을 그리기 위한 것

D3DXMATRIXA16 g_matViewAndProjection; // 미리 계산해둔 뷰행렬 * 투영행렬
D3DXMATRIXA16 g_matProjection;        // 미리 계산해둔 투영행렬
D3DXMATRIXA16 g_matAll; // 월드행렬과 결합하기 위한 전체변환행렬

DWORD   g_dwNormalVectorRenderingFlag = 0;
HRESULT g_DXResult = S_OK;

std::vector<VertexP3D> g_vecP3D;
std::vector<VertexP3N> g_vecP3N;
std::vector<Index16>   g_vecIndex16;
D3DXVECTOR3            g_vBaseVertex[8];
D3DXVECTOR3            g_vCubeTriangleNormal[12]; // 삼각형에서의 법선벡터

// 재질은 빛의 반사 및 흡수를 설정하는 개념입니다.
// 검정(0.0f)부터 흰색(1.0f)까지 색상의 비율을 설정할 수 있습니다.
D3DXMATERIAL g_mtrl;

// 광원 구조체입니다.
// 이번에는 방향 광원으로 사용합니다.
D3DLIGHT9 g_directionalLight;


// ====================================================================================
// 함수 선언부입니다.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();


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

	// 뷰행렬을 설정합니다.
	D3DXVECTOR3 vEye(0.0f, 0.0f, -8.0f);   // 카메라의 위치
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

	// 전역행렬 초기화입니다.
	D3DXMatrixIdentity(&g_matAll);
	g_matViewAndProjection = matView * g_matProjection;

	// 선을 그리는 객체를 생성합니다.
	D3DXCreateLine(D3DDEVICE9, &g_pLine);

	// 마우스 커서를 보여줍니다.
	RX::ShowMouseCursor(true);

	// rhw를 사용하지 않는다면 변환 이전의 공간좌표를
	// 사용하게 되므로 각종 변환 과정을 거쳐야 합니다.
	// 조명(라이팅, Lighting)도 그중 하나인데
	// 이번에는 조명을 사용하므로 조명을 켜줍니다.
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, true);

	// 컬링 모드를 설정합니다. 디폴트는 반시계방향 컬링입니다.
	// 큐브를 확인하기 위해서는 컬링 모드를 무시해야 합니다.
	//D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// 법선벡터를 자동으로 계산해주는 설정입니다.
	// 단! 이 설정을 이용하게 되면 사양을 좀 탑니다...
	// 디폴트는 FALSE입니다.
	D3DDEVICE9->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// 광원을 초기화합니다.
	g_directionalLight.Type = D3DLIGHT_DIRECTIONAL;
	g_directionalLight.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

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

// 업데이트 함수입니다.
// 렌더링에 영향을 주거나 프로그램에 영향을 주는
// 정보들을 매프레임마다 업데이트합니다.
HRESULT CALLBACK OnUpdate()
{
	InputOldKeyboard();
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
	D3DDEVICE9->SetMaterial(&g_mtrl.MatD3D);

	D3DDEVICE9->SetFVF(VertexP3N::format); // 정점 형식 연결
	D3DDEVICE9->SetStreamSource(
		0,                  // 사용할 스트림 인덱스
		g_pVertexBuffer,    // 연결할 정점 버퍼
		0,                  // 정점 버퍼의 오프셋
		sizeof(VertexP3N)); // 정점 용량

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
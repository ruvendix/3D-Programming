#include "base_project.h"
#include "global_variable_declaration.h"
#include "main.h"
#include "RX/RX3DAxisDX9.h"

// ====================================================================================
// 매크로 정의부입니다.


// ====================================================================================
// 구조체 및 공용체 선언부입니다.


// ====================================================================================
// 전역 변수 선언부입니다.
namespace
{
	RX::RXMain_DX9*  g_pMainDX = nullptr;
	RX::RX3DAxisDX9* g_p3DAxis = nullptr;
}

extern IDirect3DDevice9* g_pD3DDevice9 = nullptr;
extern HRESULT           g_DXResult    = S_OK;


// ====================================================================================
// 함수 선언부입니다.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();
HRESULT CALLBACK OnLost();
HRESULT CALLBACK OnReset();

void ViewMatrixTest();
void ProjectionMatrixTest();
void ViewPortTest();

void SetVector(D3DXVECTOR3* pV, FLOAT rX, FLOAT rY, FLOAT rZ);


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
	g_pMainDX->setSubFunc(OnRender,  SUBFUNC_TYPE::RENDER);
	g_pMainDX->setSubFunc(OnRelease, SUBFUNC_TYPE::RELEASE);
	g_pMainDX->setSubFunc(OnLost,    SUBFUNC_TYPE::LOSTDEVICE);
	g_pMainDX->setSubFunc(OnReset,   SUBFUNC_TYPE::RESETDEVICE);

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

	g_p3DAxis = RXNew RX::RX3DAxisDX9;
	NULLCHK_HEAPALLOC(g_p3DAxis);
	g_p3DAxis->CreateAxis(0.5f);

	// rhw를 사용하지 않는다면 변환 이전의 공간좌표를 사용하게 되므로
	// 각종 변환 과정을 거쳐야 합니다. 조명(라이팅, Lighting)도 그중 하나인데
	// 조명에 관한 연산을 따로 하지 않았으므로 조명은 꺼줘야 합니다.
	g_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, false);

	return S_OK;
}

// 렌더링 함수입니다.
// 실제 렌더링 작업인 Draw Call이 처리됩니다.
// Draw Call은 프레임당 호출되는 렌더링 함수를 뜻하는데 호출되는 빈도수를
// 조사하면 Draw Call Count를 알아낼 수 있습니다.
HRESULT CALLBACK OnRender()
{
	//ViewMatrixTest();
	//ProjectionMatrixTest();
	ViewPortTest();
	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	SAFE_DELTE(g_p3DAxis);
	return S_OK;
}

// 뷰변환입니다.
// 뷰변환은 월드변환 후에 처리됩니다.
void ViewMatrixTest()
{
	// 뷰변환입니다.
	// 뷰변환은 월드변환 후에 처리됩니다.
	// 카메라를 월드에 놓고 카메라 기준으로 보게 됩니다.
	// 카메라 기준으로 보기 위한 변환이 뷰행렬 변환입니다.
	D3DXVECTOR3 vEye(0.3f, 0.3f, -0.3f);   // 카메라의 위치
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // 카메라가 보는 지점
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // 카메라의 업 벡터

	// ====================================================================================
	// 카메라의 시선 벡터는 vLookAt이 아니라 vLookAt - vEye입니다.
	// vEye나 vLookAt은 위치벡터입니다. vLookAt을 시선 벡터로 보면 안 됩니다.
	// vUp은 업 벡터인데 카메라 축을 구할 때 사용합니다.
	// 카메라 축을 구할 때 외적을 사용하는데 vUp은 이때 사용됩니다.
	// 따라서 vUp은 방향 벡터라고 이해하면 됩니다.
	// ====================================================================================

	// 뷰행렬을 만듭니다.
	// 뷰행렬을 적용할 때는 D3DTS_VIEW를 쓰면 됩니다.
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp); // 왼손좌표계 기준입니다.
	g_pD3DDevice9->SetTransform(D3DTS_VIEW, &matView);

	g_p3DAxis->DrawAxis();
}

// 투영변환입니다.
// 투영변환은 뷰변환 후에 처리됩니다.
// 투영변환은 크게 직교투영, 등각투영, 원근투영이 있습니다.
// 각각의 예는 포스팅을 참고해주세요.
void ProjectionMatrixTest()
{
	// 뷰변환입니다.
	// 뷰변환은 월드변환 후에 처리됩니다.
	// 카메라를 월드에 놓고 카메라 기준으로 보게 됩니다.
	// 카메라 기준으로 보기 위한 변환이 뷰행렬 변환입니다.
	D3DXVECTOR3 vEye(3.0f, 3.0f, -3.0f);   // 카메라의 위치
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // 카메라가 보는 지점
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // 카메라의 업 벡터

	// 뷰행렬을 만듭니다.
	// 뷰행렬을 적용할 때는 D3DTS_VIEW를 쓰면 됩니다.
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	g_pD3DDevice9->SetTransform(D3DTS_VIEW, &matView);
	
	// 종횡비입니다. 가로 / 세로를 의미합니다.
	// 일반 해상도인지 와이드 해상도인지를 구분할 때 사용하는데
	// 투영변환에는 종횡비가 사용됩니다. 일반적으로는 1.0으로 사용합니다.
	FLOAT rAspectRatio =
		static_cast<FLOAT>(g_pMainDX->getClientWidth() / g_pMainDX->getClientHeight());

	// 투영행렬을 만듭니다.
	// 투영행렬을 적용할 때는 D3DTS_PROJECTION을 쓰면 됩니다.
	//
	// 투영행렬에는 FOV(Field Of View)와 Near, Far를 입력해야 하는데
	// FOV는 시야각이라고 해서 눈에 들어오는 범위를 의미합니다.
	// 너무 작으면 보이는 게 별로 없고, 너무 크면 보이는 게 많아지지만...
	// 모니터를 통해서 렌더링된 결과물을 보기 때문에 일반적으로는 45도로 지정합니다.
	// 즉, 180도의 4분의 1을 지정합니다. 그리고 FOV는 Y축 기준으로 설정합니다.
	// Near, Far는 보이기 시작하는 부분과 보이지 않는 부분의 지점을 의미합니다.
	// Near ~ Far가 이루는 공간을 절두체(Frustum)라고 하는데 투영행렬은
	// 절두체를 기준으로 만들어지게 됩니다. 원근투영은 꽤 복잡한 편이죠 ㄷㄷ...
	// 일반적으로는 Near는 1.0으로 설정하고 Far는 크게 잡습니다.
	D3DXMATRIXA16 matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection, D3DX_PI / 4.0f, rAspectRatio, 1.0f, 1000.0f);
	g_pD3DDevice9->SetTransform(D3DTS_PROJECTION, &matProjection);

	g_p3DAxis->DrawAxis();
}

void ViewPortTest()
{
#pragma region LeftTopViewPort
	
	// 뷰변환입니다.
	// 뷰변환은 월드변환 후에 처리됩니다.
	// 카메라를 월드에 놓고 카메라 기준으로 보게 됩니다.
	// 카메라 기준으로 보기 위한 변환이 뷰변환입니다.
	D3DXVECTOR3 vEye(2.0f, 0.0f, 0.0f);    // 카메라의 위치
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // 카메라가 보는 지점
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // 카메라의 업 벡터

	// 뷰행렬을 만듭니다.
	// 뷰행렬을 적용할 때는 D3DTS_VIEW를 쓰면 됩니다.
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	g_pD3DDevice9->SetTransform(D3DTS_VIEW, &matView);
	
	// 종횡비입니다. 가로 / 세로를 의미합니다.
	// 일반 해상도인지 와이드 해상도인지를 구분할 때 사용하는데
	// 투영변환에는 종횡비가 사용됩니다. 일반적으로는 1.0으로 사용합니다.
	FLOAT rAspectRatio =
		static_cast<FLOAT>(g_pMainDX->getClientWidth() / g_pMainDX->getClientHeight());

	// 투영행렬을 만듭니다.
	// 투영행렬을 적용할 때는 D3DTS_PROJECTION을 쓰면 됩니다.
	//
	// 투영행렬에는 FOV(Field Of View)와 Near, Far를 입력해야 하는데
	// FOV는 시야각이라고 해서 눈에 들어오는 범위를 의미합니다.
	// 너무 작으면 보이는 게 별로 없고, 너무 크면 보이는 게 많아지지만...
	// 모니터를 통해서 렌더링된 결과물을 보기 때문에 일반적으로는 45도로 지정합니다.
	// 즉, 180도의 4분의 1을 지정합니다. 그리고 FOV는 Y축 기준으로 설정합니다.
	// Near, Far는 보이기 시작하는 부분과 보이지 않는 부분의 지점을 의미합니다.
	// Near ~ Far가 이루는 공간을 절두체(Frustum)라고 하는데 투영행렬은
	// 절두체를 기준으로 만들어지게 됩니다. 원근투영은 꽤 복잡한 편이죠 ㄷㄷ...
	// 일반적으로는 Near는 1.0으로 설정하고 Far는 크게 잡습니다.
	D3DXMATRIXA16 matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection, D3DX_PI / 4.0f, rAspectRatio, 1.0f, 1000.0f);
	g_pD3DDevice9->SetTransform(D3DTS_PROJECTION, &matProjection);

	// 뷰포트 설정입니다.
	// 뷰포트는 백버퍼와 다른 개념으로서, 백버퍼에 실제로 렌더링할 영역이라고 보면 됩니다.
	// 따라서 뷰포트를 잘 이용하면 하나의 오브젝트를 여러 화면으로 렌더링하는 게 가능합니다.
	D3DVIEWPORT9 viewPort;
	D3DVIEWPORT9 backupViewPort;
	g_pD3DDevice9->GetViewport(&viewPort);
	backupViewPort = viewPort; // 현재 뷰포트 백업

	// 뷰포트의 렌더링 영역을 설정합니다.
	// 화면 4개로 분할할 것이므로 메인 뷰포트는 왼쪽 위가 됩니다.
	// 가로와 세로 길이는 클라이언트 영역의 절반입니다. 즉, 4분의 1 정도가 됩니다.
	viewPort.Width  = g_pMainDX->getClientWidth() / 2;
	viewPort.Height = g_pMainDX->getClientHeight() / 2;

	// 뷰포트 설정을 변경하면 반드시 가상 디바이스에 적용해줘야 합니다.
	g_pD3DDevice9->SetViewport(&viewPort);

	// 결과적으로 렌더링은 마지막에 처리됩니다.
	g_p3DAxis->DrawAxis();

#pragma endregion

	// ====================================================================================
	// 이후부터는 위의 과정을 반복하면 되는데 이전에 렌더링했던 것을 지워줘야 합니다.
	// 그렇지 않으면 이전에 렌더링한 것에 덮어씌워서 렌더링하게 됩니다.
	// 화면 분할을 이용하는 프로그램도 있으므로 잘 알아두면 좋습니다.
	// ====================================================================================

#pragma region RightTopViewPort

	// 카메라 정보를 조정합니다.
	// 카메라의 시선벡터는 카메라의 Z축이 됩니다.
	// 카메라의 X축을 구할 때 왼손좌표계 기준으로 업 벡터와 시선 벡터를 외적합니다.
	// 그 다음 카메라의 Z축과 X축을 외적해서 Y축을 구하게 됩니다.
	// 탑뷰일 때를 생각해보면 업 벡터가 (0, 1, 0)일 때 시선벡터와의 외적은 0이 됩니다.
	// 두 벡터가 이루는 각이 0도 또는 180도라면 외적은 0입니다. (sin 그래프 참고)
	// 따라서 탑뷰를 구현하려면 업 벡터를 조정해야 합니다.
	SetVector(&vEye, 0.0f, 2.0f, 0.0f);
	SetVector(&vUp, 0.0f, 0.0f, -1.0f);
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	g_pD3DDevice9->SetTransform(D3DTS_VIEW, &matView);

	// 뷰포트 영역을 새로 설정해줍니다.
	viewPort.X = g_pMainDX->getClientWidth() / 2;
	g_pD3DDevice9->SetViewport(&viewPort);

	// 3D축을 렌더링합니다.
	// 새로운 객체가 아닌 기존 객체를 그대로 사용합니다.
	g_p3DAxis->DrawAxis();

#pragma endregion
#pragma region LeftBottomViewPort
	
	// 같은 과정을 반복합니다.
	// 카메라 정보를 조정합니다.
	SetVector(&vEye, 0.0f, 0.0f, -2.0f);
	SetVector(&vUp, 0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	g_pD3DDevice9->SetTransform(D3DTS_VIEW, &matView);

	// 뷰포트 영역을 새로 설정해줍니다.
	viewPort.X = 0;
	viewPort.Y = g_pMainDX->getClientHeight() / 2;
	g_pD3DDevice9->SetViewport(&viewPort);

	// 3D축을 렌더링합니다.
	// 새로운 객체가 아닌 기존 객체를 그대로 사용합니다.
	g_p3DAxis->DrawAxis();

#pragma endregion
#pragma region RightBottomViewPort

	// 같은 과정을 반복합니다.
	// 카메라 정보를 조정합니다.
	SetVector(&vEye, 1.0f, 1.0f, -1.0f);
	SetVector(&vUp, 0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	g_pD3DDevice9->SetTransform(D3DTS_VIEW, &matView);

	// 뷰포트 영역을 새로 설정해줍니다.
	viewPort.X = g_pMainDX->getClientWidth() / 2;;
	viewPort.Y = g_pMainDX->getClientHeight() / 2;
	g_pD3DDevice9->SetViewport(&viewPort);

	// 3D축을 렌더링합니다.
	// 새로운 객체가 아닌 기존 객체를 그대로 사용합니다.
	g_p3DAxis->DrawAxis();

#pragma endregion

	// 백업한 뷰포트로 복구해줍니다.
	// 복구해주지 않으면 뷰포트 영역끼리 꼬이게 됩니다.
	g_pD3DDevice9->SetViewport(&backupViewPort);
}

void SetVector(D3DXVECTOR3* pV, FLOAT rX, FLOAT rY, FLOAT rZ)
{
	NULLCHK_RETURN_NOCOMENT(pV);

	// 벡터는 이렇게 값을 설정합니다.
	pV->x = rX;
	pV->y = rY;
	pV->z = rZ;
}

HRESULT CALLBACK OnLost()
{
	OnRelease();
	return S_OK;
}

HRESULT CALLBACK OnReset()
{
	OnInit();
	return S_OK;
}
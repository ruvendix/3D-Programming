#include "base_project.h"
#include "global_variable_declaration.h"
#include "main.h"
#include "RX\RXCubeDX9.h"

// ====================================================================================
// 매크로 정의부입니다.


// ====================================================================================
// 구조체 및 공용체 선언부입니다.


// ====================================================================================
// 전역 변수 선언부입니다.
namespace
{
	RX::RXMain_DX9* g_pMainDX = nullptr;
	RX::RXCubeDX9*  g_pCube   = nullptr;
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

	g_pCube = RXNew RX::RXCubeDX9;
	NULLCHK_HEAPALLOC(g_pCube);

	D3DXVECTOR3 vMin(-1.0f, -1.0f, -1.0f);
	D3DXVECTOR3 vMax( 1.0f,  1.0f,  1.0f);

	g_pCube->CreateCubeByVertex(vMin, vMax);
	//g_pCube->CreateCubeByIndex(vMin, vMax);

	// rhw를 사용하지 않는다면 변환 이전의 공간좌표를 사용하게 되므로
	// 각종 변환 과정을 거쳐야 합니다. 조명(라이팅, Lighting)도 그중 하나인데
	// 조명에 관한 연산을 따로 하지 않았으므로 조명은 꺼줘야 합니다.
	RX::RXRendererDX9::Instance()->AdjustRenderState(D3DRS_LIGHTING, false);

	// 정점 크기 설정입니다.
	// FLOAT을 DWORD로 해석해야 하므로 DWORD*로 변경한 뒤에 값을 참조해야 합니다.
    FLOAT rPointSize = 20.0f;
	RX::RXRendererDX9::Instance()->AdjustRenderState(D3DRS_POINTSIZE,
		*reinterpret_cast<DWORD*>(&rPointSize));

	return S_OK;
}

// 렌더링 함수입니다.
// 실제 렌더링 작업인 Draw Call이 처리됩니다.
// Draw Call은 프레임당 호출되는 렌더링 함수를 뜻하는데 호출되는 빈도수를
// 조사하면 Draw Call Count를 알아낼 수 있습니다.
HRESULT CALLBACK OnRender()
{
	D3DXVECTOR3 vEye(4.0f, 4.0f, -4.0f);   // 카메라의 위치
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // 카메라가 보는 지점
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // 카메라의 업 벡터

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	RX::RXRendererDX9::Instance()->ApplyViewMatrix(matView);

	// 큐브 렌더링입니다.
	g_pCube->DrawCubeByVertex();
	//g_pCube->DrawCubeByIndex();

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	SAFE_DELTE(g_pCube);
	return S_OK;
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
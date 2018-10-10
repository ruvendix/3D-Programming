#include "base_project.h"
#include "global_variable_declaration.h"
#include "main.h"
#include "RXVertexBufferDX9_test.h"

// ====================================================================================
// 매크로 정의부입니다.


// ====================================================================================
// 구조체 및 공용체 선언부입니다.


// ====================================================================================
// 전역 변수 선언부입니다.
namespace
{
	RX::RXMain_DX9*        g_pMainDX       = nullptr;
	RX::RXVertexBufferDX9* g_pVertexBuffer = nullptr;
}

extern IDirect3DDevice9* g_pD3DDevice9 = nullptr;
extern HRESULT           g_hDXResult   = S_OK;

// ====================================================================================
// 함수 선언부입니다.
HRESULT CALLBACK OnInit();
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

	g_pMainDX = RXNew RX::RXMain_DX9;
	NULLCHK(g_pMainDX);
	
	g_pMainDX->setSubFunc(OnInit,    SUBFUNC_TYPE::INIT);
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
	g_pD3DDevice9 = g_pMainDX->getD3DDevice9();
	NULLCHK(g_pD3DDevice9);

	g_pVertexBuffer = RXNew RX::RXVertexBufferDX9;
	NULLCHK_HEAPALLOC(g_pVertexBuffer);

	// FVF(Flexible Vertex Format) 형식을 설정합니다.
	// DirectX9에서는 FVF 설정이 필요합니다.
	// 아래에 있는 뜻은 변환되기 전의 공간좌표와 색상을 사용하겠다는 뜻입니다.
	g_pVertexBuffer->setFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

	// 정점 순서는 왼손좌표계이므로 시계방향입니다.
	// 순서(Winding Order)를 무시하려면 반시계방향(CCW) 컬링을 막아야 합니다.
	// 여기서 넣는 정점은 로컬좌표입니다. 이 정점들을 월드좌표로 변환하게 됩니다.
	g_pVertexBuffer->InsertVertex(0.0f, 0.2f, 0.0f, DXCOLOR_RED);
	g_pVertexBuffer->InsertVertex(0.2f, -0.2f, 0.0f, DXCOLOR_GREEN);
	g_pVertexBuffer->InsertVertex(-0.2f, -0.2f, 0.0f, DXCOLOR_BLUE);

	g_pVertexBuffer->CreateVertexBuffer();

	// rhw를 사용하지 않는다면 변환 이전의 공간좌표를 사용하게 되므로
	// 각종 변환 과정을 거쳐야 합니다. 조명(라이팅, Lighting)도 그중 하나인데
	// 조명에 관한 연산을 따로 하지 않았으므로 조명은 꺼줘야 합니다.
	g_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, false);
	//g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return S_OK;
}

// 렌더링 함수입니다.
// 실제 렌더링 작업인 Draw Call이 처리됩니다.
// Draw Call은 프레임당 호출되는 렌더링 함수를 뜻하는데 호출되는 빈도수를
// 조사하면 Draw Call Count를 알아낼 수 있습니다.
HRESULT CALLBACK OnRender()
{
	D3DXMATRIXA16 matTrans;
	D3DXMatrixTranslation(&matTrans, 0.2f, 0.0f, 0.0f);

	// 로컬좌표를 월드좌표로 변환해줍니다.
	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &matTrans);

	g_pVertexBuffer->DrawPrimitive();

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	SAFE_DELTE(g_pVertexBuffer);
	return S_OK;
}
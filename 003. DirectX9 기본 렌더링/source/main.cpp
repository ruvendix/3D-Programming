#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


// ====================================================================================
// 매크로 정의부입니다.

// FVF(Flexible Vertex Format) 형식을 설정합니다.
// DirectX9에서는 FVF 설정이 필요합니다.
// 아래에 있는 뜻은 변환된 공간좌표와 색상을 사용하겠다는 뜻입니다.
#define CUSTOM_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)


// ====================================================================================
// 구조체 및 공용체 선언부입니다.

// 정점의 형식을 설정합니다.
// x, y, z, rhw, diffuse로 설정합니다.
struct CustomVertex
{
	D3DXVECTOR3 vPos;    // v는 vector의 약자입니다. 공간좌표를 뜻합니다.
	float       rRHW;    // RHW는 Reciprocal Homogeneous W의 약자입니다. 동차좌표를 뜻합니다.
	DWORD       dwColor; // 정점의 색상을 의미합니다. Diffuse는 확산이라는 뜻입니다.
};


// ====================================================================================
// 전역 변수 선언부입니다.
IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
HRESULT                 g_DXResult      = S_OK;


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
	
	RXMAIN_DX9->setSubFunc(OnInit,    SUBFUNC_TYPE::INIT);
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
	// 정점 순서는 왼손좌표계이므로 시계방향입니다.
	CustomVertex vertices[3] =
	{
		{ { 500.0f, 100.0f, 0.0f }, 1.0f, DXCOLOR_RED   },
	    { { 800.0f, 600.0f, 0.0f }, 1.0f, DXCOLOR_GREEN },
		{ { 200.0f, 600.0f, 0.0f }, 1.0f, DXCOLOR_BLUE  },
	};

	// 정점 버퍼를 생성합니다.
	g_DXResult = D3DDEVICE9->CreateVertexBuffer(
		sizeof(CustomVertex) * 3, // 정점 버퍼의 용량입니다.
		D3DUSAGE_WRITEONLY,       // 사용법인데 0이 기본이지만 최적화를 위해 쓰기 전용으로 설정합니다.
		CUSTOM_FVF,               // FVF 형식입니다.
		D3DPOOL_MANAGED,          // 메모리풀 설정입니다.
		&g_pVertexBuffer,         // 정점 버퍼의 포인터를 넘깁니다.
		nullptr);                 // nullptr로 설정합니다.

	DXERR_HANDLER(g_DXResult);
	NULLCHK_RETURN_EFAIL(g_pVertexBuffer, "정점 버퍼 초기화 실패!");
	
	// 정점 버퍼에 실제로 정점 정보를 복사합니다.
	// 메모리에 접근하기 때문에 메모리를 잠그고 푸는 과정이 있습니다.
	void* pVertexData = nullptr;
	g_pVertexBuffer->Lock(
		0,                        // 오프셋(Offset), 즉 시작 위치를 의미하는데 전체 잠금은 0입니다.
		sizeof(CustomVertex) * 3, // 복사할 정점 정보의 용량을 넘겨줍니다.
		&pVertexData,             // 복사된 정점 정보를 다룰 수 있는 포인터를 설정해줍니다.
		D3DFLAG_NONE);            // 잠금 플래그인데 0으로 설정합니다.
	::CopyMemory(pVertexData, vertices, sizeof(CustomVertex) * 3);
	g_pVertexBuffer->Unlock();

	return S_OK;
}

// 렌더링 함수입니다.
// 실제 렌더링 작업인 Draw Call이 처리됩니다.
// Draw Call은 프레임당 호출되는 렌더링 함수를 뜻하는데 호출되는 빈도수를
// 조사하면 Draw Call Count를 알아낼 수 있습니다.
HRESULT CALLBACK OnRender()
{
	D3DDEVICE9->SetFVF(CUSTOM_FVF);
	D3DDEVICE9->SetStreamSource(
		0,                     // 스트림 넘버인데 0으로 설정합니다.
		g_pVertexBuffer,       // 정점 버퍼를 설정해줍니다.
		0,                     // 오프셋인데 0으로 설정합니다.
		sizeof(CustomVertex)); // 보폭(Stride)을 의미하는데 FVF로 생성한 크기와 일치해야 합니다.
	
	D3DDEVICE9->DrawPrimitive(
		D3DPT_TRIANGLELIST, // 렌더링 형식을 설정합니다.
		0,                  // 오프셋인데 0으로 설정합니다.
		1);                 // 렌더링할 개수를 설정합니다. 개수가 안 맞으면 정상 작동 보장하지 못합니다.

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	SAFE_RELEASE(g_pVertexBuffer);
	return S_OK;
}
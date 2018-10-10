#include "base_project.h"
#include "global_variable_definition.h"

// ====================================================================================
// 매크로 정의부입니다.

// FVF(Flexible Vertex Format) 형식을 설정합니다.
// DirectX9에서는 FVF 설정이 필요합니다.
// 아래에 있는 뜻은 변환되기 전의 공간좌표와 색상을 사용하겠다는 뜻입니다.
// D3DFVF_XYZRHW에서 D3DFVF_XYZ로 변경됩니다.
#define CUSTOM_FVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)


// ====================================================================================
// 구조체 및 공용체 선언부입니다.

// 정점의 형식을 설정합니다.
// x, y, z, diffuse로 설정합니다.
// 변환되기 전이므로 rhw는 없습니다.
struct CustomVertex
{
	D3DXVECTOR3 vPos;    // v는 vector의 약자입니다. 공간좌표를 뜻합니다.
	DWORD       dwColor; // 정점의 색상을 의미합니다. Diffuse는 분산이라는 뜻입니다.
};


// ====================================================================================
// 전역 변수 선언부입니다.
namespace
{
	RX::RXMain_DX9*         g_pMainDX       = nullptr;
	IDirect3DDevice9*       g_pD3DDevice9   = nullptr;
	IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
	HRESULT                 g_hDXResult     = S_OK;
}


// ====================================================================================
// 함수 선언부입니다.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

// 기준 축 회전입니다.
void RotationBasisAxisTest(FLOAT rDegreeX, FLOAT rDegreeY, FLOAT rDegreeZ);

// 오일러 각 회전입니다.
void RotationEulerAngleTest(FLOAT rDegreeX, FLOAT rDegreeY, FLOAT rDegreeZ);

// 임의의 축 회전입니다.
void RotationFreeAxis(const D3DXVECTOR3& vAxis, FLOAT rDegree);

// 2차원에서의 쿼터니언 회전입니다.
void RotationQuaternionAxisTest(const D3DXVECTOR3& vAxis, FLOAT rDegree);


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

	// 정점 순서는 왼손좌표계이므로 시계방향입니다.
	// 프로그램의 정중앙이 원점(0, 0, 0)이 되므로
	// 기본 카메라에 맞게 좌표를 설정해야 합니다.
	CustomVertex vertices[3] =
	{
		{ {  0.0f,  0.2f , 0.0f }, DXCOLOR_RED   },
	    { {  0.2f, -0.2f , 0.0f }, DXCOLOR_GREEN },
		{ { -0.2f, -0.2f , 0.0f }, DXCOLOR_BLUE  },
	};

	// 정점 버퍼를 생성합니다.
	g_hDXResult = g_pD3DDevice9->CreateVertexBuffer(
		sizeof(CustomVertex) * 3, // 정점 버퍼의 크기입니다.
		0,                        // 사용법인데 일반적으로는 0입니다.
		CUSTOM_FVF,               // FVF 형식입니다.
		D3DPOOL_MANAGED,          // 메모리풀 설정입니다.
		&g_pVertexBuffer,         // 정점 버퍼의 포인터를 넘깁니다.
		nullptr);                 // nullptr로 설정합니다.

	DXERR_HANDLER(g_hDXResult);
	NULLCHK_EFAIL_RETURN(g_pVertexBuffer, "정점 버퍼 초기화 실패!");
	
	// 정점 버퍼에 실제로 정점 정보를 복사합니다.
	// 메모리에 접근하기 때문에 메모리를 잠그고 푸는 과정이 있습니다.
	void* pVertexData = nullptr;
	g_pVertexBuffer->Lock(
		0,                        // 오프셋(Offset), 즉 시작 위치를 의미하는데 전체 잠금은 0입니다.
		sizeof(CustomVertex) * 3, // 복사할 정점 정보의 크기를 넘겨줍니다.
		&pVertexData,             // 복사된 정점 정보를 다룰 수 있는 포인터를 설정해줍니다.
		0);                       // 잠금 플래그인데 0으로 설정합니다.
	::CopyMemory(pVertexData, vertices, sizeof(CustomVertex) * 3);
	g_pVertexBuffer->Unlock();

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
	// 기준 축 회전을 테스트합니다.
	//RotationBasisAxisTest(10.0f, 30.0f, -10.0f);

	// 오일러 각 회전을 테스트합니다.
	//RotationEulerAngleTest(10.0f, 30.0f, -10.0f);

	// 임의의 축 회전을 테스트합니다.
	D3DXVECTOR3 vAxis(0.0f, 0.0f, 1.0f);
	//RotationFreeAxis(vAxis, -70.0f);

	// 쿼터니언 회전을 테스트합니다.
	// 쿼터니언 회전에는 임의의 축이 필요합니다.
	//RotationQuaternionAxisTest(vAxis, -70.0f);

	g_pD3DDevice9->SetFVF(CUSTOM_FVF);
	g_pD3DDevice9->SetStreamSource(
		0,                     // 스트림 넘버인데 0으로 설정합니다.
		g_pVertexBuffer,       // 정점 버퍼를 설정해줍니다.
		0,                     // 오프셋인데 0으로 설정합니다.
		sizeof(CustomVertex)); // 보폭(Stride)을 의미하는데 FVF로 생성한 크기와 일치해야 합니다.
	
	g_pD3DDevice9->DrawPrimitive(
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

// 오일러 각 회전과 맞추기 위해 Z -> X -> Y 순으로만 회전합니다.
void RotationBasisAxisTest(FLOAT rDegreeX, FLOAT rDegreeY, FLOAT rDegreeZ)
{
	D3DXMATRIXA16 matRotZ;
	D3DXMatrixIdentity(&matRotZ);
	D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(rDegreeZ));

	D3DXMATRIXA16 matRotX;
	D3DXMatrixIdentity(&matRotX);
	D3DXMatrixRotationX(&matRotX, D3DXToRadian(rDegreeX));

	D3DXMATRIXA16 matRotY;
	D3DXMatrixIdentity(&matRotY);
	D3DXMatrixRotationY(&matRotY, D3DXToRadian(rDegreeY));

	// Z * X * Y 순으로 곱합니다.
	// 오일러 각 회전의 기본형입니다.
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);
	matWorld = matRotZ * matRotX * matRotY;

	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &matWorld);
}

void RotationEulerAngleTest(FLOAT rDegreeX, FLOAT rDegreeY, FLOAT rDegreeZ)
{
	D3DXMATRIXA16 matRot;
	D3DXMatrixIdentity(&matRot);

	// Yaw   -> Y축
	// Pitch -> X축
	// Roll  -> Z축
	//
	// 오른쪽 인자부터 전달되므로 Roll -> Pitch -> Yaw 순으로 회전됩니다.
	D3DXMatrixRotationYawPitchRoll(&matRot,
		D3DXToRadian(rDegreeY), D3DXToRadian(rDegreeX), D3DXToRadian(rDegreeZ));
	
	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &matRot);
}

void RotationFreeAxis(const D3DXVECTOR3& vAxis, FLOAT rDegree)
{
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationAxis(&matRot, &vAxis, D3DXToRadian(rDegree));
	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &matRot);
}

void RotationQuaternionAxisTest(const D3DXVECTOR3& vAxis, FLOAT rDegree)
{
	// 설정한 임의의 축으로 쿼터니언을 설정합니다.
	D3DXQUATERNION quat;
	D3DXQuaternionRotationAxis(&quat, &vAxis, D3DXToRadian(rDegree));

	// 쿼터니언을 행렬로 변환시킵니다.
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationQuaternion(&matRot, &quat);

	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &matRot);
}
#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


// ====================================================================================
// 구조체 선언부입니다.
struct LoadXFileInfo
{
	ID3DXMesh*          pMesh;         // 메시 인터페이스 포인터
	D3DXMATERIAL*       pMaterial;     // 머티리얼과 텍스처 이름을 담아둘 포인터
	DWORD               materialCnt;   // 모델의 머티리얼 개수
	IDirect3DTexture9** ppMeshTexture; // 메시에 사용되는 텍스처 인터페이스 포인터의 포인터
};


// ====================================================================================
// 전역 변수 선언부입니다.
IDirect3DDevice9* g_pD3DDevice9 = nullptr;
RX::RXMain_DX9*   g_pMainDX     = nullptr;

HRESULT       g_DXResult = S_OK;
D3DXMATRIXA16 g_matWorld;

namespace
{
	// 원래는 벡터로만 사용되는데 이번에는 FLOAT 3개를 묶은 것으로 봅니다.
	D3DXVECTOR3 g_rotateAngle;

	// XFile 로딩 정보 관리 및 가독성을 높이기 위해 구조체로 묶었습니다.
	LoadXFileInfo g_loadXFileInfo;
}


// ====================================================================================
// 함수 선언부입니다.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

// 기본 행렬을 설정합니다.
void DefaultMatrix();

// 기본 렌더 스테이트를 설정합니다.
void DefaultRenderState();

// XFile을 로딩합니다.
void LoadXFile();

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

	DefaultMatrix();
	DefaultRenderState();

	// XFile을 로딩합니다.
	LoadXFile();

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
	// XFile에서 로딩한 메시를 렌더링합니다.
	for (INT32 i = 0; i < g_loadXFileInfo.materialCnt; ++i)
	{
		// 각 메시마다 머티리얼을 설정해줍니다.
		g_pD3DDevice9->SetMaterial(&g_loadXFileInfo.pMaterial[i].MatD3D);

		// 각 메시마다 텍스처를 설정해줍니다.
		if (g_loadXFileInfo.ppMeshTexture[i] != nullptr)
		{
			g_pD3DDevice9->SetTexture(0, g_loadXFileInfo.ppMeshTexture[i]);
		}

		// 텍스처가 존재하지 않아도 메시는 렌더링 가능합니다.
		g_loadXFileInfo.pMesh->DrawSubset(i);
	}

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	for (INT32 i = 0; i < g_loadXFileInfo.materialCnt; ++i)
	{
		SAFE_RELEASE(g_loadXFileInfo.ppMeshTexture[i]);
	}
	
	SAFE_DELTE_ARR(g_loadXFileInfo.ppMeshTexture);
	SAFE_DELTE(g_loadXFileInfo.pMaterial);

	SAFE_RELEASE(g_loadXFileInfo.pMesh);

	return S_OK;
}

void DefaultMatrix()
{
	// =====================================================================
	// 뷰행렬을 설정합니다.
	D3DXVECTOR3 vEye(0.0f, 0.0f, -10.0f);   // 카메라의 위치
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // 카메라가 보는 지점
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // 카메라의 업 벡터

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	g_pD3DDevice9->SetTransform(D3DTS_VIEW, &matView);
	
	// =====================================================================
	// 투영행렬을 설정합니다.
	D3DXMATRIXA16 matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection, (D3DX_PI / 4.0f),
		(static_cast<FLOAT>(g_pMainDX->getClientWidth()) / (g_pMainDX->getClientHeight())),
		1.0f, 1000.0f);
	g_pD3DDevice9->SetTransform(D3DTS_PROJECTION, &matProjection);
}

void DefaultRenderState()
{
	// rhw를 사용하지 않는다면 변환 이전의 공간좌표를
	// 사용하게 되므로 각종 변환 과정을 거쳐야 합니다.
	// 조명(라이팅, Lighting)도 그중 하나인데
	// 이번에는 조명을 사용하지 않으므로 조명을 꺼줍니다.
	g_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, FALSE);

	// 필 모드를 설정합니다. 디폴트는 솔리드입니다.
	g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	//g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);

	// 컬링 모드를 설정합니다. 디폴트는 반시계방향 컬링입니다.
	// 큐브를 확인하기 위해서는 컬링 모드를 무시해야 합니다.
	g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	//g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}

void LoadXFile()
{
	// XFile을 로딩할 때 사용되는 리소스 버퍼입니다.
	// 셰이더에도 사용되는데 지금 당장은 XFile에서만 사용됩니다.
	ID3DXBuffer* pMaterialBuffer = nullptr;

	// XFile을 로딩합니다.
	g_DXResult = D3DXLoadMeshFromX(
		L"Resource/XFile/SimpleSword.x", // XFile 경로
		D3DXMESH_MANAGED, // XFile에서 로딩한 메시의 정점 버퍼 및 인덱스 버퍼를 저장할 메모리풀
		g_pD3DDevice9,    // 가상 디바이스 객체(레퍼런스 카운트 증가)
		nullptr, // 지금 당장은 인접 정보를 사용하지 않음
		&pMaterialBuffer, // XFile의 머티리얼 정보를 저장
		nullptr, // 지금 당장은 사용하지 않음
		&g_loadXFileInfo.materialCnt, // XFile의 머티리얼 개수를 저장
		&g_loadXFileInfo.pMesh); // XFile의 메시를 저장
	DXERR_HANDLER(g_DXResult);

	// XFile에서 로딩한 머티리얼 정보를 캐스팅해서 사용합니다.
	// 이렇게 포인터를 다른 포인터로 해석할 때는 reinterpret_cast<>를 사용해야 가독성이 좋습니다.
	D3DXMATERIAL* pMaterial = static_cast<D3DXMATERIAL*>(pMaterialBuffer->GetBufferPointer());

	// create a new material buffer for each material in the mesh
	INT32 materialCnt = g_loadXFileInfo.materialCnt;
	g_loadXFileInfo.pMaterial     = RXNew D3DXMATERIAL[materialCnt];
	g_loadXFileInfo.ppMeshTexture = RXNew IDirect3DTexture9*[materialCnt];
	
	for (INT32 i = 0; i < materialCnt; ++i)
	{
		// XFile에서 로딩한 머티리얼 정보를 저장합니다.
		g_loadXFileInfo.pMaterial[i] = pMaterial[i];
		g_loadXFileInfo.pMaterial[i].MatD3D.Ambient = g_loadXFileInfo.pMaterial[i].MatD3D.Diffuse;
		
		// XFile에서 로딩한 텍스처 정보를 저장해야 하는데
		// 현재 XFile은 텍스처 경로가 좀 이상해서 이렇게 수동으로 넣습니다.	
		// 텍스처 로딩에 실패하면 nullptr로 설정합니다.
		if (FAILED(D3DXCreateTextureFromFile(g_pD3DDevice9,
			L"Resource/Texture/SimpleSword_Diffuse.dds",
			&g_loadXFileInfo.ppMeshTexture[i])))
		{
			g_loadXFileInfo.ppMeshTexture[i] = nullptr;
		}
	}
}

void OnUserInput()
{
	if (::GetAsyncKeyState('Z') & 0x8000)
	{
		g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	if (::GetAsyncKeyState('X') & 0x8000)
	{
		g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}

	if (::GetAsyncKeyState('C') & 0x8000)
	{
		g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
	}

	if (::GetAsyncKeyState('A') & 0x8000)
	{
		g_rotateAngle.z += 4.0f;
	}

	if (::GetAsyncKeyState('D') & 0x8000)
	{
		g_rotateAngle.z -= 4.0f;
	}

	if (::GetAsyncKeyState('W') & 0x8000)
	{
		g_rotateAngle.x += 4.0f;
	}

	if (::GetAsyncKeyState('S') & 0x8000)
	{
		g_rotateAngle.x -= 4.0f;
	}

	if (::GetAsyncKeyState('Q') & 0x8000)
	{
		g_rotateAngle.y += 4.0f;
	}

	if (::GetAsyncKeyState('E') & 0x8000)
	{
		g_rotateAngle.y -= 4.0f;
	}

	if (::GetAsyncKeyState('R') & 0x8000)
	{
		RX::ZeroVector(&g_rotateAngle);
		g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	D3DXMATRIXA16 matScale;
	FLOAT rFactor = 1.0f / 36.0f;
	D3DXMatrixScaling(&matScale, rFactor, rFactor, rFactor);

	D3DXMATRIXA16 matTrans;
	D3DXMatrixTranslation(&matTrans, 0.0f, -3.5f, 0.0f);

	// 각도 보정
	g_rotateAngle.z = RX::AdjustAngle(g_rotateAngle.z);
	g_rotateAngle.x = RX::AdjustAngle(g_rotateAngle.x);

	// 회전행렬입니다. 순서는 Z -> X -> Y입니다.
	// 즉, Roll -> Pitch -> Yaw입니다.
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot,
		D3DXToRadian(g_rotateAngle.y),
		D3DXToRadian(g_rotateAngle.x),
		D3DXToRadian(g_rotateAngle.z));

	g_matWorld = matScale * matRot * matTrans;
	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &g_matWorld);
}
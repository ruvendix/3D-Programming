#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


// ====================================================================================
// 구조체 선언부입니다.
struct LoadXFileInfo
{
	ID3DXMesh*          pMesh;         // 메시 인터페이스 포인터
	D3DXMATERIAL*       pMaterial;     // 재질과 텍스처 이름을 담아둘 포인터
	DWORD               materialCnt;   // 모델의 재질 개수
	IDirect3DTexture9** ppMeshTexture; // 메시에 사용되는 텍스처 인터페이스 포인터의 포인터
};


// ====================================================================================
// 전역 변수 선언부입니다.
HRESULT       g_DXResult = S_OK;

namespace
{
	D3DXVECTOR3   g_rotateAngle;
	D3DXMATRIXA16 g_matWorld;

	// XFile 로딩 정보 관리 및 가독성을 높이기 위해 구조체로 묶었습니다.
	LoadXFileInfo g_loadXFileInfo;
}


// ====================================================================================
// 함수 선언부입니다.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

void DefaultMatrix();
void DefaultRenderState();
void OnUserInput();

// XFile을 로딩합니다.
void LoadXFile();


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
	DefaultMatrix();
	DefaultRenderState();

	// XFile을 로딩합니다.
	LoadXFile();

	RX::ShowMouseCursor(true);

	return S_OK;
}

HRESULT CALLBACK OnUpdate()
{
	OnUserInput();
	return S_OK;
}

HRESULT CALLBACK OnRender()
{
	// XFile에서 로딩한 메시를 렌더링합니다.
	for (UINT32 i = 0; i < g_loadXFileInfo.materialCnt; ++i)
	{
		// 각 메시마다 재질을 설정해줍니다.
		D3DDEVICE9->SetMaterial(&g_loadXFileInfo.pMaterial[i].MatD3D);

		// 각 메시마다 텍스처를 설정해줍니다.
		if (g_loadXFileInfo.ppMeshTexture[i] != nullptr)
		{
			D3DDEVICE9->SetTexture(0, g_loadXFileInfo.ppMeshTexture[i]);
		}

		// 텍스처가 존재하지 않아도 메시는 렌더링 가능합니다.
		g_loadXFileInfo.pMesh->DrawSubset(i);
	}

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	for (UINT32 i = 0; i < g_loadXFileInfo.materialCnt; ++i)
	{
		SAFE_RELEASE(g_loadXFileInfo.ppMeshTexture[i]);
	}
	
	SAFE_DELTE_ARR(g_loadXFileInfo.ppMeshTexture);
	SAFE_DELTE_ARR(g_loadXFileInfo.pMaterial);

	SAFE_RELEASE(g_loadXFileInfo.pMesh);

	return S_OK;
}

void DefaultMatrix()
{
	// ==========================================================
	// 뷰행렬을 설정합니다.
	D3DXVECTOR3 vEye(0.0f, 0.0f, -10.0f);  // 카메라의 위치
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // 카메라가 보는 지점
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

void DefaultRenderState()
{
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, FALSE);
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
		D3DDEVICE9, // 가상 디바이스 객체(레퍼런스 카운트 증가)
		nullptr, // 지금 당장은 인접 정보를 사용하지 않음
		&pMaterialBuffer, // XFile의 재질 정보를 저장
		nullptr, // 지금 당장은 사용하지 않음
		&g_loadXFileInfo.materialCnt, // XFile의 재질 개수를 저장
		&g_loadXFileInfo.pMesh); // XFile의 메시를 저장
	DXERR_HANDLER(g_DXResult);

	// XFile에서 로딩한 재질 정보를 캐스팅해서 사용합니다.
	// 이렇게 포인터를 다른 포인터로 해석할 때는 reinterpret_cast<>를 사용해야 가독성이 좋습니다.
	D3DXMATERIAL* pMaterial = reinterpret_cast<D3DXMATERIAL*>(pMaterialBuffer->GetBufferPointer());

	// 부가 정보를 저장하기 위한 공간을 생성합니다.
	INT32 materialCnt = g_loadXFileInfo.materialCnt;
	g_loadXFileInfo.pMaterial     = RXNew D3DXMATERIAL[materialCnt];
	g_loadXFileInfo.ppMeshTexture = RXNew IDirect3DTexture9*[materialCnt];
	
	for (INT32 i = 0; i < materialCnt; ++i)
	{
		// XFile에서 로딩한 재질 정보를 저장합니다.
		g_loadXFileInfo.pMaterial[i] = pMaterial[i];
		g_loadXFileInfo.pMaterial[i].MatD3D.Ambient = g_loadXFileInfo.pMaterial[i].MatD3D.Diffuse;
		
		// XFile에서 로딩한 텍스처 정보를 저장해야 하는데
		// 현재 XFile은 텍스처 경로가 좀 이상해서 이렇게 수동으로 넣습니다.	
		// 텍스처 로딩에 실패하면 nullptr로 설정합니다.
		if (FAILED(D3DXCreateTextureFromFile(D3DDEVICE9,
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

	FLOAT rDeltaSeconds = RX::RXProgramFPSMgr::Instance()->getTimer()->getDeltaSeconds();
	if (::GetAsyncKeyState('A') & 0x8000)
	{
		g_rotateAngle.z += 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('D') & 0x8000)
	{
		g_rotateAngle.z -= 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('W') & 0x8000)
	{
		g_rotateAngle.x += 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('S') & 0x8000)
	{
		g_rotateAngle.x -= 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('Q') & 0x8000)
	{
		g_rotateAngle.y += 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('E') & 0x8000)
	{
		g_rotateAngle.y -= 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('R') & 0x8000)
	{
		RX::ZeroVector(&g_rotateAngle);
		D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	D3DXMATRIXA16 matScale;
	FLOAT rFactor = 1.0f / 36.0f;
	D3DXMatrixScaling(&matScale, rFactor, rFactor, rFactor);

	D3DXMATRIXA16 matTrans;
	D3DXMatrixTranslation(&matTrans, 0.0f, -3.5f, 0.0f);

	g_rotateAngle.z = RX::AdjustAngle(g_rotateAngle.z);
	g_rotateAngle.x = RX::AdjustAngle(g_rotateAngle.x);

	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot,
		D3DXToRadian(g_rotateAngle.y),
		D3DXToRadian(g_rotateAngle.x),
		D3DXToRadian(g_rotateAngle.z));

	g_matWorld = matScale * matRot * matTrans;
	D3DDEVICE9->SetTransform(D3DTS_WORLD, &g_matWorld);
}
#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"
#include "RX\RX3DAxisDX9.h"


// ====================================================================================
// 공용체 선언부입니다.
enum class SHAPE_TYPE : INT32
{
	NONE = 0,
	BOX,
	SPHERE,
	TEAPOT,
	TORUS,
	CYLINDER,
	END,
};


// ====================================================================================
// 전역 변수 선언부입니다.
HRESULT g_DXResult = S_OK;

namespace
{
	RX::RX3DAxisDX9 g_axis; // 3D축을 다루기 위한 것
	ID3DXMesh* g_pMesh = nullptr; // 메시 인터페이스
	INT32 g_shapeType = 0;

	// 원래는 벡터로만 사용되는데 이번에는 FLOAT 3개를 묶은 것으로 봅니다.
	D3DXVECTOR3 g_rotateAngle;
}


// ====================================================================================
// 함수 선언부입니다.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

void DefaultViewAndProjection();
void DefaultLight();
void DefaultRenderState();
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
	DefaultViewAndProjection();
	DefaultLight();
	DefaultRenderState();

	g_axis.CreateAxis(20.0f);

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
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_axis.DrawAxis();
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, TRUE);

	// 메시를 렌더링할 때는 DrawSubset()을 사용합니다.
	// Subset은 메시에서 같은 재질끼리 묶은 것이라고 보면 됩니다.
	// 즉, 메시에 재질이 3개라면 Subset은 3개가 됩니다.
	if (g_pMesh)
	{
		g_pMesh->DrawSubset(0);
	}

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	g_axis.Release();
	SAFE_RELEASE(g_pMesh);
	return S_OK;
}

void DefaultViewAndProjection()
{
	// ==========================================================
	// 뷰행렬을 설정합니다.
	D3DXVECTOR3 vEye(3.0f, 3.0f, 3.0f);    // 카메라의 위치
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

void DefaultLight()
{
	// 조명을 생성하고 등록합니다.
	// 이번에는 방향 광원을 이용합니다.
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	
	D3DXVECTOR3 vDir = { -1.0f, -1.0f, 0.2f };
	D3DXVec3Normalize(&vDir, &vDir);
	light.Direction = vDir;

	light.Ambient  = D3DXCOLOR(DXCOLOR_WHITE);
	light.Diffuse  = D3DXCOLOR(DXCOLOR_WHITE);
	light.Specular = D3DXCOLOR(DXCOLOR_WHITE);

	// 광원을 등록하고 활성화시킵니다.
	D3DDEVICE9->SetLight(0, &light);
	D3DDEVICE9->LightEnable(0, TRUE);

	// 재질은 하나만 등록 가능합니다.
	// 따라서 재질을 자주 바꿔가며 렌더링할 때가 많습니다.
	D3DXMATERIAL mtrl;
	::ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.MatD3D.Ambient  = D3DXCOLOR(DXCOLOR_BLACK);
	mtrl.MatD3D.Diffuse  = D3DXCOLOR(DXCOLOR_BLUE);
	
	// 반사광 설정입니다.
	// 반사광에는 Power가 있는데 이게 높으면 반사 범위가 줄어들고
	// 이게 낮으면 반사 범위가 넓어집니다.
	// 반사광을 사용하려면 렌더 스테이트에서 반사광을 활성해야 합니다.
	mtrl.MatD3D.Specular = D3DXCOLOR(DXCOLOR_WHITE);
	mtrl.MatD3D.Power    = 20.0f;

	// 방사광 설정입니다.
	// 방사광은 낮게 설정해야 합니다.
	mtrl.MatD3D.Emissive = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);

	D3DDEVICE9->SetMaterial(&(mtrl.MatD3D));
}

void DefaultRenderState()
{
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, TRUE);
	D3DDEVICE9->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	D3DDEVICE9->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
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

	if (::GetAsyncKeyState('F') & 0x0001)
	{
		g_axis.FlipEnable();
	}

	if (::GetAsyncKeyState('V') & 0x0001)
	{
		++g_shapeType;
		SHAPE_TYPE shapeType = static_cast<SHAPE_TYPE>(g_shapeType);
		if (shapeType >= SHAPE_TYPE::END)
		{
			shapeType = SHAPE_TYPE::NONE;
			g_shapeType = static_cast<INT32>(shapeType);
		}

		switch (shapeType)
		{
		case SHAPE_TYPE::BOX:
		{
			SAFE_RELEASE(g_pMesh);
			D3DXCreateBox(
				D3DDEVICE9, // 가상 디바이스
				1.4f, // 가로 길이
				1.4f, // 세로 길이
				1.4f, // 깊이
				&g_pMesh, // 메시 인터페이스 포인터
				nullptr); // 안 쓰는 값
			break;
		}
		case SHAPE_TYPE::SPHERE:
		{
			SAFE_RELEASE(g_pMesh);
			D3DXCreateSphere(
				D3DDEVICE9, // 가상 디바이스
				1.0f, // 반지름
				80, // Z축 슬라이스
				80, // X축 스택
				&g_pMesh, // 메시 인터페이스 포인터
				nullptr); // 안 쓰는 값
			break;
		}
		case SHAPE_TYPE::TEAPOT:
		{
			SAFE_RELEASE(g_pMesh);
			D3DXCreateTeapot(
				D3DDEVICE9, // 가상 디바이스
				&g_pMesh, // 메시 인터페이스 포인터
				nullptr); // 안 쓰는 값
			break;
		}
		case SHAPE_TYPE::TORUS:
		{
			SAFE_RELEASE(g_pMesh);
			D3DXCreateTorus(
				D3DDEVICE9, // 가상 디바이스
				0.4f, // 내부원 반지름
				0.8f, // 외부원 반지름
				80, // Z축 슬라이스
				80, // X축 스택
				&g_pMesh, // 메시 인터페이스 포인터
				nullptr); // 안 쓰는값
			break;
		}
		case SHAPE_TYPE::CYLINDER:
		{
			SAFE_RELEASE(g_pMesh);
			D3DXCreateCylinder(
				D3DDEVICE9, // 가상 디바이스
				0.4f, // 첫번째 반지름
				0.6f, // 두번째 반지름
				1.4f, // 길이
				40, // Z축 슬라이스
				40, // X축 스택
				&g_pMesh, // 메시 인터페이스 포인터
				nullptr); // 안 쓰는 값
			break;
		}
		case SHAPE_TYPE::NONE:
		{
			SAFE_RELEASE(g_pMesh);
			break;
		}
		}
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

	// 각도 보정
	g_rotateAngle.z = RX::AdjustAngle(g_rotateAngle.z);
	g_rotateAngle.x = RX::AdjustAngle(g_rotateAngle.x);
	g_rotateAngle.y = RX::AdjustAngle(g_rotateAngle.y);

	// 회전행렬입니다. 순서는 Z -> X -> Y입니다.
	// 즉, Roll -> Pitch -> Yaw입니다.
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot,
		D3DXToRadian(g_rotateAngle.y),
		D3DXToRadian(g_rotateAngle.x),
		D3DXToRadian(g_rotateAngle.z));

	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matRot);
}
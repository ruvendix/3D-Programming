#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


// ====================================================================================
// 전역 변수 선언부입니다.
namespace
{
	D3DXVECTOR3   g_vLocalPos;
	D3DXVECTOR3   g_vWorldPos;
	D3DXVECTOR3   g_vViewPos;

	D3DXMATRIXA16 g_matWorld;
	D3DXMATRIXA16 g_matView;
}

// ====================================================================================
// 함수 선언부입니다.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnRender();

void LocalToWorld();
void WorldToLocal();
void LocalToView();
void ViewToLocal();


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

	RXMAIN_DX9->setSubFunc(OnInit,   SUBFUNC_TYPE::INIT);
	RXMAIN_DX9->setSubFunc(OnRender, SUBFUNC_TYPE::RENDER);

	RXMAIN_DX9->RunMainRoutine(hInstance, IDI_RUVENDIX_ICO);

	return RXMAIN_DX9->getMessageCode();
}

HRESULT CALLBACK OnInit()
{
	LocalToWorld();
	//WorldToLocal();
	LocalToView();
	//ViewToLocal();
	return S_OK;
}

HRESULT CALLBACK OnRender()
{
	TCHAR szText[DEFAULT_STRING_LENGTH];
	swprintf_s(szText, L"\n로컬좌표(%.2f, %.2f, %.2f)\n월드좌표(%.2f, %.2f, %.2f)",
		g_vLocalPos.x, g_vLocalPos.y, g_vLocalPos.z, g_vWorldPos.x, g_vWorldPos.y, g_vWorldPos.z);
	RX::RXRendererDX9::Instance()->DrawTextOriginClientArea(szText, DT_LEFT, DXCOLOR_WHITE);
	
	swprintf_s(szText, L"\n\n\n뷰좌표(%.2f, %.2f, %.2f)", g_vViewPos.x, g_vViewPos.y, g_vViewPos.z);
	RX::RXRendererDX9::Instance()->DrawTextOriginClientArea(szText, DT_LEFT, DXCOLOR_WHITE);
	return S_OK;
}

// 로컬좌표 -> 월드좌표
void LocalToWorld()
{
	// 로컬좌표
	RX::SetVector(&g_vLocalPos, 1.0f, 14.0f, -4.0f);

	// 각 변환행렬 구하기
	D3DXMATRIXA16 matScale;
	D3DXMatrixScaling(&matScale, 2.0f, 3.0f, 2.0f);
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationY(&matRot, D3DXToRadian(60.0f));
	D3DXMATRIXA16 matMove;
	D3DXMatrixTranslation(&matMove, 102.0f, 325.0f, 221.0f);

	// 월드행렬 결합
	g_matWorld = matScale * matRot * matMove;

	// D3DXVec3TransformNormal()은 이동행렬이 없을 경우
	// D3DXVec3TransformCoord()는 이동행렬이 있을 경우

	// 로컬좌표 * 월드행렬
	D3DXVec3TransformCoord(&g_vWorldPos, &g_vLocalPos, &g_matWorld);
}

// 월드좌표 -> 로컬좌표
void WorldToLocal()
{
	// 월드행렬의 역행렬
	D3DXMatrixInverse(&g_matWorld, nullptr, &g_matWorld);

	// 로컬좌표 구하기
	D3DXVec3TransformCoord(&g_vWorldPos, &g_vWorldPos, &g_matWorld);
}

// 로컬좌표 -> 월드좌표 -> 뷰좌표
void LocalToView()
{
	// 뷰행렬 구하기
	D3DXVECTOR3 vEye(12.0f, 15.0f, 13.0f);
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vWorldUp(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&g_matView, &vEye, &vLookAt, &vWorldUp);

	// 월드행렬 * 뷰행렬
	g_matView = g_matWorld * g_matView;

	// 로컬좌표 * 월드행렬 * 뷰행렬
	D3DXVec3TransformCoord(&g_vViewPos, &g_vLocalPos, &g_matView);
}

// 뷰좌표 -> 월드좌표 -> 로컬좌표
void ViewToLocal()
{
	// 뷰행렬까지의 역행렬 구하기
	D3DXMatrixInverse(&g_matView, nullptr, &g_matView);

	// 로컬좌표 구하기
	D3DXVec3TransformCoord(&g_vViewPos, &g_vViewPos, &g_matView);
}

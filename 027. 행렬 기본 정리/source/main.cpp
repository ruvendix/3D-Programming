#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


// ====================================================================================
// ���� ���� ������Դϴ�.
namespace
{
	D3DXVECTOR3   g_vLocalPos;
	D3DXVECTOR3   g_vWorldPos;
	D3DXVECTOR3   g_vViewPos;

	D3DXMATRIXA16 g_matWorld;
	D3DXMATRIXA16 g_matView;
}

// ====================================================================================
// �Լ� ������Դϴ�.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnRender();

void LocalToWorld();
void WorldToLocal();
void LocalToView();
void ViewToLocal();


// ====================================================================================
// <Win32 API�� WinMain()�� �������Դϴ�>
INT32 APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    szCmdLine,
	INT32     cmdShow)
{
	// ������� �ʴ� �Ű������� ǥ�����ݴϴ�.
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
	swprintf_s(szText, L"\n������ǥ(%.2f, %.2f, %.2f)\n������ǥ(%.2f, %.2f, %.2f)",
		g_vLocalPos.x, g_vLocalPos.y, g_vLocalPos.z, g_vWorldPos.x, g_vWorldPos.y, g_vWorldPos.z);
	RX::RXRendererDX9::Instance()->DrawTextOriginClientArea(szText, DT_LEFT, DXCOLOR_WHITE);
	
	swprintf_s(szText, L"\n\n\n����ǥ(%.2f, %.2f, %.2f)", g_vViewPos.x, g_vViewPos.y, g_vViewPos.z);
	RX::RXRendererDX9::Instance()->DrawTextOriginClientArea(szText, DT_LEFT, DXCOLOR_WHITE);
	return S_OK;
}

// ������ǥ -> ������ǥ
void LocalToWorld()
{
	// ������ǥ
	RX::SetVector(&g_vLocalPos, 1.0f, 14.0f, -4.0f);

	// �� ��ȯ��� ���ϱ�
	D3DXMATRIXA16 matScale;
	D3DXMatrixScaling(&matScale, 2.0f, 3.0f, 2.0f);
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationY(&matRot, D3DXToRadian(60.0f));
	D3DXMATRIXA16 matMove;
	D3DXMatrixTranslation(&matMove, 102.0f, 325.0f, 221.0f);

	// ������� ����
	g_matWorld = matScale * matRot * matMove;

	// D3DXVec3TransformNormal()�� �̵������ ���� ���
	// D3DXVec3TransformCoord()�� �̵������ ���� ���

	// ������ǥ * �������
	D3DXVec3TransformCoord(&g_vWorldPos, &g_vLocalPos, &g_matWorld);
}

// ������ǥ -> ������ǥ
void WorldToLocal()
{
	// ��������� �����
	D3DXMatrixInverse(&g_matWorld, nullptr, &g_matWorld);

	// ������ǥ ���ϱ�
	D3DXVec3TransformCoord(&g_vWorldPos, &g_vWorldPos, &g_matWorld);
}

// ������ǥ -> ������ǥ -> ����ǥ
void LocalToView()
{
	// ����� ���ϱ�
	D3DXVECTOR3 vEye(12.0f, 15.0f, 13.0f);
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vWorldUp(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&g_matView, &vEye, &vLookAt, &vWorldUp);

	// ������� * �����
	g_matView = g_matWorld * g_matView;

	// ������ǥ * ������� * �����
	D3DXVec3TransformCoord(&g_vViewPos, &g_vLocalPos, &g_matView);
}

// ����ǥ -> ������ǥ -> ������ǥ
void ViewToLocal()
{
	// ����ı����� ����� ���ϱ�
	D3DXMatrixInverse(&g_matView, nullptr, &g_matView);

	// ������ǥ ���ϱ�
	D3DXVec3TransformCoord(&g_vViewPos, &g_vViewPos, &g_matView);
}

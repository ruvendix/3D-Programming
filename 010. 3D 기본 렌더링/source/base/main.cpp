#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"
#include "RX/RX3DAxisDX9.h"


// ====================================================================================
// ���� ���� ������Դϴ�.
HRESULT g_DXResult = S_OK;

namespace
{
	RX::RX3DAxisDX9* g_p3DAxis = nullptr;
}


// ====================================================================================
// �Լ� ������Դϴ�.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

void ViewMatrixTest();
void ProjectionMatrixTest();
void ViewPortTest();


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
	
	RXMAIN_DX9->setSubFunc(OnInit,    SUBFUNC_TYPE::INIT);
	RXMAIN_DX9->setSubFunc(OnRender,  SUBFUNC_TYPE::RENDER);
	RXMAIN_DX9->setSubFunc(OnRelease, SUBFUNC_TYPE::RELEASE);

	RXMAIN_DX9->RunMainRoutine(hInstance, IDI_RUVENDIX_ICO);
	return RXMAIN_DX9->getMessageCode();
}

HRESULT CALLBACK OnInit()
{
	g_p3DAxis = RXNew RX::RX3DAxisDX9;
	NULLCHK_HEAPALLOC(g_p3DAxis);
	g_p3DAxis->CreateAxis(0.5f);

	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, false);

	return S_OK;
}

HRESULT CALLBACK OnRender()
{
	ViewMatrixTest();
	//ProjectionMatrixTest();
	//ViewPortTest();
	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	SAFE_DELTE(g_p3DAxis);
	return S_OK;
}

// �亯ȯ�Դϴ�.
// �亯ȯ�� ���庯ȯ �Ŀ� ó���˴ϴ�.
void ViewMatrixTest()
{
	// �亯ȯ�Դϴ�.
	// �亯ȯ�� ���庯ȯ �Ŀ� ó���˴ϴ�.
	// ī�޶� ���忡 ���� ī�޶� �������� ���� �˴ϴ�.
	// ī�޶� �������� ���� ���� ��ȯ�� ����� ��ȯ�Դϴ�.
	D3DXVECTOR3 vEye(0.3f, 0.3f, -0.3f);   // ī�޶��� ��ġ
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // ī�޶� ���� ����
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // ī�޶��� �� ����

	// ====================================================================================
	// ī�޶��� �ü� ���ʹ� vLookAt�� �ƴ϶� vLookAt - vEye�Դϴ�.
	// vEye�� vLookAt�� ��ġ�����Դϴ�. vLookAt�� �ü� ���ͷ� ���� �� �˴ϴ�.
	// vUp�� �� �����ε� ī�޶� ���� ���� �� ����մϴ�.
	// ī�޶� ���� ���� �� ������ ����ϴµ� vUp�� �̶� ���˴ϴ�.
	// ���� vUp�� ���� ���Ͷ�� �����ϸ� �˴ϴ�.
	// ====================================================================================

	// ������� ����ϴ�.
	// ������� ������ ���� D3DTS_VIEW�� ���� �˴ϴ�.
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp); // �޼���ǥ�� �����Դϴ�.
	D3DDEVICE9->SetTransform(D3DTS_VIEW, &matView);

	g_p3DAxis->DrawAxis();
}

// ������ȯ�Դϴ�.
// ������ȯ�� �亯ȯ �Ŀ� ó���˴ϴ�.
// ������ȯ�� ũ�� ��������, �����, ���������� �ֽ��ϴ�.
// ������ ���� �������� �������ּ���.
void ProjectionMatrixTest()
{
	D3DXVECTOR3 vEye(3.0f, 3.0f, -3.0f);
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	D3DDEVICE9->SetTransform(D3DTS_VIEW, &matView);
	
	// ��Ⱦ���Դϴ�. ���� / ���θ� �ǹ��մϴ�.
	// �Ϲ� �ػ����� ���̵� �ػ������� ������ �� ����ϴµ�
	// ������ȯ���� ��Ⱦ�� ���˴ϴ�. �Ϲ������δ� 1.0���� ����մϴ�.
	FLOAT rAspectRatio =
		static_cast<FLOAT>(RXMAIN_DX9->getClientRect()->CalcWidth() /
			               RXMAIN_DX9->getClientRect()->CalcHeight());

	// ��������� ����ϴ�.
	// ��������� ������ ���� D3DTS_PROJECTION�� ���� �˴ϴ�.
	//
	// ������Ŀ��� FOV(Field Of View)�� Near, Far�� �Է��ؾ� �ϴµ�
	// FOV�� �þ߰��̶�� �ؼ� ���� ������ ������ �ǹ��մϴ�.
	// �ʹ� ������ ���̴� �� ���� ����, �ʹ� ũ�� ���̴� �� ����������...
	// ����͸� ���ؼ� �������� ������� ���� ������ �Ϲ������δ� 45���� �����մϴ�.
	// ��, 180���� 4���� 1�� �����մϴ�. �׸��� FOV�� Y�� �������� �����մϴ�.
	// Near, Far�� ���̱� �����ϴ� �κа� ������ �ʴ� �κ��� ������ �ǹ��մϴ�.
	// Near ~ Far�� �̷�� ������ ����ü(Frustum)��� �ϴµ� ���������
	// ����ü�� �������� ��������� �˴ϴ�. ���������� �� ������ ������ ����...
	// �Ϲ������δ� Near�� 1.0���� �����ϰ� Far�� ũ�� ����ϴ�.
	D3DXMATRIXA16 matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection, D3DX_PI / 4.0f, rAspectRatio, 1.0f, 1000.0f);
	D3DDEVICE9->SetTransform(D3DTS_PROJECTION, &matProjection);

	g_p3DAxis->DrawAxis();
}

void ViewPortTest()
{
#pragma region LeftTopViewPort
	
	D3DXVECTOR3 vEye(2.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	D3DDEVICE9->SetTransform(D3DTS_VIEW, &matView);
	
	FLOAT rAspectRatio =
		static_cast<FLOAT>(RXMAIN_DX9->getClientRect()->CalcWidth() /
			               RXMAIN_DX9->getClientRect()->CalcHeight());

	D3DXMATRIXA16 matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection, D3DX_PI / 4.0f, rAspectRatio, 1.0f, 1000.0f);
	D3DDEVICE9->SetTransform(D3DTS_PROJECTION, &matProjection);

	// ����Ʈ �����Դϴ�.
	// ����Ʈ�� ����ۿ� �ٸ� �������μ�, ����ۿ� ������ �������� �����̶�� ���� �˴ϴ�.
	// ���� ����Ʈ�� �� �̿��ϸ� �ϳ��� ������Ʈ�� ���� ȭ������ �������ϴ� �� �����մϴ�.
	D3DVIEWPORT9 viewPort;
	D3DVIEWPORT9 backupViewPort;
	D3DDEVICE9->GetViewport(&viewPort);
	backupViewPort = viewPort; // ���� ����Ʈ ���

	// ����Ʈ�� ������ ������ �����մϴ�.
	// ȭ�� 4���� ������ ���̹Ƿ� ���� ����Ʈ�� ���� ���� �˴ϴ�.
	// ���ο� ���� ���̴� Ŭ���̾�Ʈ ������ �����Դϴ�. ��, 4���� 1 ������ �˴ϴ�.
	viewPort.Width  = RXMAIN_DX9->getClientRect()->CalcWidth() / 2;
	viewPort.Height = RXMAIN_DX9->getClientRect()->CalcHeight() / 2;

	// ����Ʈ ������ �����ϸ� �ݵ�� ���� ����̽��� ��������� �մϴ�.
	D3DDEVICE9->SetViewport(&viewPort);

	// ��������� �������� �������� ó���˴ϴ�.
	g_p3DAxis->DrawAxis();

#pragma endregion

	// ====================================================================================
	// ���ĺ��ʹ� ���� ������ �ݺ��ϸ� �Ǵµ� ������ �������ߴ� ���� ������� �մϴ�.
	// �׷��� ������ ������ �������� �Ϳ� ������� �������ϰ� �˴ϴ�.
	// ȭ�� ������ �̿��ϴ� ���α׷��� �����Ƿ� �� �˾Ƶθ� �����ϴ�.
	// ====================================================================================

#pragma region RightTopViewPort

	// ī�޶� ������ �����մϴ�.
	// ī�޶��� �ü����ʹ� ī�޶��� Z���� �˴ϴ�.
	// ī�޶��� X���� ���� �� �޼���ǥ�� �������� �� ���Ϳ� �ü� ���͸� �����մϴ�.
	// �� ���� ī�޶��� Z��� X���� �����ؼ� Y���� ���ϰ� �˴ϴ�.
	// ž���� ���� �����غ��� �� ���Ͱ� (0, 1, 0)�� �� �ü����Ϳ��� ������ 0�� �˴ϴ�.
	// �� ���Ͱ� �̷�� ���� 0�� �Ǵ� 180����� ������ 0�Դϴ�. (sin �׷��� ����)
	// ���� ž�並 �����Ϸ��� �� ���͸� �����ؾ� �մϴ�.
	RX::SetVector(&vEye, 0.0f, 2.0f, 0.0f);
	RX::SetVector(&vUp, 0.0f, 0.0f, -1.0f);
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	D3DDEVICE9->SetTransform(D3DTS_VIEW, &matView);

	// ����Ʈ ������ ���� �������ݴϴ�.
	viewPort.X = RXMAIN_DX9->getClientRect()->CalcWidth() / 2;
	D3DDEVICE9->SetViewport(&viewPort);

	// 3D���� �������մϴ�.
	// ���ο� ��ü�� �ƴ� ���� ��ü�� �״�� ����մϴ�.
	g_p3DAxis->DrawAxis();

#pragma endregion
#pragma region LeftBottomViewPort
	
	// ���� ������ �ݺ��մϴ�.
	// ī�޶� ������ �����մϴ�.
	RX::SetVector(&vEye, 0.0f, 0.0f, -2.0f);
	RX::SetVector(&vUp, 0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	D3DDEVICE9->SetTransform(D3DTS_VIEW, &matView);

	// ����Ʈ ������ ���� �������ݴϴ�.
	viewPort.X = 0;
	viewPort.Y = RXMAIN_DX9->getClientRect()->CalcHeight() / 2;
	D3DDEVICE9->SetViewport(&viewPort);

	// 3D���� �������մϴ�.
	// ���ο� ��ü�� �ƴ� ���� ��ü�� �״�� ����մϴ�.
	g_p3DAxis->DrawAxis();

#pragma endregion
#pragma region RightBottomViewPort

	// ���� ������ �ݺ��մϴ�.
	// ī�޶� ������ �����մϴ�.
	RX::SetVector(&vEye, 1.0f, 1.0f, -1.0f);
	RX::SetVector(&vUp, 0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	D3DDEVICE9->SetTransform(D3DTS_VIEW, &matView);

	// ����Ʈ ������ ���� �������ݴϴ�.
	viewPort.X = RXMAIN_DX9->getClientRect()->CalcWidth() / 2;
	viewPort.Y = RXMAIN_DX9->getClientRect()->CalcHeight() / 2;
	D3DDEVICE9->SetViewport(&viewPort);

	// 3D���� �������մϴ�.
	// ���ο� ��ü�� �ƴ� ���� ��ü�� �״�� ����մϴ�.
	g_p3DAxis->DrawAxis();

#pragma endregion

	// ����� ����Ʈ�� �������ݴϴ�.
	// ���������� ������ ����Ʈ �������� ���̰� �˴ϴ�.
	D3DDEVICE9->SetViewport(&backupViewPort);
}
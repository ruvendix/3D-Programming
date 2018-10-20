#include "base_project.h"
#include "global_variable_declaration.h"
#include "main.h"
#include "RX/RX3DAxisDX9.h"

// ====================================================================================
// ��ũ�� ���Ǻ��Դϴ�.


// ====================================================================================
// ����ü �� ����ü ������Դϴ�.


// ====================================================================================
// ���� ���� ������Դϴ�.
namespace
{
	RX::RXMain_DX9*  g_pMainDX = nullptr;
	RX::RX3DAxisDX9* g_p3DAxis = nullptr;
}

extern IDirect3DDevice9* g_pD3DDevice9 = nullptr;
extern HRESULT           g_DXResult    = S_OK;


// ====================================================================================
// �Լ� ������Դϴ�.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();
HRESULT CALLBACK OnLost();
HRESULT CALLBACK OnReset();

void ViewMatrixTest();
void ProjectionMatrixTest();
void ViewPortTest();

void SetVector(D3DXVECTOR3* pV, FLOAT rX, FLOAT rY, FLOAT rZ);


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

// �ʱ�ȭ �Լ��Դϴ�.
// 3D �������� ������ ���� ���Ƿ� ������ �۾��� �ʱ�ȭ����� �մϴ�.
// �������ϸ鼭 �ǽð����� ���굵 ���������� �׷��� �Ǹ� �������� �������� �˴ϴ�.
// �Ϲ������� �������� ���� ������ �۾��� ó���մϴ�.
HRESULT CALLBACK OnInit()
{
	g_pD3DDevice9 = RX::RXRendererDX9::Instance()->getD3DDevice9();
	NULLCHK(g_pD3DDevice9);

	g_p3DAxis = RXNew RX::RX3DAxisDX9;
	NULLCHK_HEAPALLOC(g_p3DAxis);
	g_p3DAxis->CreateAxis(0.5f);

	// rhw�� ������� �ʴ´ٸ� ��ȯ ������ ������ǥ�� ����ϰ� �ǹǷ�
	// ���� ��ȯ ������ ���ľ� �մϴ�. ����(������, Lighting)�� ���� �ϳ��ε�
	// ���� ���� ������ ���� ���� �ʾ����Ƿ� ������ ����� �մϴ�.
	g_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, false);

	return S_OK;
}

// ������ �Լ��Դϴ�.
// ���� ������ �۾��� Draw Call�� ó���˴ϴ�.
// Draw Call�� �����Ӵ� ȣ��Ǵ� ������ �Լ��� ���ϴµ� ȣ��Ǵ� �󵵼���
// �����ϸ� Draw Call Count�� �˾Ƴ� �� �ֽ��ϴ�.
HRESULT CALLBACK OnRender()
{
	//ViewMatrixTest();
	//ProjectionMatrixTest();
	ViewPortTest();
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
	g_pD3DDevice9->SetTransform(D3DTS_VIEW, &matView);

	g_p3DAxis->DrawAxis();
}

// ������ȯ�Դϴ�.
// ������ȯ�� �亯ȯ �Ŀ� ó���˴ϴ�.
// ������ȯ�� ũ�� ��������, �����, ���������� �ֽ��ϴ�.
// ������ ���� �������� �������ּ���.
void ProjectionMatrixTest()
{
	// �亯ȯ�Դϴ�.
	// �亯ȯ�� ���庯ȯ �Ŀ� ó���˴ϴ�.
	// ī�޶� ���忡 ���� ī�޶� �������� ���� �˴ϴ�.
	// ī�޶� �������� ���� ���� ��ȯ�� ����� ��ȯ�Դϴ�.
	D3DXVECTOR3 vEye(3.0f, 3.0f, -3.0f);   // ī�޶��� ��ġ
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // ī�޶� ���� ����
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // ī�޶��� �� ����

	// ������� ����ϴ�.
	// ������� ������ ���� D3DTS_VIEW�� ���� �˴ϴ�.
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	g_pD3DDevice9->SetTransform(D3DTS_VIEW, &matView);
	
	// ��Ⱦ���Դϴ�. ���� / ���θ� �ǹ��մϴ�.
	// �Ϲ� �ػ����� ���̵� �ػ������� ������ �� ����ϴµ�
	// ������ȯ���� ��Ⱦ�� ���˴ϴ�. �Ϲ������δ� 1.0���� ����մϴ�.
	FLOAT rAspectRatio =
		static_cast<FLOAT>(g_pMainDX->getClientWidth() / g_pMainDX->getClientHeight());

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
	g_pD3DDevice9->SetTransform(D3DTS_PROJECTION, &matProjection);

	g_p3DAxis->DrawAxis();
}

void ViewPortTest()
{
#pragma region LeftTopViewPort
	
	// �亯ȯ�Դϴ�.
	// �亯ȯ�� ���庯ȯ �Ŀ� ó���˴ϴ�.
	// ī�޶� ���忡 ���� ī�޶� �������� ���� �˴ϴ�.
	// ī�޶� �������� ���� ���� ��ȯ�� �亯ȯ�Դϴ�.
	D3DXVECTOR3 vEye(2.0f, 0.0f, 0.0f);    // ī�޶��� ��ġ
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // ī�޶� ���� ����
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // ī�޶��� �� ����

	// ������� ����ϴ�.
	// ������� ������ ���� D3DTS_VIEW�� ���� �˴ϴ�.
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	g_pD3DDevice9->SetTransform(D3DTS_VIEW, &matView);
	
	// ��Ⱦ���Դϴ�. ���� / ���θ� �ǹ��մϴ�.
	// �Ϲ� �ػ����� ���̵� �ػ������� ������ �� ����ϴµ�
	// ������ȯ���� ��Ⱦ�� ���˴ϴ�. �Ϲ������δ� 1.0���� ����մϴ�.
	FLOAT rAspectRatio =
		static_cast<FLOAT>(g_pMainDX->getClientWidth() / g_pMainDX->getClientHeight());

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
	g_pD3DDevice9->SetTransform(D3DTS_PROJECTION, &matProjection);

	// ����Ʈ �����Դϴ�.
	// ����Ʈ�� ����ۿ� �ٸ� �������μ�, ����ۿ� ������ �������� �����̶�� ���� �˴ϴ�.
	// ���� ����Ʈ�� �� �̿��ϸ� �ϳ��� ������Ʈ�� ���� ȭ������ �������ϴ� �� �����մϴ�.
	D3DVIEWPORT9 viewPort;
	D3DVIEWPORT9 backupViewPort;
	g_pD3DDevice9->GetViewport(&viewPort);
	backupViewPort = viewPort; // ���� ����Ʈ ���

	// ����Ʈ�� ������ ������ �����մϴ�.
	// ȭ�� 4���� ������ ���̹Ƿ� ���� ����Ʈ�� ���� ���� �˴ϴ�.
	// ���ο� ���� ���̴� Ŭ���̾�Ʈ ������ �����Դϴ�. ��, 4���� 1 ������ �˴ϴ�.
	viewPort.Width  = g_pMainDX->getClientWidth() / 2;
	viewPort.Height = g_pMainDX->getClientHeight() / 2;

	// ����Ʈ ������ �����ϸ� �ݵ�� ���� ����̽��� ��������� �մϴ�.
	g_pD3DDevice9->SetViewport(&viewPort);

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
	SetVector(&vEye, 0.0f, 2.0f, 0.0f);
	SetVector(&vUp, 0.0f, 0.0f, -1.0f);
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	g_pD3DDevice9->SetTransform(D3DTS_VIEW, &matView);

	// ����Ʈ ������ ���� �������ݴϴ�.
	viewPort.X = g_pMainDX->getClientWidth() / 2;
	g_pD3DDevice9->SetViewport(&viewPort);

	// 3D���� �������մϴ�.
	// ���ο� ��ü�� �ƴ� ���� ��ü�� �״�� ����մϴ�.
	g_p3DAxis->DrawAxis();

#pragma endregion
#pragma region LeftBottomViewPort
	
	// ���� ������ �ݺ��մϴ�.
	// ī�޶� ������ �����մϴ�.
	SetVector(&vEye, 0.0f, 0.0f, -2.0f);
	SetVector(&vUp, 0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	g_pD3DDevice9->SetTransform(D3DTS_VIEW, &matView);

	// ����Ʈ ������ ���� �������ݴϴ�.
	viewPort.X = 0;
	viewPort.Y = g_pMainDX->getClientHeight() / 2;
	g_pD3DDevice9->SetViewport(&viewPort);

	// 3D���� �������մϴ�.
	// ���ο� ��ü�� �ƴ� ���� ��ü�� �״�� ����մϴ�.
	g_p3DAxis->DrawAxis();

#pragma endregion
#pragma region RightBottomViewPort

	// ���� ������ �ݺ��մϴ�.
	// ī�޶� ������ �����մϴ�.
	SetVector(&vEye, 1.0f, 1.0f, -1.0f);
	SetVector(&vUp, 0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	g_pD3DDevice9->SetTransform(D3DTS_VIEW, &matView);

	// ����Ʈ ������ ���� �������ݴϴ�.
	viewPort.X = g_pMainDX->getClientWidth() / 2;;
	viewPort.Y = g_pMainDX->getClientHeight() / 2;
	g_pD3DDevice9->SetViewport(&viewPort);

	// 3D���� �������մϴ�.
	// ���ο� ��ü�� �ƴ� ���� ��ü�� �״�� ����մϴ�.
	g_p3DAxis->DrawAxis();

#pragma endregion

	// ����� ����Ʈ�� �������ݴϴ�.
	// ���������� ������ ����Ʈ �������� ���̰� �˴ϴ�.
	g_pD3DDevice9->SetViewport(&backupViewPort);
}

void SetVector(D3DXVECTOR3* pV, FLOAT rX, FLOAT rY, FLOAT rZ)
{
	NULLCHK_RETURN_NOCOMENT(pV);

	// ���ʹ� �̷��� ���� �����մϴ�.
	pV->x = rX;
	pV->y = rY;
	pV->z = rZ;
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
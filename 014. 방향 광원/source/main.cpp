#include "base_project.h"
#include "old_main.h"
#include "global_variable_declaration.h"

// ====================================================================================
// ���� ���� ������Դϴ�.
IDirect3DDevice9*       g_pD3DDevice9   = nullptr;
RX::RXMain_DX9*         g_pMainDX       = nullptr;
IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
IDirect3DIndexBuffer9*  g_pIndexBuffer  = nullptr;
ID3DXLine*              g_pLine         = nullptr; // ���� �׸��� ���� ��

D3DXMATRIXA16 g_matViewAndProjection; // �̸� ����ص� ����� * �������
D3DXMATRIXA16 g_matProjection;        // �̸� ����ص� �������
D3DXMATRIXA16 g_matAll; // ������İ� �����ϱ� ���� ��ü��ȯ���

DWORD   g_dwNormalVectorRenderingFlag = 0;
HRESULT g_DXResult = S_OK;

std::vector<VertexP3D> g_vecP3D;
std::vector<VertexP3N> g_vecP3N;
std::vector<Index16>   g_vecIndex16;
D3DXVECTOR3            g_vBaseVertex[8];
D3DXVECTOR3            g_vTriangleNormal[12]; // �ﰢ�������� ��������

// ������ ���� �ݻ� �� ����� �����ϴ� �����Դϴ�.
// ����(0.0f)���� ���(1.0f)���� ������ ������ ������ �� �ֽ��ϴ�.
D3DXMATERIAL g_mtrl;

// ���� ����ü�Դϴ�.
// �̹����� ���� �������� ����մϴ�.
D3DLIGHT9 g_directionalLight;


// ====================================================================================
// �Լ� ������Դϴ�.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();


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
	g_pMainDX->setSubFunc(OnUpdate,  SUBFUNC_TYPE::UPDATE);
	g_pMainDX->setSubFunc(OnRender,  SUBFUNC_TYPE::RENDER);
	g_pMainDX->setSubFunc(OnRelease, SUBFUNC_TYPE::RELEASE);

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

	CreateCube(-1.0f, 1.0f);

	// ������� �����մϴ�.
	D3DXVECTOR3 vEye(0.0f, 0.0f, -8.0f);   // ī�޶��� ��ġ
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // ī�޶� ���� ����
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // ī�޶��� �� ����

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	g_pD3DDevice9->SetTransform(D3DTS_VIEW, &matView);

	// ��������� �����մϴ�.
	D3DXMatrixPerspectiveFovLH(&g_matProjection, (D3DX_PI / 4.0f),
		(static_cast<FLOAT>(g_pMainDX->getClientWidth()) / (g_pMainDX->getClientHeight())),
		1.0f, 1000.0f);
	g_pD3DDevice9->SetTransform(D3DTS_PROJECTION, &g_matProjection);

	// ������� �ʱ�ȭ�Դϴ�.
	D3DXMatrixIdentity(&g_matAll);
	g_matViewAndProjection = matView * g_matProjection;

	// ���� �׸��� ��ü�� �����մϴ�.
	D3DXCreateLine(g_pD3DDevice9, &g_pLine);

	// ���콺 Ŀ���� �����ݴϴ�.
	RX::ShowMouseCursor(true);

	// rhw�� ������� �ʴ´ٸ� ��ȯ ������ ������ǥ��
	// ����ϰ� �ǹǷ� ���� ��ȯ ������ ���ľ� �մϴ�.
	// ����(������, Lighting)�� ���� �ϳ��ε�
	// �̹����� ������ ����ϹǷ� ������ ���ݴϴ�.
	g_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, true);

	// �ø� ��带 �����մϴ�. ����Ʈ�� �ݽð���� �ø��Դϴ�.
	// ť�긦 Ȯ���ϱ� ���ؼ��� �ø� ��带 �����ؾ� �մϴ�.
	//g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// �������͸� �ڵ����� ������ִ� �����Դϴ�.
	// ��! �� ������ �̿��ϰ� �Ǹ� ����� �� ž�ϴ�...
	// ����Ʈ�� FALSE�Դϴ�.
	g_pD3DDevice9->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// ������ �ʱ�ȭ�մϴ�.
	g_directionalLight.Type = D3DLIGHT_DIRECTIONAL;
	g_directionalLight.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	D3DXVECTOR3 vDir = { 1.0f, 0.0f, 0.0f };
	D3DXVec3Normalize(&vDir, &vDir);
	g_directionalLight.Direction = vDir;

	// ������ ���� �� ��ġ �����մϴ�.
	// �ϳ��� ��ġ�ϹǷ� 0�� �ε����� ������ Ȱ��ȭ��ŵ�ϴ�.
	g_pD3DDevice9->LightEnable(99, TRUE);

	// ������ ���� ����̽��� ������ݴϴ�.
	g_pD3DDevice9->SetLight(100, &g_directionalLight);

	// ������ �������� �ʱ�ȭ�մϴ�.
	::ZeroMemory(&g_mtrl, sizeof(g_mtrl));
	g_mtrl.MatD3D.Diffuse.r = 1.0f;

	return S_OK;
}

// ������Ʈ �Լ��Դϴ�.
// �������� ������ �ְų� ���α׷��� ������ �ִ�
// �������� �������Ӹ��� ������Ʈ�մϴ�.
HRESULT CALLBACK OnUpdate()
{
	InputOldKeyboard();
	return S_OK;
}

// ������ �Լ��Դϴ�.
// ���� ������ �۾��� Draw Call�� ó���˴ϴ�.
// Draw Call�� �����Ӵ� ȣ��Ǵ� ������ �Լ��� ���ϴµ� ȣ��Ǵ� �󵵼���
// �����ϸ� Draw Call Count�� �˾Ƴ� �� �ֽ��ϴ�.
HRESULT CALLBACK OnRender()
{
	// ������ ����մϴ�.
	// ������ �ϳ��� ��� �����մϴ�.
	// ���� ������ ���� �ٲٸ鼭 �������ؾ� �մϴ�.
	g_pD3DDevice9->SetMaterial(&g_mtrl.MatD3D);

	g_pD3DDevice9->SetFVF(VertexP3N::format); // ���� ���� ����
	g_pD3DDevice9->SetStreamSource(
		0,                  // ����� ��Ʈ�� �ε���
		g_pVertexBuffer,    // ������ ���� ����
		0,                  // ���� ������ ������
		sizeof(VertexP3N)); // ���� �뷮

	g_pD3DDevice9->SetIndices(g_pIndexBuffer); // �ε��� ���� ����
	g_pD3DDevice9->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, // �ε��� ���۴� Ʈ���̾ޱ۸���Ʈ�� ����
		0,   // ù �ε����� �� ���� ������ ���� �ε���
		0,   // ����� ù �ε���(�ε����� 0, 1, 2, 3�� ���� �� 3�̸� 3���� ����)
		8,   // ����� ���� ����(2��° ���� + 3��° ���ڸ�ŭ �� ��)
		0,   // �ε��� ������ ������
		12); // �������� ����(ť��� �ﰢ�� 12��)

	RenderNormalVector();

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	SAFE_RELEASE(g_pIndexBuffer);
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pLine);
	return S_OK;
}
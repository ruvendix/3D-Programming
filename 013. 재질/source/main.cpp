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
std::vector<Index16>   g_vecIndex16;
D3DXVECTOR3            g_vBaseVertex[8];
D3DXVECTOR3            g_vCubeTriangleNormal[12]; // �ﰢ�������� ��������

// ������ ���� �ݻ� �� ����� �����ϴ� �����Դϴ�.
// ����(0.0f)���� ���(1.0f)���� ������ ������ ������ �� �ֽ��ϴ�.
D3DXMATERIAL g_mtrl;


// ====================================================================================
// �Լ� ������Դϴ�.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

void AdjustColorRange(D3DCOLORVALUE* pColor);

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
	D3DXVECTOR3 vEye(4.0f, 4.0f, -4.0f);   // ī�޶��� ��ġ
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

	// �ﰢ�������� �������͸� ���մϴ�.
	CalcTriangleNormal();

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

	// ������ Ȯ���ϱ� ���� ���� �ֺ����� �����մϴ�.
	g_pD3DDevice9->SetRenderState(D3DRS_AMBIENT, DXCOLOR_WHITE);

	// ������ �ʱ�ȭ�մϴ�.
	::ZeroMemory(&g_mtrl, sizeof(g_mtrl));

	return S_OK;
}

// ������Ʈ �Լ��Դϴ�.
// �������� ������ �ְų� ���α׷��� ������ �ִ�
// �������� �������Ӹ��� ������Ʈ�մϴ�.
HRESULT CALLBACK OnUpdate()
{
	InputOldKeyboard();

	// ���� ===============================
	if (::GetAsyncKeyState('T') & 0x8000)
	{
		g_mtrl.MatD3D.Ambient.r += 0.05f;
	}

	if (::GetAsyncKeyState('Y') & 0x8000)
	{
		g_mtrl.MatD3D.Ambient.r -= 0.05f;
	}
	// ====================================

	// �ʷ� ===============================
	if (::GetAsyncKeyState('U') & 0x8000)
	{
		g_mtrl.MatD3D.Ambient.g += 0.05f;
	}

	if (::GetAsyncKeyState('I') & 0x8000)
	{
		g_mtrl.MatD3D.Ambient.g -= 0.05f;
	}
	// ====================================

	// �Ķ� ===============================
	if (::GetAsyncKeyState('O') & 0x8000)
	{
		g_mtrl.MatD3D.Ambient.b += 0.05f;
	}

	if (::GetAsyncKeyState('P') & 0x8000)
	{
		g_mtrl.MatD3D.Ambient.b -= 0.05f;
	}
	// ====================================

	AdjustColorRange(&g_mtrl.MatD3D.Ambient);

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

	g_pD3DDevice9->SetFVF(VertexP3D::format); // ���� ���� ����
	g_pD3DDevice9->SetStreamSource(
		0,                  // ����� ��Ʈ�� �ε���
		g_pVertexBuffer,    // ������ ���� ����
		0,                  // ���� ������ ������
		sizeof(VertexP3D)); // ���� �뷮

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

void AdjustColorRange(D3DCOLORVALUE* pColor)
{
	if (pColor == nullptr)
	{
		return;
	}

	if (pColor->a < 0.0f)
	{
		pColor->a = 0.0f;
	}
	else if (pColor->a > 1.0f)
	{
		pColor->a = 1.0f;
	}

	if (pColor->r < 0.0f)
	{
		pColor->r = 0.0f;
	}
	else if (pColor->r > 1.0f)
	{
		pColor->r = 1.0f;
	}

	if (pColor->g < 0.0f)
	{
		pColor->g = 0.0f;
	}
	else if (pColor->g > 1.0f)
	{
		pColor->g = 1.0f;
	}

	if (pColor->b < 0.0f)
	{
		pColor->b = 0.0f;
	}
	else if (pColor->b > 1.0f)
	{
		pColor->b = 1.0f;
	}
}

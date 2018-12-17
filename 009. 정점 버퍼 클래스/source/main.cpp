#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


// ====================================================================================
// ���� ���� ������Դϴ�.
HRESULT g_DXResult = S_OK;

namespace
{
	RX::RXVertexBufferDX9* g_pVB1    = nullptr;
	RX::RXVertexBufferDX9* g_pVB2    = nullptr;
}


// ====================================================================================
// �Լ� ������Դϴ�.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();
HRESULT CALLBACK OnLost();
HRESULT CALLBACK OnReset();


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
	RXMAIN_DX9->setSubFunc(OnLost,    SUBFUNC_TYPE::LOSTDEVICE);
	RXMAIN_DX9->setSubFunc(OnReset,   SUBFUNC_TYPE::RESETDEVICE);

	RXMAIN_DX9->RunMainRoutine(hInstance, IDI_RUVENDIX_ICO);
	return RXMAIN_DX9->getMessageCode();
}

// �ʱ�ȭ �Լ��Դϴ�.
// 3D �������� ������ ���� ���Ƿ� ������ �۾��� �ʱ�ȭ����� �մϴ�.
// �������ϸ鼭 �ǽð����� ���굵 ���������� �׷��� �Ǹ� �������� �������� �˴ϴ�.
// �Ϲ������� �������� ���� ������ �۾��� ó���մϴ�.
HRESULT CALLBACK OnInit()
{
	g_pVB1 = RXNew RX::RXVertexBufferDX9;
	NULLCHK_HEAPALLOC(g_pVB1);

	// FVF(Flexible Vertex Format) ������ �����մϴ�.
	// DirectX9������ FVF ������ �ʿ��մϴ�.
	// �Ʒ��� �ִ� ���� ��ȯ�Ǳ� ���� ������ǥ�� ������ ����ϰڴٴ� ���Դϴ�.
	//g_pVB1->setFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

	// ���� ������ �޼���ǥ���̹Ƿ� �ð�����Դϴ�.
	// ����(Winding Order)�� �����Ϸ��� �ݽð����(CCW) �ø��� ���ƾ� �մϴ�.
	// ���⼭ �ִ� ������ ������ǥ�Դϴ�. �� �������� ������ǥ�� ��ȯ�ϰ� �˴ϴ�.
	g_pVB1->InsertVertex(0.0f, 0.2f, 0.0f, DXCOLOR_RED);
	g_pVB1->InsertVertex(0.2f, -0.2f, 0.0f, DXCOLOR_GREEN);
	g_pVB1->InsertVertex(-0.2f, -0.2f, 0.0f, DXCOLOR_BLUE);

	g_pVB1->CreateVertexBuffer();

	// ==================================================================
	// ���⼭���ʹ� �� ��° ���� ���� ���� �����Դϴ�.
	g_pVB2 = RXNew RX::RXVertexBufferDX9;
	NULLCHK_HEAPALLOC(g_pVB2);

	//g_pVB2->setFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

	g_pVB2->InsertVertex(-0.4f, -0.2f, 0.0f, DXCOLOR_BLUE);
	g_pVB2->InsertVertex(-0.2f, -0.6f, 0.0f, DXCOLOR_RED);
	g_pVB2->InsertVertex(-0.6f, -0.6f, 0.0f, DXCOLOR_GREEN);
	g_pVB2->InsertVertex(-0.8f, -0.2f, 0.0f, DXCOLOR_RED);
	g_pVB2->InsertVertex(-0.4f, -0.2f, 0.0f, DXCOLOR_GREEN);
	g_pVB2->InsertVertex(-0.6f, -0.6f, 0.0f, DXCOLOR_BLUE);

	g_pVB2->CreateVertexBuffer();

	// ������� �����մϴ�.
	D3DXVECTOR3 vEye(0.0f, 0.0f, -2.0f);   // ī�޶��� ��ġ
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // ī�޶� ���� ����
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // ī�޶��� �� ����

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	D3DDEVICE9->SetTransform(D3DTS_VIEW, &matView);

	// ��������� �����մϴ�.
	D3DXMATRIXA16 matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection, (D3DX_PI / 4.0f),
		(static_cast<FLOAT>(RXMAIN_DX9->getClientRect()->CalcWidth()) /
		                   (RXMAIN_DX9->getClientRect()->CalcHeight())), 1.0f, 1000.0f);
	D3DDEVICE9->SetTransform(D3DTS_PROJECTION, &matProjection);

	// rhw�� ������� �ʴ´ٸ� ��ȯ ������ ������ǥ�� ����ϰ� �ǹǷ�
	// ���� ��ȯ ������ ���ľ� �մϴ�. ����(������, Lighting)�� ���� �ϳ��ε�
	// ���� ���� ������ ���� ���� �ʾ����Ƿ� ������ ����� �մϴ�.
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, false);
	//D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return S_OK;
}

// ������ �Լ��Դϴ�.
// ���� ������ �۾��� Draw Call�� ó���˴ϴ�.
// Draw Call�� �����Ӵ� ȣ��Ǵ� ������ �Լ��� ���ϴµ� ȣ��Ǵ� �󵵼���
// �����ϸ� Draw Call Count�� �˾Ƴ� �� �ֽ��ϴ�.
HRESULT CALLBACK OnRender()
{
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationZ(&matRot, D3DXToRadian(-20.0f));

	// ������ǥ�� ������ǥ�� ��ȯ���ݴϴ�.
	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matRot);

	RX::RXRendererDX9::Instance()->DrawPrimitive(D3DPT_TRIANGLELIST, *g_pVB1);
	RX::RXRendererDX9::Instance()->DrawPrimitive(D3DPT_TRIANGLELIST, *g_pVB2);
	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	SAFE_DELTE(g_pVB2);
	SAFE_DELTE(g_pVB1);
	return S_OK;
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
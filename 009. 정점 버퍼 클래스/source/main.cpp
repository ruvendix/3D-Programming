#include "base_project.h"
#include "global_variable_declaration.h"
#include "main.h"
#include "RX/RXVertexBufferDX9.h"

// ====================================================================================
// ��ũ�� ���Ǻ��Դϴ�.


// ====================================================================================
// ����ü �� ����ü ������Դϴ�.


// ====================================================================================
// ���� ���� ������Դϴ�.
namespace
{
	RX::RXMain_DX9*        g_pMainDX = nullptr;
	RX::RXVertexBufferDX9* g_pVB1    = nullptr;
	RX::RXVertexBufferDX9* g_pVB2    = nullptr;
}

extern IDirect3DDevice9* g_pD3DDevice9 = nullptr;
extern HRESULT           g_DXResult   = S_OK;


// ====================================================================================
// �Լ� ������Դϴ�.
HRESULT CALLBACK OnInit();
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
	g_pD3DDevice9 = g_pMainDX->getD3DDevice9();
	NULLCHK(g_pD3DDevice9);

	g_pVB1 = RXNew RX::RXVertexBufferDX9;
	NULLCHK_HEAPALLOC(g_pVB1);

	// FVF(Flexible Vertex Format) ������ �����մϴ�.
	// DirectX9������ FVF ������ �ʿ��մϴ�.
	// �Ʒ��� �ִ� ���� ��ȯ�Ǳ� ���� ������ǥ�� ������ ����ϰڴٴ� ���Դϴ�.
	g_pVB1->setFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

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

	g_pVB2->setFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

	g_pVB2->InsertVertex(-0.4f, -0.2f, 0.0f, DXCOLOR_BLUE);
	g_pVB2->InsertVertex(-0.2f, -0.6f, 0.0f, DXCOLOR_RED);
	g_pVB2->InsertVertex(-0.6f, -0.6f, 0.0f, DXCOLOR_GREEN);
	g_pVB2->InsertVertex(-0.8f, -0.2f, 0.0f, DXCOLOR_RED);
	g_pVB2->InsertVertex(-0.4f, -0.2f, 0.0f, DXCOLOR_GREEN);
	g_pVB2->InsertVertex(-0.6f, -0.6f, 0.0f, DXCOLOR_BLUE);

	g_pVB2->CreateVertexBuffer();

	// rhw�� ������� �ʴ´ٸ� ��ȯ ������ ������ǥ�� ����ϰ� �ǹǷ�
	// ���� ��ȯ ������ ���ľ� �մϴ�. ����(������, Lighting)�� ���� �ϳ��ε�
	// ���� ���� ������ ���� ���� �ʾ����Ƿ� ������ ����� �մϴ�.
	g_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, false);
	//g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

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
	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &matRot);

	g_pVB1->DrawPrimitive(D3DPT_TRIANGLELIST);
	g_pVB2->DrawPrimitive(D3DPT_TRIANGLELIST);

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	SAFE_DELTE(g_pVB2);
	SAFE_DELTE(g_pVB1);
	return S_OK;
}
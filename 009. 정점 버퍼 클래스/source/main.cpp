#include "base_project.h"
#include "global_variable_declaration.h"
#include "main.h"

// ====================================================================================
// ��ũ�� ���Ǻ��Դϴ�.


// ====================================================================================
// ����ü �� ����ü ������Դϴ�.


// ====================================================================================
// ���� ���� ������Դϴ�.
namespace
{
	RX::RXMain_DX9* g_pMainDX = nullptr;
}

extern IDirect3DDevice9* g_pD3DDevice9 = nullptr;
extern HRESULT           g_hDXResult   = S_OK;

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

	// FVF(Flexible Vertex Format) ������ �����մϴ�.
	// DirectX9������ FVF ������ �ʿ��մϴ�.
	// �Ʒ��� �ִ� ���� ��ȯ�Ǳ� ���� ������ǥ�� ������ ����ϰڴٴ� ���Դϴ�.
	vb.setFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

	// ���� ������ �޼���ǥ���̹Ƿ� �ð�����Դϴ�.
	// ���α׷��� ���߾��� ����(0, 0, 0)�� �ǹǷ�
	// �⺻ ī�޶� �°� ��ǥ�� �����ؾ� �մϴ�.
	// ����(Winding Order)�� �����Ϸ��� �ݽð����(CCW) �ø��� ���ƾ� �մϴ�.
	vb.InsertVertex(0.0f, 0.2f, 0.0f, DXCOLOR_RED);
	vb.InsertVertex(0.2f, -0.2f, 0.0f, DXCOLOR_GREEN);
	vb.InsertVertex(-0.2f, -0.2f, 0.0f, DXCOLOR_BLUE);

	vb.CreateVertexBuffer();

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
	// ���� �� ȸ���� �׽�Ʈ�մϴ�.
	//RotationBasisAxisTest(10.0f, 30.0f, -10.0f);

	// ���Ϸ� �� ȸ���� �׽�Ʈ�մϴ�.
	//RotationEulerAngleTest(10.0f, 30.0f, -10.0f);

	// ������ �� ȸ���� �׽�Ʈ�մϴ�.
	//D3DXVECTOR3 vAxis(0.0f, 0.0f, 1.0f);
	//RotationFreeAxis(vAxis, -70.0f);

	// ���ʹϾ� ȸ���� �׽�Ʈ�մϴ�.
	// ���ʹϾ� ȸ������ ������ ���� �ʿ��մϴ�.
	//RotationQuaternionAxisTest(vAxis, -70.0f);

	vb.DrawPrimitive();

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	return S_OK;
}
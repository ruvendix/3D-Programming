#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


// ====================================================================================
// ��ũ�� ���Ǻ��Դϴ�.

// FVF(Flexible Vertex Format) ������ �����մϴ�.
// DirectX9������ FVF ������ �ʿ��մϴ�.
// �Ʒ��� �ִ� ���� ��ȯ�� ������ǥ�� ������ ����ϰڴٴ� ���Դϴ�.
#define CUSTOM_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)


// ====================================================================================
// ����ü �� ����ü ������Դϴ�.

// ������ ������ �����մϴ�.
// x, y, z, rhw, diffuse�� �����մϴ�.
struct CustomVertex
{
	D3DXVECTOR3 vPos;    // v�� vector�� �����Դϴ�. ������ǥ�� ���մϴ�.
	float       rRHW;    // RHW�� Reciprocal Homogeneous W�� �����Դϴ�. ������ǥ�� ���մϴ�.
	DWORD       dwColor; // ������ ������ �ǹ��մϴ�. Diffuse�� Ȯ���̶�� ���Դϴ�.
};


// ====================================================================================
// ���� ���� ������Դϴ�.
IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
HRESULT                 g_DXResult      = S_OK;


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
	
	RXMAIN_DX9->setSubFunc(OnInit,    SUBFUNC_TYPE::INIT);
	RXMAIN_DX9->setSubFunc(OnRender,  SUBFUNC_TYPE::RENDER);
	RXMAIN_DX9->setSubFunc(OnRelease, SUBFUNC_TYPE::RELEASE);

	RXMAIN_DX9->RunMainRoutine(hInstance, IDI_RUVENDIX_ICO);
	return RXMAIN_DX9->getMessageCode();
}

// �ʱ�ȭ �Լ��Դϴ�.
// 3D �������� ������ ���� ���Ƿ� ������ �۾��� �ʱ�ȭ����� �մϴ�.
// �������ϸ鼭 �ǽð����� ���굵 ���������� �׷��� �Ǹ� �������� �������� �˴ϴ�.
// �Ϲ������� �������� ���� ������ �۾��� ó���մϴ�.
HRESULT CALLBACK OnInit()
{
	// ���� ������ �޼���ǥ���̹Ƿ� �ð�����Դϴ�.
	CustomVertex vertices[3] =
	{
		{ { 500.0f, 100.0f, 0.0f }, 1.0f, DXCOLOR_RED   },
	    { { 800.0f, 600.0f, 0.0f }, 1.0f, DXCOLOR_GREEN },
		{ { 200.0f, 600.0f, 0.0f }, 1.0f, DXCOLOR_BLUE  },
	};

	// ���� ���۸� �����մϴ�.
	g_DXResult = D3DDEVICE9->CreateVertexBuffer(
		sizeof(CustomVertex) * 3, // ���� ������ �뷮�Դϴ�.
		D3DUSAGE_WRITEONLY,       // �����ε� 0�� �⺻������ ����ȭ�� ���� ���� �������� �����մϴ�.
		CUSTOM_FVF,               // FVF �����Դϴ�.
		D3DPOOL_MANAGED,          // �޸�Ǯ �����Դϴ�.
		&g_pVertexBuffer,         // ���� ������ �����͸� �ѱ�ϴ�.
		nullptr);                 // nullptr�� �����մϴ�.

	DXERR_HANDLER(g_DXResult);
	NULLCHK_RETURN_EFAIL(g_pVertexBuffer, "���� ���� �ʱ�ȭ ����!");
	
	// ���� ���ۿ� ������ ���� ������ �����մϴ�.
	// �޸𸮿� �����ϱ� ������ �޸𸮸� ��װ� Ǫ�� ������ �ֽ��ϴ�.
	void* pVertexData = nullptr;
	g_pVertexBuffer->Lock(
		0,                        // ������(Offset), �� ���� ��ġ�� �ǹ��ϴµ� ��ü ����� 0�Դϴ�.
		sizeof(CustomVertex) * 3, // ������ ���� ������ �뷮�� �Ѱ��ݴϴ�.
		&pVertexData,             // ����� ���� ������ �ٷ� �� �ִ� �����͸� �������ݴϴ�.
		D3DFLAG_NONE);            // ��� �÷����ε� 0���� �����մϴ�.
	::CopyMemory(pVertexData, vertices, sizeof(CustomVertex) * 3);
	g_pVertexBuffer->Unlock();

	return S_OK;
}

// ������ �Լ��Դϴ�.
// ���� ������ �۾��� Draw Call�� ó���˴ϴ�.
// Draw Call�� �����Ӵ� ȣ��Ǵ� ������ �Լ��� ���ϴµ� ȣ��Ǵ� �󵵼���
// �����ϸ� Draw Call Count�� �˾Ƴ� �� �ֽ��ϴ�.
HRESULT CALLBACK OnRender()
{
	D3DDEVICE9->SetFVF(CUSTOM_FVF);
	D3DDEVICE9->SetStreamSource(
		0,                     // ��Ʈ�� �ѹ��ε� 0���� �����մϴ�.
		g_pVertexBuffer,       // ���� ���۸� �������ݴϴ�.
		0,                     // �������ε� 0���� �����մϴ�.
		sizeof(CustomVertex)); // ����(Stride)�� �ǹ��ϴµ� FVF�� ������ ũ��� ��ġ�ؾ� �մϴ�.
	
	D3DDEVICE9->DrawPrimitive(
		D3DPT_TRIANGLELIST, // ������ ������ �����մϴ�.
		0,                  // �������ε� 0���� �����մϴ�.
		1);                 // �������� ������ �����մϴ�. ������ �� ������ ���� �۵� �������� ���մϴ�.

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	SAFE_RELEASE(g_pVertexBuffer);
	return S_OK;
}
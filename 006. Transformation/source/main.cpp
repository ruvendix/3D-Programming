#include "base_project.h"
#include "global_variable_definition.h"

// ====================================================================================
// ��ũ�� ���Ǻ��Դϴ�.

// FVF(Flexible Vertex Format) ������ �����մϴ�.
// DirectX9������ FVF ������ �ʿ��մϴ�.
// �Ʒ��� �ִ� ���� ��ȯ�Ǳ� ���� ������ǥ�� ������ ����ϰڴٴ� ���Դϴ�.
// D3DFVF_XYZRHW���� D3DFVF_XYZ�� ����˴ϴ�.
#define CUSTOM_FVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

// ====================================================================================
// ����ü �� ����ü ������Դϴ�.

// ������ ������ �����մϴ�.
// x, y, z, diffuse�� �����մϴ�.
// ��ȯ�Ǳ� ���̹Ƿ� rhw�� �����ϴ�.
struct CustomVertex
{
	D3DXVECTOR3 vPos;    // v�� vector�� �����Դϴ�. ������ǥ�� ���մϴ�.
	DWORD       dwColor; // ������ ������ �ǹ��մϴ�. Diffuse�� �л��̶�� ���Դϴ�.
};

// ====================================================================================
// ���� ���� ������Դϴ�.
namespace
{
	RX::RXMain_DX9*         g_pMainDX       = nullptr;
	IDirect3DDevice9*       g_pD3DDevice9   = nullptr;
	IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
	HRESULT                 g_hDXResult     = S_OK;
}

// ====================================================================================
// �Լ� ������Դϴ�.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

void ScalingTest(FLOAT rX, FLOAT rY, FLOAT rZ);
void Rotation2DTest(FLOAT rDegree);           // 2���������� ���Ϸ� ȸ���Դϴ�.
void RotationQuaternion2DTest(FLOAT rDegree); // 2���������� ���ʹϾ� ȸ���Դϴ�.
void TranslationTest(FLOAT rX, FLOAT rY, FLOAT rZ);

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

	// ���� ������ �޼���ǥ���̹Ƿ� �ð�����Դϴ�.
	// ���α׷��� ���߾��� ����(0, 0, 0)�� �ǹǷ�
	// �⺻ ī�޶� �°� ��ǥ�� �����ؾ� �մϴ�.
	CustomVertex vertices[3] =
	{
		{ {  0.0f,  0.2f , 0.0f }, DXCOLOR_RED   },
	    { {  0.2f, -0.2f , 0.0f }, DXCOLOR_GREEN },
		{ { -0.2f, -0.2f , 0.0f }, DXCOLOR_BLUE  },
	};

	// ���� ���۸� �����մϴ�.
	g_hDXResult = g_pD3DDevice9->CreateVertexBuffer(
		sizeof(CustomVertex) * 3, // ���� ������ ũ���Դϴ�.
		0,                        // �����ε� �Ϲ������δ� 0�Դϴ�.
		CUSTOM_FVF,               // FVF �����Դϴ�.
		D3DPOOL_MANAGED,          // �޸�Ǯ �����Դϴ�.
		&g_pVertexBuffer,         // ���� ������ �����͸� �ѱ�ϴ�.
		nullptr);                 // nullptr�� �����մϴ�.

	DXERR_HANDLER(g_hDXResult);
	NULLCHK_EFAIL_RETURN(g_pVertexBuffer, "���� ���� �ʱ�ȭ ����!");
	
	// ���� ���ۿ� ������ ���� ������ �����մϴ�.
	// �޸𸮿� �����ϱ� ������ �޸𸮸� ��װ� Ǫ�� ������ �ֽ��ϴ�.
	void* pVertexData = nullptr;
	g_pVertexBuffer->Lock(
		0,                        // ������(Offset), �� ���� ��ġ�� �ǹ��ϴµ� ��ü ����� 0�Դϴ�.
		sizeof(CustomVertex) * 3, // ������ ���� ������ ũ�⸦ �Ѱ��ݴϴ�.
		&pVertexData,             // ����� ���� ������ �ٷ� �� �ִ� �����͸� �������ݴϴ�.
		0);                       // ��� �÷����ε� 0���� �����մϴ�.
	::CopyMemory(pVertexData, vertices, sizeof(CustomVertex) * 3);
	g_pVertexBuffer->Unlock();

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
	// ũ�⺯ȯ����� �׽�Ʈ�մϴ�.
	//ScalingTest(2.0f, 2.0f, 0.0f);

	// ȸ������� �׽�Ʈ�մϴ�.
	//Rotation2DTest(30.0f);

	// ���ʹϾ� ȸ������� �׽�Ʈ�մϴ�.
	//RotationQuaternion2DTest(30.0f);

	// �̵������ �׽�Ʈ�մϴ�.
	//TranslationTest(0.4f, 0.4f, 0.0f);

	g_pD3DDevice9->SetFVF(CUSTOM_FVF);
	g_pD3DDevice9->SetStreamSource(
		0,                     // ��Ʈ�� �ѹ��ε� 0���� �����մϴ�.
		g_pVertexBuffer,       // ���� ���۸� �������ݴϴ�.
		0,                     // �������ε� 0���� �����մϴ�.
		sizeof(CustomVertex)); // ����(Stride)�� �ǹ��ϴµ� FVF�� ������ ũ��� ��ġ�ؾ� �մϴ�.
	
	g_pD3DDevice9->DrawPrimitive(
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


void ScalingTest(FLOAT rX, FLOAT rY, FLOAT rZ)
{
	D3DXMATRIXA16 matScale;
	D3DXMatrixScaling(&matScale, rX, rY, rZ);
	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &matScale);
}

void Rotation2DTest(FLOAT rDegree)
{
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationZ(&matRot, -D3DXToRadian(rDegree)); // ������ �������� �����ؾ� �մϴ�.
	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &matRot);
}

void RotationQuaternion2DTest(FLOAT rDegree)
{	
	// ���� ������ ���� �������ݴϴ�.
	// 2D������ ȸ���� Z�� �����Դϴ�.
	D3DXVECTOR3 vAxis(0.0f, 0.0f, 1.0f);

	// ������ ������ ������ ���ʹϾ��� �����մϴ�.
	D3DXQUATERNION quat;
	D3DXQuaternionRotationAxis(&quat, &vAxis, -D3DXToRadian(rDegree)); // ������ �������� �����ؾ� �մϴ�.

	// ���ʹϾ��� ��ķ� ��ȯ��ŵ�ϴ�.
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationQuaternion(&matRot, &quat);

	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &matRot);
}

void TranslationTest(FLOAT rX, FLOAT rY, FLOAT rZ)
{
	D3DXMATRIXA16 matTrans;
	D3DXMatrixTranslation(&matTrans, rX, rY, rZ);
	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &matTrans);
}
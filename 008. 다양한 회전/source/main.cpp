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

// ���� �� ȸ���Դϴ�.
void RotationBasisAxisTest(FLOAT rDegreeX, FLOAT rDegreeY, FLOAT rDegreeZ);

// ���Ϸ� �� ȸ���Դϴ�.
void RotationEulerAngleTest(FLOAT rDegreeX, FLOAT rDegreeY, FLOAT rDegreeZ);

// ������ �� ȸ���Դϴ�.
void RotationFreeAxis(const D3DXVECTOR3& vAxis, FLOAT rDegree);

// 2���������� ���ʹϾ� ȸ���Դϴ�.
void RotationQuaternionAxisTest(const D3DXVECTOR3& vAxis, FLOAT rDegree);


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
	// ���� �� ȸ���� �׽�Ʈ�մϴ�.
	//RotationBasisAxisTest(10.0f, 30.0f, -10.0f);

	// ���Ϸ� �� ȸ���� �׽�Ʈ�մϴ�.
	//RotationEulerAngleTest(10.0f, 30.0f, -10.0f);

	// ������ �� ȸ���� �׽�Ʈ�մϴ�.
	D3DXVECTOR3 vAxis(0.0f, 0.0f, 1.0f);
	//RotationFreeAxis(vAxis, -70.0f);

	// ���ʹϾ� ȸ���� �׽�Ʈ�մϴ�.
	// ���ʹϾ� ȸ������ ������ ���� �ʿ��մϴ�.
	//RotationQuaternionAxisTest(vAxis, -70.0f);

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

// ���Ϸ� �� ȸ���� ���߱� ���� Z -> X -> Y �����θ� ȸ���մϴ�.
void RotationBasisAxisTest(FLOAT rDegreeX, FLOAT rDegreeY, FLOAT rDegreeZ)
{
	D3DXMATRIXA16 matRotZ;
	D3DXMatrixIdentity(&matRotZ);
	D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(rDegreeZ));

	D3DXMATRIXA16 matRotX;
	D3DXMatrixIdentity(&matRotX);
	D3DXMatrixRotationX(&matRotX, D3DXToRadian(rDegreeX));

	D3DXMATRIXA16 matRotY;
	D3DXMatrixIdentity(&matRotY);
	D3DXMatrixRotationY(&matRotY, D3DXToRadian(rDegreeY));

	// Z * X * Y ������ ���մϴ�.
	// ���Ϸ� �� ȸ���� �⺻���Դϴ�.
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);
	matWorld = matRotZ * matRotX * matRotY;

	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &matWorld);
}

void RotationEulerAngleTest(FLOAT rDegreeX, FLOAT rDegreeY, FLOAT rDegreeZ)
{
	D3DXMATRIXA16 matRot;
	D3DXMatrixIdentity(&matRot);

	// Yaw   -> Y��
	// Pitch -> X��
	// Roll  -> Z��
	//
	// ������ ���ں��� ���޵ǹǷ� Roll -> Pitch -> Yaw ������ ȸ���˴ϴ�.
	D3DXMatrixRotationYawPitchRoll(&matRot,
		D3DXToRadian(rDegreeY), D3DXToRadian(rDegreeX), D3DXToRadian(rDegreeZ));
	
	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &matRot);
}

void RotationFreeAxis(const D3DXVECTOR3& vAxis, FLOAT rDegree)
{
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationAxis(&matRot, &vAxis, D3DXToRadian(rDegree));
	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &matRot);
}

void RotationQuaternionAxisTest(const D3DXVECTOR3& vAxis, FLOAT rDegree)
{
	// ������ ������ ������ ���ʹϾ��� �����մϴ�.
	D3DXQUATERNION quat;
	D3DXQuaternionRotationAxis(&quat, &vAxis, D3DXToRadian(rDegree));

	// ���ʹϾ��� ��ķ� ��ȯ��ŵ�ϴ�.
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationQuaternion(&matRot, &quat);

	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &matRot);
}
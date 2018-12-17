#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


// ====================================================================================
// ���� ���� ������Դϴ�.
IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
HRESULT                 g_DXResult      = S_OK;


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
	// ���α׷��� ���߾��� ����(0, 0, 0)�� �ǹǷ�
	// �⺻ ī�޶� �°� ��ǥ�� �����ؾ� �մϴ�.
	VertexP3D vertices[3] =
	{
		{ {  0.0f,  0.2f , 0.0f }, DXCOLOR_RED   },
	    { {  0.2f, -0.2f , 0.0f }, DXCOLOR_GREEN },
		{ { -0.2f, -0.2f , 0.0f }, DXCOLOR_BLUE  },
	};

	// ���� ���۸� �����մϴ�.
	g_DXResult = D3DDEVICE9->CreateVertexBuffer(
		sizeof(VertexP3D) * 3, // ���� ������ ũ���Դϴ�.
		D3DUSAGE_NONE,         // �����ε� �Ϲ������δ� 0�Դϴ�.
		VertexP3D::format,     // FVF �����Դϴ�.
		D3DPOOL_MANAGED,       // �޸�Ǯ �����Դϴ�.
		&g_pVertexBuffer,      // ���� ������ �����͸� �ѱ�ϴ�.
		nullptr);              // nullptr�� �����մϴ�.

	DXERR_HANDLER(g_DXResult);
	NULLCHK_RETURN_EFAIL(g_pVertexBuffer, "���� ���� �ʱ�ȭ ����!");
	
	// ���� ���ۿ� ������ ���� ������ �����մϴ�.
	// �޸𸮿� �����ϱ� ������ �޸𸮸� ��װ� Ǫ�� ������ �ֽ��ϴ�.
	void* pVertexData = nullptr;
	g_pVertexBuffer->Lock(
		0,                     // ������(Offset), �� ���� ��ġ�� �ǹ��ϴµ� ��ü ����� 0�Դϴ�.
		sizeof(VertexP3D) * 3, // ������ ���� ������ ũ�⸦ �Ѱ��ݴϴ�.
		&pVertexData,          // ����� ���� ������ �ٷ� �� �ִ� �����͸� �������ݴϴ�.
		D3DFLAG_NONE);         // ��� �÷����ε� 0���� �����մϴ�.
	::CopyMemory(pVertexData, vertices, sizeof(VertexP3D) * 3);
	g_pVertexBuffer->Unlock();

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

	D3DDEVICE9->SetFVF(VertexP3D::format);
	D3DDEVICE9->SetStreamSource(
		0,                  // ��Ʈ�� �ѹ��ε� 0���� �����մϴ�.
		g_pVertexBuffer,    // ���� ���۸� �������ݴϴ�.
		0,                  // �������ε� 0���� �����մϴ�.
		sizeof(VertexP3D)); // ����(Stride)�� �ǹ��ϴµ� FVF�� ������ ũ��� ��ġ�ؾ� �մϴ�.
	
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

	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matWorld);
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
	
	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matRot);
}

void RotationFreeAxis(const D3DXVECTOR3& vAxis, FLOAT rDegree)
{
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationAxis(&matRot, &vAxis, D3DXToRadian(rDegree));
	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matRot);
}

void RotationQuaternionAxisTest(const D3DXVECTOR3& vAxis, FLOAT rDegree)
{
	// ������ ������ ������ ���ʹϾ��� �����մϴ�.
	D3DXQUATERNION quat;
	D3DXQuaternionRotationAxis(&quat, &vAxis, D3DXToRadian(rDegree));

	// ���ʹϾ��� ��ķ� ��ȯ��ŵ�ϴ�.
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationQuaternion(&matRot, &quat);

	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matRot);
}
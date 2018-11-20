#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


// ====================================================================================
// ����ü ������Դϴ�.
struct LoadXFileInfo
{
	ID3DXMesh*          pMesh;         // �޽� �������̽� ������
	D3DXMATERIAL*       pMaterial;     // ��Ƽ����� �ؽ�ó �̸��� ��Ƶ� ������
	DWORD               materialCnt;   // ���� ��Ƽ���� ����
	IDirect3DTexture9** ppMeshTexture; // �޽ÿ� ���Ǵ� �ؽ�ó �������̽� �������� ������
};


// ====================================================================================
// ���� ���� ������Դϴ�.
IDirect3DDevice9* g_pD3DDevice9 = nullptr;
RX::RXMain_DX9*   g_pMainDX     = nullptr;

HRESULT       g_DXResult = S_OK;
D3DXMATRIXA16 g_matWorld;

namespace
{
	// ������ ���ͷθ� ���Ǵµ� �̹����� FLOAT 3���� ���� ������ ���ϴ�.
	D3DXVECTOR3 g_rotateAngle;

	// XFile �ε� ���� ���� �� �������� ���̱� ���� ����ü�� �������ϴ�.
	LoadXFileInfo g_loadXFileInfo;
}


// ====================================================================================
// �Լ� ������Դϴ�.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

// �⺻ ����� �����մϴ�.
void DefaultMatrix();

// �⺻ ���� ������Ʈ�� �����մϴ�.
void DefaultRenderState();

// XFile�� �ε��մϴ�.
void LoadXFile();

// ������� Ű���� �Ǵ� ���콺 �Է¿� ���� ó���� �մϴ�.
void OnUserInput();


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

	// �޸� �Ҵ� ������ �̿��ؼ� �޸� ������ ã���ϴ�.
	// ��ҿ��� �ּ� ó���ϸ� �˴ϴ�.
	//_CrtSetBreakAlloc(451);

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

	DefaultMatrix();
	DefaultRenderState();

	// XFile�� �ε��մϴ�.
	LoadXFile();

	// ���콺 Ŀ���� �����ݴϴ�.
	RX::ShowMouseCursor(true);

	return S_OK;
}

// ������Ʈ �Լ��Դϴ�.
// �������� ������ �ְų� ���α׷��� ������ �ִ�
// �������� �������Ӹ��� ������Ʈ�մϴ�.
HRESULT CALLBACK OnUpdate()
{
	OnUserInput();
	return S_OK;
}

// ������ �Լ��Դϴ�.
// ���� ������ �۾��� Draw Call�� ó���˴ϴ�.
// Draw Call�� �����Ӵ� ȣ��Ǵ� ������ �Լ��� ���ϴµ� ȣ��Ǵ� �󵵼���
// �����ϸ� Draw Call Count�� �˾Ƴ� �� �ֽ��ϴ�.
HRESULT CALLBACK OnRender()
{
	// XFile���� �ε��� �޽ø� �������մϴ�.
	for (INT32 i = 0; i < g_loadXFileInfo.materialCnt; ++i)
	{
		// �� �޽ø��� ��Ƽ������ �������ݴϴ�.
		g_pD3DDevice9->SetMaterial(&g_loadXFileInfo.pMaterial[i].MatD3D);

		// �� �޽ø��� �ؽ�ó�� �������ݴϴ�.
		if (g_loadXFileInfo.ppMeshTexture[i] != nullptr)
		{
			g_pD3DDevice9->SetTexture(0, g_loadXFileInfo.ppMeshTexture[i]);
		}

		// �ؽ�ó�� �������� �ʾƵ� �޽ô� ������ �����մϴ�.
		g_loadXFileInfo.pMesh->DrawSubset(i);
	}

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	for (INT32 i = 0; i < g_loadXFileInfo.materialCnt; ++i)
	{
		SAFE_RELEASE(g_loadXFileInfo.ppMeshTexture[i]);
	}
	
	SAFE_DELTE_ARR(g_loadXFileInfo.ppMeshTexture);
	SAFE_DELTE(g_loadXFileInfo.pMaterial);

	SAFE_RELEASE(g_loadXFileInfo.pMesh);

	return S_OK;
}

void DefaultMatrix()
{
	// =====================================================================
	// ������� �����մϴ�.
	D3DXVECTOR3 vEye(0.0f, 0.0f, -10.0f);   // ī�޶��� ��ġ
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // ī�޶� ���� ����
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // ī�޶��� �� ����

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	g_pD3DDevice9->SetTransform(D3DTS_VIEW, &matView);
	
	// =====================================================================
	// ��������� �����մϴ�.
	D3DXMATRIXA16 matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection, (D3DX_PI / 4.0f),
		(static_cast<FLOAT>(g_pMainDX->getClientWidth()) / (g_pMainDX->getClientHeight())),
		1.0f, 1000.0f);
	g_pD3DDevice9->SetTransform(D3DTS_PROJECTION, &matProjection);
}

void DefaultRenderState()
{
	// rhw�� ������� �ʴ´ٸ� ��ȯ ������ ������ǥ��
	// ����ϰ� �ǹǷ� ���� ��ȯ ������ ���ľ� �մϴ�.
	// ����(������, Lighting)�� ���� �ϳ��ε�
	// �̹����� ������ ������� �����Ƿ� ������ ���ݴϴ�.
	g_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, FALSE);

	// �� ��带 �����մϴ�. ����Ʈ�� �ָ����Դϴ�.
	g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	//g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);

	// �ø� ��带 �����մϴ�. ����Ʈ�� �ݽð���� �ø��Դϴ�.
	// ť�긦 Ȯ���ϱ� ���ؼ��� �ø� ��带 �����ؾ� �մϴ�.
	g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	//g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}

void LoadXFile()
{
	// XFile�� �ε��� �� ���Ǵ� ���ҽ� �����Դϴ�.
	// ���̴����� ���Ǵµ� ���� ������ XFile������ ���˴ϴ�.
	ID3DXBuffer* pMaterialBuffer = nullptr;

	// XFile�� �ε��մϴ�.
	g_DXResult = D3DXLoadMeshFromX(
		L"Resource/XFile/SimpleSword.x", // XFile ���
		D3DXMESH_MANAGED, // XFile���� �ε��� �޽��� ���� ���� �� �ε��� ���۸� ������ �޸�Ǯ
		g_pD3DDevice9,    // ���� ����̽� ��ü(���۷��� ī��Ʈ ����)
		nullptr, // ���� ������ ���� ������ ������� ����
		&pMaterialBuffer, // XFile�� ��Ƽ���� ������ ����
		nullptr, // ���� ������ ������� ����
		&g_loadXFileInfo.materialCnt, // XFile�� ��Ƽ���� ������ ����
		&g_loadXFileInfo.pMesh); // XFile�� �޽ø� ����
	DXERR_HANDLER(g_DXResult);

	// XFile���� �ε��� ��Ƽ���� ������ ĳ�����ؼ� ����մϴ�.
	// �̷��� �����͸� �ٸ� �����ͷ� �ؼ��� ���� reinterpret_cast<>�� ����ؾ� �������� �����ϴ�.
	D3DXMATERIAL* pMaterial = static_cast<D3DXMATERIAL*>(pMaterialBuffer->GetBufferPointer());

	// create a new material buffer for each material in the mesh
	INT32 materialCnt = g_loadXFileInfo.materialCnt;
	g_loadXFileInfo.pMaterial     = RXNew D3DXMATERIAL[materialCnt];
	g_loadXFileInfo.ppMeshTexture = RXNew IDirect3DTexture9*[materialCnt];
	
	for (INT32 i = 0; i < materialCnt; ++i)
	{
		// XFile���� �ε��� ��Ƽ���� ������ �����մϴ�.
		g_loadXFileInfo.pMaterial[i] = pMaterial[i];
		g_loadXFileInfo.pMaterial[i].MatD3D.Ambient = g_loadXFileInfo.pMaterial[i].MatD3D.Diffuse;
		
		// XFile���� �ε��� �ؽ�ó ������ �����ؾ� �ϴµ�
		// ���� XFile�� �ؽ�ó ��ΰ� �� �̻��ؼ� �̷��� �������� �ֽ��ϴ�.	
		// �ؽ�ó �ε��� �����ϸ� nullptr�� �����մϴ�.
		if (FAILED(D3DXCreateTextureFromFile(g_pD3DDevice9,
			L"Resource/Texture/SimpleSword_Diffuse.dds",
			&g_loadXFileInfo.ppMeshTexture[i])))
		{
			g_loadXFileInfo.ppMeshTexture[i] = nullptr;
		}
	}
}

void OnUserInput()
{
	if (::GetAsyncKeyState('Z') & 0x8000)
	{
		g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	if (::GetAsyncKeyState('X') & 0x8000)
	{
		g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}

	if (::GetAsyncKeyState('C') & 0x8000)
	{
		g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
	}

	if (::GetAsyncKeyState('A') & 0x8000)
	{
		g_rotateAngle.z += 4.0f;
	}

	if (::GetAsyncKeyState('D') & 0x8000)
	{
		g_rotateAngle.z -= 4.0f;
	}

	if (::GetAsyncKeyState('W') & 0x8000)
	{
		g_rotateAngle.x += 4.0f;
	}

	if (::GetAsyncKeyState('S') & 0x8000)
	{
		g_rotateAngle.x -= 4.0f;
	}

	if (::GetAsyncKeyState('Q') & 0x8000)
	{
		g_rotateAngle.y += 4.0f;
	}

	if (::GetAsyncKeyState('E') & 0x8000)
	{
		g_rotateAngle.y -= 4.0f;
	}

	if (::GetAsyncKeyState('R') & 0x8000)
	{
		RX::ZeroVector(&g_rotateAngle);
		g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	D3DXMATRIXA16 matScale;
	FLOAT rFactor = 1.0f / 36.0f;
	D3DXMatrixScaling(&matScale, rFactor, rFactor, rFactor);

	D3DXMATRIXA16 matTrans;
	D3DXMatrixTranslation(&matTrans, 0.0f, -3.5f, 0.0f);

	// ���� ����
	g_rotateAngle.z = RX::AdjustAngle(g_rotateAngle.z);
	g_rotateAngle.x = RX::AdjustAngle(g_rotateAngle.x);

	// ȸ������Դϴ�. ������ Z -> X -> Y�Դϴ�.
	// ��, Roll -> Pitch -> Yaw�Դϴ�.
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot,
		D3DXToRadian(g_rotateAngle.y),
		D3DXToRadian(g_rotateAngle.x),
		D3DXToRadian(g_rotateAngle.z));

	g_matWorld = matScale * matRot * matTrans;
	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &g_matWorld);
}
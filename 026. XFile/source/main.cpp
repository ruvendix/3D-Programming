#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


// ====================================================================================
// ����ü ������Դϴ�.
struct LoadXFileInfo
{
	ID3DXMesh*          pMesh;         // �޽� �������̽� ������
	D3DXMATERIAL*       pMaterial;     // ������ �ؽ�ó �̸��� ��Ƶ� ������
	DWORD               materialCnt;   // ���� ���� ����
	IDirect3DTexture9** ppMeshTexture; // �޽ÿ� ���Ǵ� �ؽ�ó �������̽� �������� ������
};


// ====================================================================================
// ���� ���� ������Դϴ�.
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

	RXMAIN_DX9->setSubFunc(OnInit,    SUBFUNC_TYPE::INIT);
	RXMAIN_DX9->setSubFunc(OnUpdate,  SUBFUNC_TYPE::UPDATE);
	RXMAIN_DX9->setSubFunc(OnRender,  SUBFUNC_TYPE::RENDER);
	RXMAIN_DX9->setSubFunc(OnRelease, SUBFUNC_TYPE::RELEASE);

	// �޸� �Ҵ� ������ �̿��ؼ� �޸� ������ ã���ϴ�.
	// ��ҿ��� �ּ� ó���ϸ� �˴ϴ�.
	//_CrtSetBreakAlloc(451);

	RXMAIN_DX9->RunMainRoutine(hInstance, IDI_RUVENDIX_ICO);
	return RXMAIN_DX9->getMessageCode();
}

// �ʱ�ȭ �Լ��Դϴ�.
// 3D �������� ������ ���� ���Ƿ� ������ �۾��� �ʱ�ȭ����� �մϴ�.
// �������ϸ鼭 �ǽð����� ���굵 ���������� �׷��� �Ǹ� �������� �������� �˴ϴ�.
// �Ϲ������� �������� ���� ������ �۾��� ó���մϴ�.
HRESULT CALLBACK OnInit()
{
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
	for (UINT32 i = 0; i < g_loadXFileInfo.materialCnt; ++i)
	{
		// �� �޽ø��� ������ �������ݴϴ�.
		D3DDEVICE9->SetMaterial(&g_loadXFileInfo.pMaterial[i].MatD3D);

		// �� �޽ø��� �ؽ�ó�� �������ݴϴ�.
		if (g_loadXFileInfo.ppMeshTexture[i] != nullptr)
		{
			D3DDEVICE9->SetTexture(0, g_loadXFileInfo.ppMeshTexture[i]);
		}

		// �ؽ�ó�� �������� �ʾƵ� �޽ô� ������ �����մϴ�.
		g_loadXFileInfo.pMesh->DrawSubset(i);
	}

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	for (UINT32 i = 0; i < g_loadXFileInfo.materialCnt; ++i)
	{
		SAFE_RELEASE(g_loadXFileInfo.ppMeshTexture[i]);
	}
	
	SAFE_DELTE_ARR(g_loadXFileInfo.ppMeshTexture);
	SAFE_DELTE_ARR(g_loadXFileInfo.pMaterial);

	SAFE_RELEASE(g_loadXFileInfo.pMesh);

	return S_OK;
}

void DefaultMatrix()
{
	// =====================================================================
	// ������� �����մϴ�.
	D3DXVECTOR3 vEye(0.0f, 0.0f, -10.0f);  // ī�޶��� ��ġ
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // ī�޶� ���� ����
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // ī�޶��� �� ����

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	D3DDEVICE9->SetTransform(D3DTS_VIEW, &matView);
	
	// =====================================================================
	// ��������� �����մϴ�.
	D3DXMATRIXA16 matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection, (D3DX_PI / 4.0f),
		(static_cast<FLOAT>(RXMAIN_DX9->getClientRect()->CalcWidth()) /
		                   (RXMAIN_DX9->getClientRect()->CalcHeight())), 1.0f, 1000.0f);
	D3DDEVICE9->SetTransform(D3DTS_PROJECTION, &matProjection);
}

void DefaultRenderState()
{
	// rhw�� ������� �ʴ´ٸ� ��ȯ ������ ������ǥ��
	// ����ϰ� �ǹǷ� ���� ��ȯ ������ ���ľ� �մϴ�.
	// ����(������, Lighting)�� ���� �ϳ��ε�
	// �̹����� ������ ������� �����Ƿ� ������ ���ݴϴ�.
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, FALSE);

	// �� ��带 �����մϴ�. ����Ʈ�� �ָ����Դϴ�.
	D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	//D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);

	// �ø� ��带 �����մϴ�. ����Ʈ�� �ݽð���� �ø��Դϴ�.
	// ť�긦 Ȯ���ϱ� ���ؼ��� �ø� ��带 �����ؾ� �մϴ�.
	D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	//D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
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
		D3DDEVICE9,    // ���� ����̽� ��ü(���۷��� ī��Ʈ ����)
		nullptr, // ���� ������ ���� ������ ������� ����
		&pMaterialBuffer, // XFile�� ���� ������ ����
		nullptr, // ���� ������ ������� ����
		&g_loadXFileInfo.materialCnt, // XFile�� ���� ������ ����
		&g_loadXFileInfo.pMesh); // XFile�� �޽ø� ����
	DXERR_HANDLER(g_DXResult);

	// XFile���� �ε��� ���� ������ ĳ�����ؼ� ����մϴ�.
	// �̷��� �����͸� �ٸ� �����ͷ� �ؼ��� ���� reinterpret_cast<>�� ����ؾ� �������� �����ϴ�.
	D3DXMATERIAL* pMaterial = reinterpret_cast<D3DXMATERIAL*>(pMaterialBuffer->GetBufferPointer());

	// �ΰ� ������ �����ϱ� ���� ������ �����մϴ�.
	INT32 materialCnt = g_loadXFileInfo.materialCnt;
	g_loadXFileInfo.pMaterial     = RXNew D3DXMATERIAL[materialCnt];
	g_loadXFileInfo.ppMeshTexture = RXNew IDirect3DTexture9*[materialCnt];
	
	for (INT32 i = 0; i < materialCnt; ++i)
	{
		// XFile���� �ε��� ���� ������ �����մϴ�.
		g_loadXFileInfo.pMaterial[i] = pMaterial[i];
		g_loadXFileInfo.pMaterial[i].MatD3D.Ambient = g_loadXFileInfo.pMaterial[i].MatD3D.Diffuse;
		
		// XFile���� �ε��� �ؽ�ó ������ �����ؾ� �ϴµ�
		// ���� XFile�� �ؽ�ó ��ΰ� �� �̻��ؼ� �̷��� �������� �ֽ��ϴ�.	
		// �ؽ�ó �ε��� �����ϸ� nullptr�� �����մϴ�.
		if (FAILED(D3DXCreateTextureFromFile(D3DDEVICE9,
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
		D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	if (::GetAsyncKeyState('X') & 0x8000)
	{
		D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}

	if (::GetAsyncKeyState('C') & 0x8000)
	{
		D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
	}

	FLOAT rDeltaSeconds = RX::RXProgramFPSMgr::Instance()->getTimer()->getDeltaSeconds();
	if (::GetAsyncKeyState('A') & 0x8000)
	{
		g_rotateAngle.z += 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('D') & 0x8000)
	{
		g_rotateAngle.z -= 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('W') & 0x8000)
	{
		g_rotateAngle.x += 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('S') & 0x8000)
	{
		g_rotateAngle.x -= 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('Q') & 0x8000)
	{
		g_rotateAngle.y += 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('E') & 0x8000)
	{
		g_rotateAngle.y -= 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('R') & 0x8000)
	{
		RX::ZeroVector(&g_rotateAngle);
		D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
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
	D3DDEVICE9->SetTransform(D3DTS_WORLD, &g_matWorld);
}
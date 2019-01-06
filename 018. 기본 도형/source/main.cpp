#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"
#include "RX\RX3DAxisDX9.h"


// ====================================================================================
// ����ü ������Դϴ�.
enum class SHAPE_TYPE : INT32
{
	NONE = 0,
	BOX,
	SPHERE,
	TEAPOT,
	TORUS,
	CYLINDER,
	END,
};


// ====================================================================================
// ���� ���� ������Դϴ�.
D3DXMATRIXA16 g_matViewAndProjection; // �̸� ����ص� ����� * �������
D3DXMATRIXA16 g_matProjection;        // �̸� ����ص� �������

HRESULT g_DXResult = S_OK;

namespace
{
	RX::RX3DAxisDX9 g_axis; // 3D���� �ٷ�� ���� ��
	ID3DXMesh* g_pMesh = nullptr; // �޽� �������̽�
	INT32 g_shapeType  = 0;

	// ������ ���ͷθ� ���Ǵµ� �̹����� FLOAT 3���� ���� ������ ���ϴ�.
	D3DXVECTOR3 g_rotateAngle;
}


// ====================================================================================
// �Լ� ������Դϴ�.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

// �⺻ ����� �� ������������� �����մϴ�.
void DefaultViewAndProjection();

// �⺻ ������ �����մϴ�.
void DefaultLight();

// �⺻ ���� ������Ʈ�� �����մϴ�.
void DefaultRenderState();

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
	DefaultViewAndProjection();
	DefaultLight();
	DefaultRenderState();

	// ���� �����մϴ�.
	g_axis.CreateAxis(20.0f);

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
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_axis.DrawAxis();
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, TRUE);

	if (g_pMesh)
	{
		g_pMesh->DrawSubset(0);
	}

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	g_axis.Release();
	SAFE_RELEASE(g_pMesh);
	return S_OK;
}

void DefaultViewAndProjection()
{
	// ������� �����մϴ�.
	D3DXVECTOR3 vEye(10.0f, 10.0f, 10.0f);   // ī�޶��� ��ġ
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
	D3DDEVICE9->SetTransform(D3DTS_PROJECTION, &g_matProjection);

	// ������� �ʱ�ȭ�Դϴ�.
	g_matViewAndProjection = matView * g_matProjection;
}

void DefaultLight()
{
	// ������ �����ϰ� ����մϴ�.
	// �̹����� ���� ������ �̿��մϴ�.
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	
	D3DXVECTOR3 vDir = { -1.0f, -1.0f, 0.2f };
	D3DXVec3Normalize(&vDir, &vDir);
	light.Direction = vDir;

	light.Ambient  = D3DXCOLOR(DXCOLOR_WHITE);
	light.Diffuse  = D3DXCOLOR(DXCOLOR_WHITE);
	light.Specular = D3DXCOLOR(DXCOLOR_WHITE);

	// ������ ����ϰ� Ȱ��ȭ��ŵ�ϴ�.
	D3DDEVICE9->SetLight(0, &light);
	D3DDEVICE9->LightEnable(0, TRUE);

	// ������ �ϳ��� ��� �����մϴ�.
	// ���� ������ ���� �ٲ㰡�� �������� ���� �����ϴ�.
	D3DXMATERIAL mtrl;
	::ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.MatD3D.Ambient  = D3DXCOLOR(DXCOLOR_BLACK);
	mtrl.MatD3D.Diffuse  = D3DXCOLOR(DXCOLOR_BLUE);
	
	// �ݻ籤 �����Դϴ�.
	// �ݻ籤���� Power�� �ִµ� �̰� ������ �ݻ� ������ �پ���
	// �̰� ������ �ݻ� ������ �о����ϴ�.
	// �ݻ籤�� ����Ϸ��� ���� ������Ʈ���� �ݻ籤�� Ȱ���ؾ� �մϴ�.
	mtrl.MatD3D.Specular = D3DXCOLOR(DXCOLOR_WHITE);
	mtrl.MatD3D.Power    = 20.0f;

	// ��籤 �����Դϴ�.
	// ��籤�� ���� �����ؾ� �մϴ�.
	mtrl.MatD3D.Emissive = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);

	D3DDEVICE9->SetMaterial(&(mtrl.MatD3D));
}

void DefaultRenderState()
{
	// rhw�� ������� �ʴ´ٸ� ��ȯ ������ ������ǥ��
	// ����ϰ� �ǹǷ� ���� ��ȯ ������ ���ľ� �մϴ�.
	// ����(������, Lighting)�� ���� �ϳ��ε�
	// �̹����� ������ ����ϹǷ� ������ ���ݴϴ�.
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, TRUE);

	// �� ��带 �����մϴ�. ����Ʈ�� �ָ����Դϴ�.
	D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	//D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);

	// �ø� ��带 �����մϴ�. ����Ʈ�� �ݽð���� �ø��Դϴ�.
	// ť�긦 Ȯ���ϱ� ���ؼ��� �ø� ��带 �����ؾ� �մϴ�.
	D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	//D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// �������͸� �ڵ����� ������ִ� �����Դϴ�.
	// ��! �� ������ �̿��ϰ� �Ǹ� ����� �� ž�ϴ�...
	// ����Ʈ�� FALSE�Դϴ�.
	D3DDEVICE9->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// �ݻ籤�� Ȱ����ŵ�ϴ�.
	D3DDEVICE9->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
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

	if (::GetAsyncKeyState('F') & 0x0001)
	{
		g_axis.FlipEnable();
	}

	if (::GetAsyncKeyState('V') & 0x0001)
	{
		++g_shapeType;
		SHAPE_TYPE shapeType = static_cast<SHAPE_TYPE>(g_shapeType);
		if (shapeType >= SHAPE_TYPE::END)
		{
			shapeType = SHAPE_TYPE::NONE;
			g_shapeType = static_cast<INT32>(shapeType);
		}

		switch (shapeType)
		{
		case SHAPE_TYPE::BOX:
		{
			SAFE_RELEASE(g_pMesh);
			D3DXCreateBox(
				D3DDEVICE9, // ���� ����̽�
				1.4f, // ���� ����
				1.4f, // ���� ����
				1.4f, // ����
				&g_pMesh, // �޽� �������̽� ������
				nullptr); // �� ���� ��
			break;
		}
		case SHAPE_TYPE::SPHERE:
		{
			SAFE_RELEASE(g_pMesh);
			D3DXCreateSphere(
				D3DDEVICE9, // ���� ����̽�
				1.0f, // ������
				80, // Z�� �����̽�
				80, // X�� ����
				&g_pMesh, // �޽� �������̽� ������
				nullptr); // �� ���� ��
			break;
		}
		case SHAPE_TYPE::TEAPOT:
		{
			SAFE_RELEASE(g_pMesh);
			D3DXCreateTeapot(
				D3DDEVICE9, // ���� ����̽�
				&g_pMesh, // �޽� �������̽� ������
				nullptr); // �� ���� ��
			break;
		}
		case SHAPE_TYPE::TORUS:
		{
			SAFE_RELEASE(g_pMesh);
			D3DXCreateTorus(
				D3DDEVICE9, // ���� ����̽�
				0.4f, // ���ο� ������
				0.8f, // �ܺο� ������
				80, // Z�� �����̽�
				80, // X�� ����
				&g_pMesh, // �޽� �������̽� ������
				nullptr); // �� ���°�
			break;
		}
		case SHAPE_TYPE::CYLINDER:
		{
			SAFE_RELEASE(g_pMesh);
			D3DXCreateCylinder(
				D3DDEVICE9, // ���� ����̽�
				0.4f, // ù��° ������
				0.6f, // �ι�° ������
				1.4f, // ����
				40, // Z�� �����̽�
				40, // X�� ����
				&g_pMesh, // �޽� �������̽� ������
				nullptr); // �� ���� ��
			break;
		}
		case SHAPE_TYPE::NONE:
		{
			SAFE_RELEASE(g_pMesh);
			break;
		}
		}
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

	// ���� ����
	g_rotateAngle.z = RX::AdjustAngle(g_rotateAngle.z);
	g_rotateAngle.x = RX::AdjustAngle(g_rotateAngle.x);
	g_rotateAngle.y = RX::AdjustAngle(g_rotateAngle.y);

	// ȸ������Դϴ�. ������ Z -> X -> Y�Դϴ�.
	// ��, Roll -> Pitch -> Yaw�Դϴ�.
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot,
		D3DXToRadian(g_rotateAngle.y),
		D3DXToRadian(g_rotateAngle.x),
		D3DXToRadian(g_rotateAngle.z));

	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matRot);
}
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
IDirect3DDevice9* g_pD3DDevice9 = nullptr;
RX::RXMain_DX9*   g_pMainDX     = nullptr;

D3DXMATRIXA16 g_matViewAndProjection; // �̸� ����ص� ����� * �������
D3DXMATRIXA16 g_matProjection;        // �̸� ����ص� �������

HRESULT g_DXResult = S_OK;

namespace
{
	RX::RX3DAxisDX9 g_axis; // 3D���� �ٷ�� ���� ��
	ID3DXMesh* g_pMesh = nullptr; // �޽� �������̽�
	INT32 g_shapeType;

	// ������ ���ͷθ� ���Ǵµ� �̹����� FLOAT 3���� ���� ������ ���ϴ�.
	D3DXVECTOR3 g_roateAngle;

	// �̸� ���� ����Դϴ�.
	D3DXMATRIXA16 g_matTextScale;
	D3DXMATRIXA16 g_matTextTrans;
}

HDC g_hMainDC = nullptr;

// ====================================================================================
// �Լ� ������Դϴ�.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

// �⺻ ����� �����մϴ�.
void DefaultMatrix();

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
	DefaultLight();
	DefaultRenderState();

	// ���� �����մϴ�.
	g_axis.CreateAxis(20.0f);

	// ���콺 Ŀ���� �����ݴϴ�.
	RX::ShowMouseCursor(true);

	// ��Ʈ �׽�Ʈ
	IDirect3DSurface9* pBackBufferSurface = nullptr;
	g_pD3DDevice9->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBufferSurface);
	NULLCHK(pBackBufferSurface);

	HDC hBackBufferDC = nullptr;
	pBackBufferSurface->GetDC(&hBackBufferDC);


	LOGFONT logicalFont; // LOOGFONT�� Logical Font�� �����Դϴ�.
	::ZeroMemory(&logicalFont, sizeof(logicalFont));

	// �ý��� �����Ͽ� ���缭 ���� ������ �������ݴϴ�.
	logicalFont.lfCharSet = DEFAULT_CHARSET;

	// ��Ʈ�� �����մϴ�. "C:\Windows\Fonts\"�� �ִ� ��Ʈ�� ���� �����մϴ�.
	// ��, �ڽ��� ��ǻ�Ϳ��� ��Ʈ�� �־ �ٸ� ��ǻ�Ϳ��� ���� �� �ֽ��ϴ�.
	// ���α׷��� ������ ���� ��Ʈ�� ����ؾ� �� ������� ����صμ���.
	// _TRUNCATE�� -1�� ��ȣ ���� ������ ǥ���� ���ε� 42�� ���� �˴ϴ�.
	// ��, ������ ���̿� ������ ���� �ʰڴٴ� �ǹ��Դϴ�.
	wcsncpy_s(logicalFont.lfFaceName, _countof(logicalFont.lfFaceName), L"����", _TRUNCATE);

	// Win32 API�� �̿��ؼ� ��Ʈ�� �����մϴ�.
	// �ؽ�Ʈ�� �޽÷� �����ϱ� ���� �����Դϴ�.
	HFONT hFont = ::CreateFontIndirect(&logicalFont);

	// ������� DC ������ �����ؼ� ���ο� DC�� �����
	// ������ DC�� ��Ʈ �ڵ��� �����մϴ�.
	HDC hDC = ::CreateCompatibleDC(hBackBufferDC);
	HFONT hOldFont = static_cast<HFONT>(::SelectObject(hDC, hFont));

	g_DXResult = D3DXCreateText(
		g_pD3DDevice9, // ���� ����̽� ��ü
		hDC, // ��Ʈ�� �ִ� DC
		L"��� ����~!", // �ؽ�Ʈ
		0.0f, // ��Ʈ ������� �ǵ� �׳� 0.0���� �����ϸ� ��
		0.5f, // ��Ʈ�� �����ε� ���� Ŀ������ ������ Ƣ���...
		&g_pMesh, nullptr, nullptr);
	DXERR_HANDLER(g_DXResult);

	// ���� ��Ʈ�� �����ϰ� ��Ʈ �� DC�� �����մϴ�.
	hFont = static_cast<HFONT>(::SelectObject(hDC, hOldFont));
	::DeleteObject(hFont);
	::DeleteDC(hDC);

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
	g_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_axis.DrawAxis();
	g_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, TRUE);

	// ���� ����� ����� ȸ����Ļ��̶� �̷��� �����ɴϴ�.
	// ������ �ٸ� ������� ȸ������� �����ɴϴ�.
	D3DXMATRIXA16 matRot;
	g_pD3DDevice9->GetTransform(D3DTS_WORLD, &matRot);

	// ���� ���Ϸ��� ȸ���� �̿� ���ε� �Ϲ������� �ƴ� ������
	// ũ�� * ���� * �̵��� �ƴ϶� ũ�� * ���� * �̵����� �����ؾ� �մϴ�.
	D3DXMATRIXA16 matTextWorld;
	matTextWorld = g_matTextScale * g_matTextTrans * matRot;
	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &matTextWorld);
	
	if (g_pMesh != nullptr)
	{
		// �ؽ�Ʈ�� �޽÷� ����������� DrawSubset()���� �������մϴ�.
		g_pMesh->DrawSubset(0);
	}

	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &matRot);

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	g_axis.Release();
	SAFE_RELEASE(g_pMesh);
	return S_OK;
}

void DefaultMatrix()
{
	// =====================================================================
	// ��������� �����մϴ�.
	D3DXMatrixScaling(&g_matTextScale, 0.8f, 0.8f, 0.8f);
	D3DXMatrixTranslation(&g_matTextTrans, -2.0f, 0.0f, 0.0f);

	// =====================================================================
	// ������� �����մϴ�.
	D3DXVECTOR3 vEye(1.0f, 4.0f, -3.0f);   // ī�޶��� ��ġ
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // ī�޶� ���� ����
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // ī�޶��� �� ����

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	g_pD3DDevice9->SetTransform(D3DTS_VIEW, &matView);
	
	// =====================================================================
	// ��������� �����մϴ�.
	D3DXMatrixPerspectiveFovLH(&g_matProjection, (D3DX_PI / 4.0f),
		(static_cast<FLOAT>(g_pMainDX->getClientWidth()) / (g_pMainDX->getClientHeight())),
		1.0f, 1000.0f);
	g_pD3DDevice9->SetTransform(D3DTS_PROJECTION, &g_matProjection);
	
	// =====================================================================
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
	g_pD3DDevice9->SetLight(0, &light);
	g_pD3DDevice9->LightEnable(0, TRUE);

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

	g_pD3DDevice9->SetMaterial(&(mtrl.MatD3D));
}

void DefaultRenderState()
{
	// rhw�� ������� �ʴ´ٸ� ��ȯ ������ ������ǥ��
	// ����ϰ� �ǹǷ� ���� ��ȯ ������ ���ľ� �մϴ�.
	// ����(������, Lighting)�� ���� �ϳ��ε�
	// �̹����� ������ ����ϹǷ� ������ ���ݴϴ�.
	g_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, TRUE);

	// �� ��带 �����մϴ�. ����Ʈ�� �ָ����Դϴ�.
	g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	//g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);

	// �ø� ��带 �����մϴ�. ����Ʈ�� �ݽð���� �ø��Դϴ�.
	// ť�긦 Ȯ���ϱ� ���ؼ��� �ø� ��带 �����ؾ� �մϴ�.
	g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	//g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// �������͸� �ڵ����� ������ִ� �����Դϴ�.
	// ��! �� ������ �̿��ϰ� �Ǹ� ����� �� ž�ϴ�...
	// ����Ʈ�� FALSE�Դϴ�.
	g_pD3DDevice9->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// �ݻ籤�� Ȱ����ŵ�ϴ�.
	g_pD3DDevice9->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
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

	if (::GetAsyncKeyState('F') & 0x0001)
	{
		g_axis.FlipEnable();
	}

	if (::GetAsyncKeyState('A'))
	{
		g_roateAngle.z += 4.0f;
	}

	if (::GetAsyncKeyState('D'))
	{
		g_roateAngle.z -= 4.0f;
	}

	if (::GetAsyncKeyState('W'))
	{
		g_roateAngle.x += 4.0f;
	}

	if (::GetAsyncKeyState('S'))
	{
		g_roateAngle.x -= 4.0f;
	}

	if (::GetAsyncKeyState('Q'))
	{
		g_roateAngle.y += 4.0f;
	}

	if (::GetAsyncKeyState('E'))
	{
		g_roateAngle.y -= 4.0f;
	}

	if (::GetAsyncKeyState('R'))
	{
		RX::ZeroVector(&g_roateAngle);
		g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	// ���� ����
	g_roateAngle.z = RX::AdjustAngle(g_roateAngle.z);
	g_roateAngle.x = RX::AdjustAngle(g_roateAngle.x);
	g_roateAngle.y = RX::AdjustAngle(g_roateAngle.y);

	// ȸ������Դϴ�. ������ Z -> X -> Y�Դϴ�.
	// ��, Roll -> Pitch -> Yaw�Դϴ�.
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot,
		D3DXToRadian(g_roateAngle.y),
		D3DXToRadian(g_roateAngle.x),
		D3DXToRadian(g_roateAngle.z));

	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &matRot);
}
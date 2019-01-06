#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"
#include "RX\RX3DAxisDX9.h"


// ====================================================================================
// ���� ���� ������Դϴ�.
HRESULT g_DXResult = S_OK;

namespace
{
	RX::RX3DAxisDX9 g_axis; // 3D���� �ٷ�� ���� ��
	ID3DXMesh* g_pMesh = nullptr; // �޽� �������̽�

	D3DXVECTOR3 g_rotateAngle;

	// �̸� ���� ����Դϴ�.
	D3DXMATRIXA16 g_matTextScale;
	D3DXMATRIXA16 g_matTextTrans;
}


// ====================================================================================
// �Լ� ������Դϴ�.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

void DefaultMatrix();
void DefaultLight();
void DefaultRenderState();
void OnUserInput();

// ��Ʈ ������ �޴� �ݹ� �Լ��Դϴ�.
// ���ڴ� ������� �� ��Ʈ, ���� ��Ʈ, ��Ʈ ����, �߰� �����Դϴ�.
INT32 CALLBACK EnumFontCallback(const LOGFONT* pLogicalFont,
	const TEXTMETRIC* pPhysicalFont, DWORD fontType, LPARAM lParam);


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

HRESULT CALLBACK OnInit()
{
	DefaultMatrix();
	DefaultLight();
	DefaultRenderState();

	g_axis.CreateAxis(20.0f);

	RX::ShowMouseCursor(true);

	// ������� �����̽��� �����ɴϴ�.
	IDirect3DSurface9* pBackBufferSurface = nullptr;
	D3DDEVICE9->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBufferSurface);
	NULLCHK(pBackBufferSurface);

	// ������� DC�� �����ͼ� �����մϴ�.
	HDC hBackBufferDC = nullptr;
	pBackBufferSurface->GetDC(&hBackBufferDC);
	SAFE_RELEASE(pBackBufferSurface); // ���۷��� ī��Ʈ�� ���������Ƿ� ���� �ʼ�!
	HDC hDC = ::CreateCompatibleDC(hBackBufferDC);

	LOGFONT logicalFont; // LOOGFONT�� Logical Font�� �����Դϴ�.
	::ZeroMemory(&logicalFont, sizeof(logicalFont));

	// �ý��� �����Ͽ� ���缭 ���� ������ �������ݴϴ�.
	logicalFont.lfCharSet = DEFAULT_CHARSET;

	// ���� ��ġ�� ��� ��Ʈ�� ã���ϴ�.
	::EnumFontFamiliesEx(
		hDC, // ��Ʈ�� ������ DC
		&logicalFont,     // ��Ʈ ������ ���� LOGFONT ����ü�� ������
		EnumFontCallback, // ��Ʈ ������ ���� �ݹ� �Լ�
		0,  // �߰� ������ ���� ������
		0); // �� ���� ��

	// ��Ʈ�� �����մϴ�. "C:\Windows\Fonts\"�� �ִ� ��Ʈ�� ���� �����մϴ�.
	// ��, �ڽ��� ��ǻ�Ϳ��� ��Ʈ�� �־ �ٸ� ��ǻ�Ϳ��� ���� �� �ֽ��ϴ�.
	// ���α׷��� ������ ���� ��Ʈ�� ����ؾ� �� ������� ����صμ���.
	// _TRUNCATE�� -1�� ��ȣ ���� ������ ǥ���� ���ε� 42�� ���� �˴ϴ�.
	// ��, ������ ���̿� ������ ���� �ʰڴٴ� �ǹ��Դϴ�.
	wcsncpy_s(logicalFont.lfFaceName, _countof(logicalFont.lfFaceName), L"����", _TRUNCATE);

	// Win32 API�� �̿��ؼ� ��Ʈ�� �����մϴ�.
	// �ؽ�Ʈ�� �޽÷� �����ϱ� ���� �����Դϴ�.
	HFONT hFont = ::CreateFontIndirect(&logicalFont);

	// ������ DC�� ��Ʈ �ڵ��� �����մϴ�.
	HFONT hOldFont = static_cast<HFONT>(::SelectObject(hDC, hFont));

	g_DXResult = D3DXCreateText(
		D3DDEVICE9, // ���� ����̽� ��ü
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

HRESULT CALLBACK OnUpdate()
{
	OnUserInput();
	return S_OK;
}

HRESULT CALLBACK OnRender()
{
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_axis.DrawAxis();
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, TRUE);

	// ���� ����� ����� ȸ����Ļ��̶� �̷��� �����ɴϴ�.
	// ������ �ٸ� ������� ȸ������� �����ɴϴ�.
	D3DXMATRIXA16 matRot;
	D3DDEVICE9->GetTransform(D3DTS_WORLD, &matRot);

	// ���� ���Ϸ��� ȸ���� �̿� ���ε� �Ϲ������� �ƴ� ������
	// ũ�� * ���� * �̵��� �ƴ϶� ũ�� * �̵� * �������� �����ؾ� �մϴ�.
	D3DXMATRIXA16 matTextWorld;
	matTextWorld = g_matTextScale * g_matTextTrans * matRot;
	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matTextWorld);
	
	if (g_pMesh != nullptr)
	{
		// �ؽ�Ʈ�� �޽÷� ����������� DrawSubset()���� �������մϴ�.
		g_pMesh->DrawSubset(0);
	}

	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matRot);

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	g_axis.Release();
	SAFE_RELEASE(g_pMesh);
	return S_OK;
}

INT32 CALLBACK EnumFontCallback(const LOGFONT* pLogicalFont,
	const TEXTMETRIC* pPhysicalFont, DWORD fontType, LPARAM lParam)
{
	const ENUMLOGFONTEX* pData = reinterpret_cast<const ENUMLOGFONTEX*>(pLogicalFont);
	NULLCHK(pData);
	RXLOG("%s(%s)", pData->elfFullName, pData->elfScript); // ��Ʈ �̸�(�Ҽ�)
	return -1;
}

#pragma region �ߺ��Ǵ� ���� �ڵ�
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
	D3DDEVICE9->SetTransform(D3DTS_VIEW, &matView);
	// =====================================================================
	// ��������� �����մϴ�.
	D3DXMATRIXA16 matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection, (D3DX_PI / 4.0f),
		(static_cast<FLOAT>(RXMAIN_DX9->getClientRect()->CalcWidth()) /
		(RXMAIN_DX9->getClientRect()->CalcHeight())), 1.0f, 1000.0f);
	D3DDEVICE9->SetTransform(D3DTS_PROJECTION, &matProjection);
	// =====================================================================
}

void DefaultLight()
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;

	D3DXVECTOR3 vDir = { -1.0f, -1.0f, 0.2f };
	D3DXVec3Normalize(&vDir, &vDir);
	light.Direction = vDir;

	light.Ambient = D3DXCOLOR(DXCOLOR_WHITE);
	light.Diffuse = D3DXCOLOR(DXCOLOR_WHITE);
	light.Specular = D3DXCOLOR(DXCOLOR_WHITE);

	D3DDEVICE9->SetLight(0, &light);
	D3DDEVICE9->LightEnable(0, TRUE);

	D3DXMATERIAL mtrl;
	::ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.MatD3D.Ambient = D3DXCOLOR(DXCOLOR_BLACK);
	mtrl.MatD3D.Diffuse = D3DXCOLOR(DXCOLOR_BLUE);
	mtrl.MatD3D.Specular = D3DXCOLOR(DXCOLOR_WHITE);
	mtrl.MatD3D.Power = 20.0f;
	mtrl.MatD3D.Emissive = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);

	D3DDEVICE9->SetMaterial(&(mtrl.MatD3D));
}

void DefaultRenderState()
{
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, TRUE);
	D3DDEVICE9->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
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

	g_rotateAngle.z = RX::AdjustAngle(g_rotateAngle.z);
	g_rotateAngle.x = RX::AdjustAngle(g_rotateAngle.x);
	g_rotateAngle.y = RX::AdjustAngle(g_rotateAngle.y);

	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot,
		D3DXToRadian(g_rotateAngle.y),
		D3DXToRadian(g_rotateAngle.x),
		D3DXToRadian(g_rotateAngle.z));

	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matRot);
}
#pragma endregion
#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"
#include "RX\RX3DAxisDX9.h"


// ====================================================================================
// 전역 변수 선언부입니다.
HRESULT g_DXResult = S_OK;

namespace
{
	RX::RX3DAxisDX9 g_axis; // 3D축을 다루기 위한 것
	ID3DXMesh* g_pMesh = nullptr; // 메시 인터페이스

	D3DXVECTOR3 g_rotateAngle;

	// 미리 계산된 행렬입니다.
	D3DXMATRIXA16 g_matTextScale;
	D3DXMATRIXA16 g_matTextTrans;
}


// ====================================================================================
// 함수 선언부입니다.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

void DefaultMatrix();
void DefaultLight();
void DefaultRenderState();
void OnUserInput();

// 폰트 정보를 받는 콜백 함수입니다.
// 인자는 순서대로 논리 폰트, 물리 폰트, 폰트 형식, 추가 정보입니다.
INT32 CALLBACK EnumFontCallback(const LOGFONT* pLogicalFont,
	const TEXTMETRIC* pPhysicalFont, DWORD fontType, LPARAM lParam);


// ====================================================================================
// <Win32 API는 WinMain()이 진입점입니다>
INT32 APIENTRY _tWinMain(HINSTANCE hInstance,
	                     HINSTANCE hPrevInstance,
	                     LPTSTR    szCmdLine,
	                     INT32     cmdShow)
{
	// 사용하지 않는 매개변수를 표시해줍니다.
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(szCmdLine);
	UNREFERENCED_PARAMETER(cmdShow);

	RXMAIN_DX9->setSubFunc(OnInit,    SUBFUNC_TYPE::INIT);
	RXMAIN_DX9->setSubFunc(OnUpdate,  SUBFUNC_TYPE::UPDATE);
	RXMAIN_DX9->setSubFunc(OnRender,  SUBFUNC_TYPE::RENDER);
	RXMAIN_DX9->setSubFunc(OnRelease, SUBFUNC_TYPE::RELEASE);

	// 메모리 할당 순서를 이용해서 메모리 누수를 찾습니다.
	// 평소에는 주석 처리하면 됩니다.
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

	// 백버퍼의 서페이스를 가져옵니다.
	IDirect3DSurface9* pBackBufferSurface = nullptr;
	D3DDEVICE9->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBufferSurface);
	NULLCHK(pBackBufferSurface);

	// 백버퍼의 DC를 가져와서 복사합니다.
	HDC hBackBufferDC = nullptr;
	pBackBufferSurface->GetDC(&hBackBufferDC);
	SAFE_RELEASE(pBackBufferSurface); // 레퍼런스 카운트가 증가했으므로 해제 필수!
	HDC hDC = ::CreateCompatibleDC(hBackBufferDC);

	LOGFONT logicalFont; // LOOGFONT는 Logical Font의 약자입니다.
	::ZeroMemory(&logicalFont, sizeof(logicalFont));

	// 시스템 로케일에 맞춰서 문자 집합을 설정해줍니다.
	logicalFont.lfCharSet = DEFAULT_CHARSET;

	// 현재 설치된 모든 폰트를 찾습니다.
	::EnumFontFamiliesEx(
		hDC, // 폰트를 적용할 DC
		&logicalFont,     // 폰트 정보를 받을 LOGFONT 구조체의 포인터
		EnumFontCallback, // 폰트 정보를 받을 콜백 함수
		0,  // 추가 정보를 받을 포인터
		0); // 안 쓰는 값

	// 폰트를 설정합니다. "C:\Windows\Fonts\"에 있는 폰트만 설정 가능합니다.
	// 즉, 자신의 컴퓨터에는 폰트가 있어도 다른 컴퓨터에는 없을 수 있습니다.
	// 프로그램을 배포할 때는 폰트도 고려해야 할 대상임을 기억해두세요.
	// _TRUNCATE은 -1을 부호 없는 정수로 표현한 값인데 42억 정도 됩니다.
	// 즉, 복사할 길이에 제한을 두지 않겠다는 의미입니다.
	wcsncpy_s(logicalFont.lfFaceName, _countof(logicalFont.lfFaceName), L"굴림", _TRUNCATE);

	// Win32 API를 이용해서 폰트를 생성합니다.
	// 텍스트를 메시로 생성하기 위한 과정입니다.
	HFONT hFont = ::CreateFontIndirect(&logicalFont);

	// 복사한 DC에 폰트 핸들을 적용합니다.
	HFONT hOldFont = static_cast<HFONT>(::SelectObject(hDC, hFont));

	g_DXResult = D3DXCreateText(
		D3DDEVICE9, // 가상 디바이스 객체
		hDC, // 폰트가 있는 DC
		L"헬로 월드~!", // 텍스트
		0.0f, // 폰트 편차라는 건데 그냥 0.0으로 설정하면 됨
		0.5f, // 폰트의 돌출인데 값이 커질수록 앞으로 튀어나옴...
		&g_pMesh, nullptr, nullptr);
	DXERR_HANDLER(g_DXResult);

	// 이전 폰트로 복구하고 폰트 및 DC를 제거합니다.
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

	// 현재 적용된 행렬이 회전행렬뿐이라 이렇게 가져옵니다.
	// 원래는 다른 방식으로 회전행렬을 가져옵니다.
	D3DXMATRIXA16 matRot;
	D3DDEVICE9->GetTransform(D3DTS_WORLD, &matRot);

	// 현재 오일러각 회전을 이용 중인데 일반적으로 아는 개념인
	// 크기 * 자전 * 이동이 아니라 크기 * 이동 * 공전으로 적용해야 합니다.
	D3DXMATRIXA16 matTextWorld;
	matTextWorld = g_matTextScale * g_matTextTrans * matRot;
	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matTextWorld);
	
	if (g_pMesh != nullptr)
	{
		// 텍스트가 메시로 만들어졌으니 DrawSubset()으로 렌더링합니다.
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
	RXLOG("%s(%s)", pData->elfFullName, pData->elfScript); // 폰트 이름(소속)
	return -1;
}

#pragma region 중복되는 이전 코드
void DefaultMatrix()
{
	// =====================================================================
	// 월드행렬을 설정합니다.
	D3DXMatrixScaling(&g_matTextScale, 0.8f, 0.8f, 0.8f);
	D3DXMatrixTranslation(&g_matTextTrans, -2.0f, 0.0f, 0.0f);
	// =====================================================================
	// 뷰행렬을 설정합니다.
	D3DXVECTOR3 vEye(1.0f, 4.0f, -3.0f);   // 카메라의 위치
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // 카메라가 보는 지점
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // 카메라의 업 벡터

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	D3DDEVICE9->SetTransform(D3DTS_VIEW, &matView);
	// =====================================================================
	// 투영행렬을 설정합니다.
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
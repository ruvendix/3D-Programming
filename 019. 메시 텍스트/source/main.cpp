#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"
#include "RX\RX3DAxisDX9.h"


// ====================================================================================
// 공용체 선언부입니다.
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
// 전역 변수 선언부입니다.
IDirect3DDevice9* g_pD3DDevice9 = nullptr;
RX::RXMain_DX9*   g_pMainDX     = nullptr;

D3DXMATRIXA16 g_matViewAndProjection; // 미리 계산해둔 뷰행렬 * 투영행렬
D3DXMATRIXA16 g_matProjection;        // 미리 계산해둔 투영행렬

HRESULT g_DXResult = S_OK;

namespace
{
	RX::RX3DAxisDX9 g_axis; // 3D축을 다루기 위한 것
	ID3DXMesh* g_pMesh = nullptr; // 메시 인터페이스
	INT32 g_shapeType;

	// 원래는 벡터로만 사용되는데 이번에는 FLOAT 3개를 묶은 것으로 봅니다.
	D3DXVECTOR3 g_roateAngle;

	// 미리 계산된 행렬입니다.
	D3DXMATRIXA16 g_matTextScale;
	D3DXMATRIXA16 g_matTextTrans;
}

HDC g_hMainDC = nullptr;

// ====================================================================================
// 함수 선언부입니다.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

// 기본 행렬을 설정합니다.
void DefaultMatrix();

// 기본 조명을 설정합니다.
void DefaultLight();

// 기본 렌더 스테이트를 설정합니다.
void DefaultRenderState();

// 사용자의 키보드 또는 마우스 입력에 따른 처리를 합니다.
void OnUserInput();

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

	g_pMainDX = RXNew RX::RXMain_DX9;
	NULLCHK(g_pMainDX);

	g_pMainDX->setSubFunc(OnInit,    SUBFUNC_TYPE::INIT);
	g_pMainDX->setSubFunc(OnUpdate,  SUBFUNC_TYPE::UPDATE);
	g_pMainDX->setSubFunc(OnRender,  SUBFUNC_TYPE::RENDER);
	g_pMainDX->setSubFunc(OnRelease, SUBFUNC_TYPE::RELEASE);

	// 메모리 할당 순서를 이용해서 메모리 누수를 찾습니다.
	// 평소에는 주석 처리하면 됩니다.
	//_CrtSetBreakAlloc(451);

	g_pMainDX->RunMainRoutine(hInstance, IDI_RUVENDIX_ICO);

	INT32 messageCode = g_pMainDX->getMessageCode();
	SAFE_DELTE(g_pMainDX);
	return messageCode;
}

// 초기화 함수입니다.
// 3D 렌더링은 연산이 많이 들어가므로 웬만한 작업은 초기화해줘야 합니다.
// 렌더링하면서 실시간으로 연산도 가능하지만 그렇게 되면 프레임이 떨어지게 됩니다.
// 일반적으로 렌더링할 때는 렌더링 작업만 처리합니다.
HRESULT CALLBACK OnInit()
{
	g_pD3DDevice9 = RX::RXRendererDX9::Instance()->getD3DDevice9();
	NULLCHK(g_pD3DDevice9);

	DefaultMatrix();
	DefaultLight();
	DefaultRenderState();

	// 축을 생성합니다.
	g_axis.CreateAxis(20.0f);

	// 마우스 커서를 보여줍니다.
	RX::ShowMouseCursor(true);

	// 폰트 테스트
	IDirect3DSurface9* pBackBufferSurface = nullptr;
	g_pD3DDevice9->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBufferSurface);
	NULLCHK(pBackBufferSurface);

	HDC hBackBufferDC = nullptr;
	pBackBufferSurface->GetDC(&hBackBufferDC);


	LOGFONT logicalFont; // LOOGFONT는 Logical Font의 약자입니다.
	::ZeroMemory(&logicalFont, sizeof(logicalFont));

	// 시스템 로케일에 맞춰서 문자 집합을 설정해줍니다.
	logicalFont.lfCharSet = DEFAULT_CHARSET;

	// 폰트를 설정합니다. "C:\Windows\Fonts\"에 있는 폰트만 설정 가능합니다.
	// 즉, 자신의 컴퓨터에는 폰트가 있어도 다른 컴퓨터에는 없을 수 있습니다.
	// 프로그램을 배포할 때는 폰트도 고려해야 할 대상임을 기억해두세요.
	// _TRUNCATE은 -1을 부호 없는 정수로 표현한 값인데 42억 정도 됩니다.
	// 즉, 복사할 길이에 제한을 두지 않겠다는 의미입니다.
	wcsncpy_s(logicalFont.lfFaceName, _countof(logicalFont.lfFaceName), L"굴림", _TRUNCATE);

	// Win32 API를 이용해서 폰트를 생성합니다.
	// 텍스트를 메시로 생성하기 위한 과정입니다.
	HFONT hFont = ::CreateFontIndirect(&logicalFont);

	// 백버퍼의 DC 정보를 복사해서 새로운 DC를 만들고
	// 생성한 DC에 폰트 핸들을 적용합니다.
	HDC hDC = ::CreateCompatibleDC(hBackBufferDC);
	HFONT hOldFont = static_cast<HFONT>(::SelectObject(hDC, hFont));

	g_DXResult = D3DXCreateText(
		g_pD3DDevice9, // 가상 디바이스 객체
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

// 업데이트 함수입니다.
// 렌더링에 영향을 주거나 프로그램에 영향을 주는
// 정보들을 매프레임마다 업데이트합니다.
HRESULT CALLBACK OnUpdate()
{
	OnUserInput();
	return S_OK;
}

// 렌더링 함수입니다.
// 실제 렌더링 작업인 Draw Call이 처리됩니다.
// Draw Call은 프레임당 호출되는 렌더링 함수를 뜻하는데 호출되는 빈도수를
// 조사하면 Draw Call Count를 알아낼 수 있습니다.
HRESULT CALLBACK OnRender()
{
	g_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_axis.DrawAxis();
	g_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, TRUE);

	// 현재 적용된 행렬이 회전행렬뿐이라 이렇게 가져옵니다.
	// 원래는 다른 방식으로 회전행렬을 가져옵니다.
	D3DXMATRIXA16 matRot;
	g_pD3DDevice9->GetTransform(D3DTS_WORLD, &matRot);

	// 현재 오일러각 회전을 이용 중인데 일반적으로 아는 개념인
	// 크기 * 자전 * 이동이 아니라 크기 * 공전 * 이동으로 적용해야 합니다.
	D3DXMATRIXA16 matTextWorld;
	matTextWorld = g_matTextScale * g_matTextTrans * matRot;
	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &matTextWorld);
	
	if (g_pMesh != nullptr)
	{
		// 텍스트가 메시로 만들어졌으니 DrawSubset()으로 렌더링합니다.
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
	g_pD3DDevice9->SetTransform(D3DTS_VIEW, &matView);
	
	// =====================================================================
	// 투영행렬을 설정합니다.
	D3DXMatrixPerspectiveFovLH(&g_matProjection, (D3DX_PI / 4.0f),
		(static_cast<FLOAT>(g_pMainDX->getClientWidth()) / (g_pMainDX->getClientHeight())),
		1.0f, 1000.0f);
	g_pD3DDevice9->SetTransform(D3DTS_PROJECTION, &g_matProjection);
	
	// =====================================================================
	// 전역행렬 초기화입니다.
	g_matViewAndProjection = matView * g_matProjection;
}

void DefaultLight()
{
	// 조명을 생성하고 등록합니다.
	// 이번에는 방향 광원을 이용합니다.
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	
	D3DXVECTOR3 vDir = { -1.0f, -1.0f, 0.2f };
	D3DXVec3Normalize(&vDir, &vDir);
	light.Direction = vDir;

	light.Ambient  = D3DXCOLOR(DXCOLOR_WHITE);
	light.Diffuse  = D3DXCOLOR(DXCOLOR_WHITE);
	light.Specular = D3DXCOLOR(DXCOLOR_WHITE);

	// 광원을 등록하고 활성화시킵니다.
	g_pD3DDevice9->SetLight(0, &light);
	g_pD3DDevice9->LightEnable(0, TRUE);

	// 재질은 하나만 등록 가능합니다.
	// 따라서 재질을 자주 바꿔가며 렌더링할 때가 많습니다.
	D3DXMATERIAL mtrl;
	::ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.MatD3D.Ambient  = D3DXCOLOR(DXCOLOR_BLACK);
	mtrl.MatD3D.Diffuse  = D3DXCOLOR(DXCOLOR_BLUE);
	
	// 반사광 설정입니다.
	// 반사광에는 Power가 있는데 이게 높으면 반사 범위가 줄어들고
	// 이게 낮으면 반사 범위가 넓어집니다.
	// 반사광을 사용하려면 렌더 스테이트에서 반사광을 활성해야 합니다.
	mtrl.MatD3D.Specular = D3DXCOLOR(DXCOLOR_WHITE);
	mtrl.MatD3D.Power    = 20.0f;

	// 방사광 설정입니다.
	// 방사광은 낮게 설정해야 합니다.
	mtrl.MatD3D.Emissive = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);

	g_pD3DDevice9->SetMaterial(&(mtrl.MatD3D));
}

void DefaultRenderState()
{
	// rhw를 사용하지 않는다면 변환 이전의 공간좌표를
	// 사용하게 되므로 각종 변환 과정을 거쳐야 합니다.
	// 조명(라이팅, Lighting)도 그중 하나인데
	// 이번에는 조명을 사용하므로 조명을 켜줍니다.
	g_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, TRUE);

	// 필 모드를 설정합니다. 디폴트는 솔리드입니다.
	g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	//g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);

	// 컬링 모드를 설정합니다. 디폴트는 반시계방향 컬링입니다.
	// 큐브를 확인하기 위해서는 컬링 모드를 무시해야 합니다.
	g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	//g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// 법선벡터를 자동으로 계산해주는 설정입니다.
	// 단! 이 설정을 이용하게 되면 사양을 좀 탑니다...
	// 디폴트는 FALSE입니다.
	g_pD3DDevice9->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// 반사광을 활성시킵니다.
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

	// 각도 보정
	g_roateAngle.z = RX::AdjustAngle(g_roateAngle.z);
	g_roateAngle.x = RX::AdjustAngle(g_roateAngle.x);
	g_roateAngle.y = RX::AdjustAngle(g_roateAngle.y);

	// 회전행렬입니다. 순서는 Z -> X -> Y입니다.
	// 즉, Roll -> Pitch -> Yaw입니다.
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot,
		D3DXToRadian(g_roateAngle.y),
		D3DXToRadian(g_roateAngle.x),
		D3DXToRadian(g_roateAngle.z));

	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &matRot);
}
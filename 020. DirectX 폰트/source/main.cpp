#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"
#include "RX\RX3DAxisDX9.h"


// ====================================================================================
// 공용체 선언부입니다.
enum class TEXT_RENDERING_FLAG : INT32
{
	TOP,
	LEFT,
	CENTER,
	RIGHT,
	VCENTER,
	BOTTOM,
	WORDBREAK,
	SINGLELINE,
	EXPANDTABS,
	TABSTOP,
	NOCLIP,
	EXTERNALLEADING,
	CALCRECT,
	NOPREFIX,
	INTERNAL,
	EDITCONTROL,
	PATH_ELLIPSIS,
	END_ELLIPSIS,
	MODIFYSTRING,
	RTLREADING,
	WORD_ELLIPSIS,
	NOFULLWIDTHCHARBREAK,
	HIDEPREFIX,
	PREFIXONLY,
	END,
};


// ====================================================================================
// 전역 변수 선언부입니다.
HRESULT g_DXResult = S_OK;

namespace
{
	RX::RX3DAxisDX9 g_axis; // 3D축을 다루기 위한 것
	
	// DirectX에서 사용되는 폰트 인터페이스 포인터입니다.
	// Win32 API보다는 좀 더 간단하지만 제대로 사용하려면 
	// ID3DXSprite와 연동해서 사용해야 합니다.
	ID3DXFont* g_pFont = nullptr;
	
	D3DXVECTOR3 g_rotateAngle;

	INT32        g_textRenderingFlagIdx = 0;
	INT32        g_textRenderingFlag    = 0;
	const TCHAR* g_szTextRenderingFlag  = CONVERT_FLAG_TO_STRING(DT_TOP);
}


// ====================================================================================
// 함수 선언부입니다.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

void DefaultMatrix();
void DefaultRenderState();
void OnUserInput();

// 텍스트 렌더링 플래그를 문자열로 바꾸는 함수입니다.
const TCHAR* ConvertTextRenderingFlagToString(INT32 flagIdx);

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
	DefaultRenderState();

	g_axis.CreateAxis(20.0f);

	RX::ShowMouseCursor(true);

	// ======================================================================
	// 폰트 정보를 채웁니다.
	D3DXFONT_DESC logicalFont;
	::ZeroMemory(&logicalFont, sizeof(logicalFont));

	logicalFont.Height    = 80; // 높이
	logicalFont.MipLevels = 1;  // 밉레벨 사용하지 않음, 0이면 밉레벨 생성함...
	logicalFont.Italic    = FALSE; // 이탤릭체 사용하지 않음
	logicalFont.CharSet   = DEFAULT_CHARSET; // 시스템 로케일에 따른 문자 집합 사용
	logicalFont.OutputPrecision = OUT_DEFAULT_PRECIS; // 기본 정확도
	logicalFont.Quality         = DEFAULT_QUALITY;    // 기본 품질
	logicalFont.PitchAndFamily  = DEFAULT_PITCH | FW_DONTCARE; // 기본 간격 및 패밀리 신경 쓰지 않음

	// 폰트를 설정하는 부분입니다. 전에 했던 대로 폰트 목록을 먼저 조사하고 설정해도 됩니다.
	wcsncpy_s(logicalFont.FaceName, _countof(logicalFont.FaceName), L"굴림", _TRUNCATE);
	
	// ======================================================================
	// 폰트 객체를 생성합니다.
	// 위에서 설정한 폰트 정보를 이용해서 생성합니다.
	g_DXResult = D3DXCreateFontIndirect(
		D3DDEVICE9, // 가상 디바이스 객체
		&logicalFont,  // D3DXFONT_DESC 포인터
		&g_pFont);     // ID3DXFont 인터페이스 포인터
	DXERR_HANDLER(g_DXResult);

	// 이것과 동일한데 이렇게 하면 모든 정보를 넣어야 합니다.
	// 하지만 폰트 정보를 먼저 설정해놓으면 필요 없는 정보를 생략할 수 있습니다.
	// 이번에는 예제라서 거의 모든 정보를 보여드렸습니다.
	//D3DXCreateFont(D3DDEVICE9, 80, 0, 0, 1, FALSE, DEFAULT_CHARSET,
	//	OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FW_DONTCARE, L"굴림", &g_pFont);

	return S_OK;
}

HRESULT CALLBACK OnUpdate()
{
	OnUserInput();
	return S_OK;
}

HRESULT CALLBACK OnRender()
{
	g_axis.DrawAxis();

	RECT rtClient;
	RXMAIN_DX9->getClientRect()->ConvertToWinAPIRect(&rtClient);

	// DirectX 폰트 객체를 이용해서 2D 텍스트를 렌더링합니다.
	g_pFont->DrawText(
		nullptr, // ID3DXSprite 인터페이스 포인터, nullptr로 설정하면 내부 객체 이용(느림)
		g_szTextRenderingFlag, // 렌더링할 텍스트
		_TRUNCATE, // 텍스트의 길이(_TRUNCATE는 -1인데 텍스트가 널문자 만날 때까지만 렌더링)
		&rtClient, // 렌더링 영역인데 nullptr로 설정하면 클라이언트 영역(0, 0)
		g_textRenderingFlag, // 렌더링 플래그(OR 연산자로 여러 플래그 이용 가능한데 무시되는 것도 있음)
		DXCOLOR_WHITE); // 렌더링할 텍스트의 색상

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	g_axis.Release();
	SAFE_RELEASE(g_pFont);
	return S_OK;
}

void DefaultMatrix()
{
	// ==========================================================
	// 뷰행렬을 설정합니다.
	D3DXVECTOR3 vEye(1.0f, 4.0f, -3.0f);   // 카메라의 위치
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // 카메라가 보는 지점
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // 카메라의 업 벡터

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	D3DDEVICE9->SetTransform(D3DTS_VIEW, &matView);	
	// ==========================================================
	// 투영행렬을 설정합니다.
	D3DXMATRIXA16 matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection, (D3DX_PI / 4.0f),
		(static_cast<FLOAT>(RXMAIN_DX9->getClientRect()->CalcWidth()) /
		                   (RXMAIN_DX9->getClientRect()->CalcHeight())), 1.0f, 1000.0f);
	D3DDEVICE9->SetTransform(D3DTS_PROJECTION, &matProjection);
	// ==========================================================
}

void DefaultRenderState()
{
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, FALSE);
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
		++g_textRenderingFlagIdx;
		g_szTextRenderingFlag = ConvertTextRenderingFlagToString(g_textRenderingFlagIdx);
		if (g_textRenderingFlagIdx >= static_cast<INT32>(TEXT_RENDERING_FLAG::END))
		{
			g_textRenderingFlag    = DT_TOP;
			g_textRenderingFlagIdx = 0;
			g_szTextRenderingFlag  = CONVERT_FLAG_TO_STRING(DT_TOP);
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

const TCHAR* ConvertTextRenderingFlagToString(INT32 flagIdx)
{
	TEXT_RENDERING_FLAG value = static_cast<TEXT_RENDERING_FLAG>(flagIdx);
	switch (value)
	{
	case TEXT_RENDERING_FLAG::TOP:
	{
		g_textRenderingFlag = DT_TOP;
		return CONVERT_FLAG_TO_STRING(DT_TOP);
	}
	case TEXT_RENDERING_FLAG::LEFT:
	{
		g_textRenderingFlag = DT_LEFT;
		return CONVERT_FLAG_TO_STRING(DT_LEFT);
	}
	case TEXT_RENDERING_FLAG::CENTER:
	{
		g_textRenderingFlag = DT_CENTER;
		return CONVERT_FLAG_TO_STRING(DT_CENTER);
	}
	case TEXT_RENDERING_FLAG::RIGHT:
	{
		g_textRenderingFlag = DT_RIGHT;
		return CONVERT_FLAG_TO_STRING(DT_RIGHT);
	}
	case TEXT_RENDERING_FLAG::VCENTER:
	{
		g_textRenderingFlag = DT_VCENTER;
		return CONVERT_FLAG_TO_STRING(DT_VCENTER);
	}
	case TEXT_RENDERING_FLAG::BOTTOM:
	{
		g_textRenderingFlag = DT_BOTTOM;
		return CONVERT_FLAG_TO_STRING(DT_BOTTOM);
	}
	case TEXT_RENDERING_FLAG::WORDBREAK:
	{
		g_textRenderingFlag = DT_WORDBREAK;
		return CONVERT_FLAG_TO_STRING(DT_WORDBREAK);
	}
	case TEXT_RENDERING_FLAG::SINGLELINE:
	{
		g_textRenderingFlag = DT_SINGLELINE;
		return CONVERT_FLAG_TO_STRING(DT_SINGLELINE);
	}
	case TEXT_RENDERING_FLAG::EXPANDTABS:
	{
		g_textRenderingFlag = DT_EXPANDTABS;
		return CONVERT_FLAG_TO_STRING(DT_EXPANDTABS);
	}
	case TEXT_RENDERING_FLAG::TABSTOP:
	{
		g_textRenderingFlag = DT_TABSTOP;
		return CONVERT_FLAG_TO_STRING(DT_TABSTOP);
	}
	case TEXT_RENDERING_FLAG::NOCLIP:
	{
		g_textRenderingFlag = DT_NOCLIP;
		return CONVERT_FLAG_TO_STRING(DT_NOCLIP);
	}
	case TEXT_RENDERING_FLAG::EXTERNALLEADING:
	{
		g_textRenderingFlag = DT_EXTERNALLEADING;
		return CONVERT_FLAG_TO_STRING(DT_EXTERNALLEADING);
	}
	case TEXT_RENDERING_FLAG::CALCRECT:
	{
		g_textRenderingFlag = DT_CALCRECT;
		return CONVERT_FLAG_TO_STRING(DT_CALCRECT);
	}
	case TEXT_RENDERING_FLAG::NOPREFIX:
	{
		g_textRenderingFlag = DT_NOPREFIX;
		return CONVERT_FLAG_TO_STRING(DT_NOPREFIX);
	}
	case TEXT_RENDERING_FLAG::INTERNAL:
	{
		g_textRenderingFlag = DT_INTERNAL;
		return CONVERT_FLAG_TO_STRING(DT_INTERNAL);
	}
	case TEXT_RENDERING_FLAG::EDITCONTROL:
	{
		g_textRenderingFlag = DT_EDITCONTROL;
		return CONVERT_FLAG_TO_STRING(DT_EDITCONTROL);
	}
	case TEXT_RENDERING_FLAG::PATH_ELLIPSIS:
	{
		g_textRenderingFlag = DT_PATH_ELLIPSIS;
		return CONVERT_FLAG_TO_STRING(DT_PATH_ELLIPSIS);
	}
	case TEXT_RENDERING_FLAG::END_ELLIPSIS:
	{
		g_textRenderingFlag = DT_END_ELLIPSIS;
		return CONVERT_FLAG_TO_STRING(DT_END_ELLIPSIS);
	}
	case TEXT_RENDERING_FLAG::MODIFYSTRING:
	{
		g_textRenderingFlag = DT_MODIFYSTRING;
		return CONVERT_FLAG_TO_STRING(DT_MODIFYSTRING);
	}
	case TEXT_RENDERING_FLAG::RTLREADING:
	{
		g_textRenderingFlag = DT_RTLREADING;
		return CONVERT_FLAG_TO_STRING(DT_RTLREADING);
	}
	case TEXT_RENDERING_FLAG::WORD_ELLIPSIS:
	{
		g_textRenderingFlag = DT_WORD_ELLIPSIS;
		return CONVERT_FLAG_TO_STRING(DT_WORD_ELLIPSIS);
	}
	case TEXT_RENDERING_FLAG::NOFULLWIDTHCHARBREAK:
	{
		g_textRenderingFlag = DT_NOFULLWIDTHCHARBREAK;
		return CONVERT_FLAG_TO_STRING(DT_NOFULLWIDTHCHARBREAK);
	}
	case TEXT_RENDERING_FLAG::HIDEPREFIX:
	{
		g_textRenderingFlag = DT_HIDEPREFIX;
		return CONVERT_FLAG_TO_STRING(DT_HIDEPREFIX);
	}
	case TEXT_RENDERING_FLAG::PREFIXONLY:
	{
		g_textRenderingFlag = DT_PREFIXONLY;
		return CONVERT_FLAG_TO_STRING(DT_PREFIXONLY);
	}
	}

	return SZ_NULL;
}

#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"
#include "RX\RX3DAxisDX9.h"


// ====================================================================================
// ����ü ������Դϴ�.
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
// ���� ���� ������Դϴ�.
HRESULT g_DXResult = S_OK;

namespace
{
	RX::RX3DAxisDX9 g_axis; // 3D���� �ٷ�� ���� ��
	
	// DirectX���� ���Ǵ� ��Ʈ �������̽� �������Դϴ�.
	// Win32 API���ٴ� �� �� ���������� ����� ����Ϸ��� 
	// ID3DXSprite�� �����ؼ� ����ؾ� �մϴ�.
	ID3DXFont* g_pFont = nullptr;
	
	D3DXVECTOR3 g_rotateAngle;

	INT32        g_textRenderingFlagIdx = 0;
	INT32        g_textRenderingFlag    = 0;
	const TCHAR* g_szTextRenderingFlag  = CONVERT_FLAG_TO_STRING(DT_TOP);
}


// ====================================================================================
// �Լ� ������Դϴ�.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

void DefaultMatrix();
void DefaultRenderState();
void OnUserInput();

// �ؽ�Ʈ ������ �÷��׸� ���ڿ��� �ٲٴ� �Լ��Դϴ�.
const TCHAR* ConvertTextRenderingFlagToString(INT32 flagIdx);

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
	DefaultRenderState();

	g_axis.CreateAxis(20.0f);

	RX::ShowMouseCursor(true);

	// ======================================================================
	// ��Ʈ ������ ä��ϴ�.
	D3DXFONT_DESC logicalFont;
	::ZeroMemory(&logicalFont, sizeof(logicalFont));

	logicalFont.Height    = 80; // ����
	logicalFont.MipLevels = 1;  // �ӷ��� ������� ����, 0�̸� �ӷ��� ������...
	logicalFont.Italic    = FALSE; // ���Ÿ�ü ������� ����
	logicalFont.CharSet   = DEFAULT_CHARSET; // �ý��� �����Ͽ� ���� ���� ���� ���
	logicalFont.OutputPrecision = OUT_DEFAULT_PRECIS; // �⺻ ��Ȯ��
	logicalFont.Quality         = DEFAULT_QUALITY;    // �⺻ ǰ��
	logicalFont.PitchAndFamily  = DEFAULT_PITCH | FW_DONTCARE; // �⺻ ���� �� �йи� �Ű� ���� ����

	// ��Ʈ�� �����ϴ� �κ��Դϴ�. ���� �ߴ� ��� ��Ʈ ����� ���� �����ϰ� �����ص� �˴ϴ�.
	wcsncpy_s(logicalFont.FaceName, _countof(logicalFont.FaceName), L"����", _TRUNCATE);
	
	// ======================================================================
	// ��Ʈ ��ü�� �����մϴ�.
	// ������ ������ ��Ʈ ������ �̿��ؼ� �����մϴ�.
	g_DXResult = D3DXCreateFontIndirect(
		D3DDEVICE9, // ���� ����̽� ��ü
		&logicalFont,  // D3DXFONT_DESC ������
		&g_pFont);     // ID3DXFont �������̽� ������
	DXERR_HANDLER(g_DXResult);

	// �̰Ͱ� �����ѵ� �̷��� �ϸ� ��� ������ �־�� �մϴ�.
	// ������ ��Ʈ ������ ���� �����س����� �ʿ� ���� ������ ������ �� �ֽ��ϴ�.
	// �̹����� ������ ���� ��� ������ ������Ƚ��ϴ�.
	//D3DXCreateFont(D3DDEVICE9, 80, 0, 0, 1, FALSE, DEFAULT_CHARSET,
	//	OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FW_DONTCARE, L"����", &g_pFont);

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

	// DirectX ��Ʈ ��ü�� �̿��ؼ� 2D �ؽ�Ʈ�� �������մϴ�.
	g_pFont->DrawText(
		nullptr, // ID3DXSprite �������̽� ������, nullptr�� �����ϸ� ���� ��ü �̿�(����)
		g_szTextRenderingFlag, // �������� �ؽ�Ʈ
		_TRUNCATE, // �ؽ�Ʈ�� ����(_TRUNCATE�� -1�ε� �ؽ�Ʈ�� �ι��� ���� �������� ������)
		&rtClient, // ������ �����ε� nullptr�� �����ϸ� Ŭ���̾�Ʈ ����(0, 0)
		g_textRenderingFlag, // ������ �÷���(OR �����ڷ� ���� �÷��� �̿� �����ѵ� ���õǴ� �͵� ����)
		DXCOLOR_WHITE); // �������� �ؽ�Ʈ�� ����

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
	// ������� �����մϴ�.
	D3DXVECTOR3 vEye(1.0f, 4.0f, -3.0f);   // ī�޶��� ��ġ
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // ī�޶� ���� ����
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // ī�޶��� �� ����

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	D3DDEVICE9->SetTransform(D3DTS_VIEW, &matView);	
	// ==========================================================
	// ��������� �����մϴ�.
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

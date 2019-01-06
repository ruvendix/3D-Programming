#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


// ====================================================================================
// ����ü ������Դϴ�.
enum BLEND_MODE : INT32
{
	ZERO,
    ONE,
    SRCCOLOR,
    INVSRCCOLOR,
    SRCALPHA,
    INVSRCALPHA,
    DESTALPHA,
    INVDESTALPHA,
    DESTCOLOR,
    INVDESTCOLOR,
	END,
};

// ====================================================================================
// ���� ���� ������Դϴ�.
HRESULT g_DXResult = S_OK;

namespace
{
	IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
	IDirect3DIndexBuffer9*  g_pIndexBuffer  = nullptr;

	std::vector<VertexP3D>  g_vecP3D;
	std::vector<Index16>    g_vecIndex16;

	D3DXVECTOR3 g_rotateAngle;

	ExampleRenderInfo g_exampleRenderInfoSrc;
	ExampleRenderInfo g_exampleRenderInfoDest;
}


// ====================================================================================
// �Լ� ������Դϴ�.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

void DefaultMatrix();
void DefaultRenderState();
void InitVertexBuffer();
void InitIndexBuffer();
void OnUserInput();

// ���� ��� ������ �������ݴϴ�.
const TCHAR* UpdateBlendMode(ExampleRenderInfo* pInfo);


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

	RX::ShowMouseCursor(true);

	InitVertexBuffer();
	InitIndexBuffer();

	g_exampleRenderInfoSrc.idx     = 2;
	g_exampleRenderInfoSrc.value   = D3DBLEND_SRCALPHA;
	g_exampleRenderInfoSrc.szValue = CONVERT_FLAG_TO_STRING(D3DBLEND_SRCALPHA);
	
	g_exampleRenderInfoDest.idx     = 3;
	g_exampleRenderInfoDest.value   = D3DBLEND_INVSRCALPHA;
	g_exampleRenderInfoDest.szValue = CONVERT_FLAG_TO_STRING(D3DBLEND_INVSRCALPHA);

	return S_OK;
}

HRESULT CALLBACK OnUpdate()
{
	OnUserInput();
	return S_OK;
}

HRESULT CALLBACK OnRender()
{
	D3DDEVICE9->SetFVF(VertexP3D::format);
	D3DDEVICE9->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(VertexP3D));
	D3DDEVICE9->SetIndices(g_pIndexBuffer);
	D3DDEVICE9->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 6, 0, 2);

	TCHAR szText[DEFAULT_STRING_LENGTH] = _T("���ۿ� ��� ����(T) ������ ��� ����(Y)");
	swprintf_s(szText,L"%s\n���ۿ� ���(%s)\n������ ���(%s)", szText,
		g_exampleRenderInfoSrc.szValue, g_exampleRenderInfoDest.szValue);
	RX::RXRendererDX9::Instance()->DrawTextOriginClientArea(szText, DT_LEFT, DXCOLOR_WHITE);

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	SAFE_RELEASE(g_pIndexBuffer);
	SAFE_RELEASE(g_pVertexBuffer);
	return S_OK;
}

void DefaultRenderState()
{
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, FALSE);

	// ���ĺ����� �����մϴ�. ����Ʈ�� FALSE�Դϴ�.
	// ���ĺ����� �����ϸ� ��Ȱ���ϱ� ������ ��� �������� ����˴ϴ�.
	// ���� ������ ������ �����ϴ�.
	//
	// ���� �ȼ� = (���ۿ� �ȼ�) * (���ۿ� ���� ���) ���� ���� (������ �ȼ�) * (������ ���� ���))
	// �Ϲ������� ���ĺ����� ���ۿ��� ���� ����� ���ۿ��� ���ĸ�,
	// �������� ���� ����� ���ۿ��� ���� ������ �̿��մϴ�.
	// �׷��� �� �ȼ��� �����ϸ� 1.0f�� ������ �����Դϴ�.
	D3DDEVICE9->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	D3DDEVICE9->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD); // ���� �̰͸� �����
	D3DDEVICE9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA); // ���ۿ��� ����
	D3DDEVICE9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA); // 1 - ���ۿ��� ����
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

	if (::GetAsyncKeyState('T') & 0x0001)
	{
		++g_exampleRenderInfoSrc.idx;
		g_exampleRenderInfoSrc.szValue = UpdateBlendMode(&g_exampleRenderInfoSrc);
		if (g_exampleRenderInfoSrc.idx >= static_cast<INT32>(BLEND_MODE::END))
		{
			g_exampleRenderInfoSrc.idx     = 2;
			g_exampleRenderInfoSrc.value   = D3DBLEND_SRCALPHA;
			g_exampleRenderInfoSrc.szValue = CONVERT_FLAG_TO_STRING(D3DBLEND_SRCALPHA);
		}

		D3DDEVICE9->SetRenderState(D3DRS_SRCBLEND, g_exampleRenderInfoSrc.value);
	}

	if (::GetAsyncKeyState('Y') & 0x0001)
	{
		++g_exampleRenderInfoDest.idx;
		g_exampleRenderInfoDest.szValue = UpdateBlendMode(&g_exampleRenderInfoDest);
		if (g_exampleRenderInfoDest.idx >= static_cast<INT32>(BLEND_MODE::END))
		{
			g_exampleRenderInfoDest.idx     = 3;
			g_exampleRenderInfoDest.value   = D3DBLEND_INVSRCALPHA;
			g_exampleRenderInfoDest.szValue = CONVERT_FLAG_TO_STRING(D3DBLEND_INVSRCALPHA);
		}

		D3DDEVICE9->SetRenderState(D3DRS_DESTBLEND, g_exampleRenderInfoDest.value);
	}

	g_rotateAngle.z = RX::AdjustAngle(g_rotateAngle.z);
	g_rotateAngle.x = RX::AdjustAngle(g_rotateAngle.x);

	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot,
		D3DXToRadian(g_rotateAngle.y),
		D3DXToRadian(g_rotateAngle.x),
		D3DXToRadian(g_rotateAngle.z));

	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matRot);
}

const TCHAR* UpdateBlendMode(ExampleRenderInfo* pInfo)
{
	NULLCHK(pInfo);
	BLEND_MODE value = static_cast<BLEND_MODE>(pInfo->idx);
	switch (value)
	{
	case BLEND_MODE::ZERO:
	{
		pInfo->value = D3DBLEND_ZERO;
		return CONVERT_FLAG_TO_STRING(D3DBLEND_ZERO);
	}
	case BLEND_MODE::ONE:
	{
		pInfo->value = D3DBLEND_ONE;
		return CONVERT_FLAG_TO_STRING(D3DBLEND_ONE);
	}
	case BLEND_MODE::SRCCOLOR:
	{
		pInfo->value = D3DBLEND_SRCCOLOR;
		return CONVERT_FLAG_TO_STRING(D3DBLEND_SRCCOLOR);
	}
	case BLEND_MODE::INVSRCCOLOR:
	{
		pInfo->value = D3DBLEND_INVSRCCOLOR;
		return CONVERT_FLAG_TO_STRING(D3DBLEND_INVSRCCOLOR);
	}
	case BLEND_MODE::SRCALPHA:
	{
		pInfo->value = D3DBLEND_ONE;
		return CONVERT_FLAG_TO_STRING(D3DBLEND_SRCALPHA);
	}
	case BLEND_MODE::INVSRCALPHA:
	{
		pInfo->value = D3DBLEND_INVSRCALPHA;
		return CONVERT_FLAG_TO_STRING(D3DBLEND_INVSRCALPHA);
	}
	case BLEND_MODE::DESTALPHA:
	{
		pInfo->value = D3DBLEND_DESTALPHA;
		return CONVERT_FLAG_TO_STRING(D3DBLEND_DESTALPHA);
	}
	case BLEND_MODE::INVDESTALPHA:
	{
		pInfo->value = D3DBLEND_INVDESTALPHA;
		return CONVERT_FLAG_TO_STRING(D3DBLEND_INVDESTALPHA);
	}
	case BLEND_MODE::DESTCOLOR:
	{
		pInfo->value = D3DBLEND_DESTCOLOR;
		return CONVERT_FLAG_TO_STRING(D3DBLEND_DESTCOLOR);
	}
	case BLEND_MODE::INVDESTCOLOR:
	{
		pInfo->value = D3DBLEND_INVDESTCOLOR;
		return CONVERT_FLAG_TO_STRING(D3DBLEND_INVDESTCOLOR);
	}
	}

	return SZ_NULL;
}

#pragma region �ߺ��Ǵ� ���� �ڵ�
void DefaultMatrix()
{
	// ==========================================================
	// ������� �����մϴ�.
	D3DXVECTOR3 vEye(0.0f, 0.0f, -4.0f);   // ī�޶��� ��ġ
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

void InitVertexBuffer()
{
	VertexP3D vertex;
	::ZeroMemory(&vertex, sizeof(vertex));
	vertex.dwColor = D3DCOLOR_ARGB(100, 255, 0, 0);

	vertex.vPos.x  = -1.0f;
	vertex.vPos.y  = 1.0f;
	g_vecP3D.push_back(vertex);

	vertex.vPos.x = 1.0f;
	vertex.vPos.y = 1.0f;
	g_vecP3D.push_back(vertex);

	vertex.vPos.x = -1.0f;
	vertex.vPos.y = -1.0f;
	g_vecP3D.push_back(vertex);

	vertex.vPos.x = 1.0f;
	vertex.vPos.y = -1.0f;
	g_vecP3D.push_back(vertex);

	INT32 verticesSize = g_vecP3D.size() * sizeof(vertex);
	g_DXResult = D3DDEVICE9->CreateVertexBuffer(verticesSize, D3DUSAGE_WRITEONLY,
		VertexP3D::format, D3DPOOL_MANAGED, &g_pVertexBuffer, nullptr);
	DXERR_HANDLER(g_DXResult);

	void* pData = nullptr;
	g_pVertexBuffer->Lock(0, verticesSize, &pData, D3DLOCK_READONLY);
	::CopyMemory(pData, &g_vecP3D[0], verticesSize);
	g_pVertexBuffer->Unlock();
}

void InitIndexBuffer()
{
	Index16 index;
	::ZeroMemory(&index, sizeof(index));

	index.index = 0;
	g_vecIndex16.push_back(index);

	index.index = 3;
	g_vecIndex16.push_back(index);

	index.index = 2;
	g_vecIndex16.push_back(index);

	index.index = 0;
	g_vecIndex16.push_back(index);

	index.index = 1;
	g_vecIndex16.push_back(index);

	index.index = 3;
	g_vecIndex16.push_back(index);

	INT32 indicesSize = g_vecIndex16.size() * sizeof(index);
	g_DXResult = D3DDEVICE9->CreateIndexBuffer(indicesSize, D3DUSAGE_WRITEONLY,
		Index16::format, D3DPOOL_MANAGED, &g_pIndexBuffer, nullptr);
	DXERR_HANDLER(g_DXResult);

	void* pData = nullptr;
	g_pIndexBuffer->Lock(0, indicesSize, &pData, D3DLOCK_READONLY);
	::CopyMemory(pData, &g_vecIndex16[0], indicesSize);
	g_pIndexBuffer->Unlock();
}
#pragma endregion
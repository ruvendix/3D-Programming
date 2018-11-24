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
IDirect3DDevice9* g_pD3DDevice9 = nullptr;
RX::RXMain_DX9*   g_pMainDX     = nullptr;

HRESULT g_DXResult = S_OK;

IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
IDirect3DIndexBuffer9*  g_pIndexBuffer  = nullptr;
std::vector<VertexP3D>  g_vecP3D;
std::vector<Index16>    g_vecIndex16;

namespace
{
	// ������ ���ͷθ� ���Ǵµ� �̹����� FLOAT 3���� ���� ������ ���ϴ�.
	D3DXVECTOR3 g_rotateAngle;

	// ���� ��¿��Դϴ�.
	ExampleRenderInfo g_exampleRenderInfoSrc;
	ExampleRenderInfo g_exampleRenderInfoDest;
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

// ���� ���ۿ� ���� ������ �����մϴ�.
void InitVertexBuffer();

// �ε��� ���ۿ� �ε��� ������ �����մϴ�.
void InitIndexBuffer();

// ������� Ű���� �Ǵ� ���콺 �Է¿� ���� ó���� �մϴ�.
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

	// ���콺 Ŀ���� �����ݴϴ�.
	RX::ShowMouseCursor(true);

	// ���� ���ۿ� �ε��� ���ۿ� ������ �ֽ��ϴ�.
	InitVertexBuffer();
	InitIndexBuffer();

	// ���� ��¿� ������ �ʱ�ȭ�մϴ�.
	g_exampleRenderInfoSrc.idx     = 2;
	g_exampleRenderInfoSrc.value   = D3DBLEND_SRCALPHA;
	g_exampleRenderInfoSrc.szValue = CONVERT_FLAG_TO_STRING(D3DBLEND_SRCALPHA);
	
	g_exampleRenderInfoDest.idx     = 3;
	g_exampleRenderInfoDest.value   = D3DBLEND_INVSRCALPHA;
	g_exampleRenderInfoDest.szValue = CONVERT_FLAG_TO_STRING(D3DBLEND_INVSRCALPHA);

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
	g_pD3DDevice9->SetFVF(VertexP3D::format);
	g_pD3DDevice9->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(VertexP3D));
	g_pD3DDevice9->SetIndices(g_pIndexBuffer);
	g_pD3DDevice9->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 6, 0, 2);

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

void DefaultMatrix()
{
	// =====================================================================
	// ������� �����մϴ�.
	D3DXVECTOR3 vEye(0.0f, 0.0f, -4.0f);   // ī�޶��� ��ġ
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

	// ���ĺ����� �����մϴ�. ����Ʈ�� FALSE�Դϴ�.
	// ���ĺ����� �����ϸ� ��Ȱ���ϱ� ������ ��� �������� ����˴ϴ�.
	// ���� ������ ������ �����ϴ�.
	// ���� �ȼ� = (���ۿ� �ȼ�)*(���ۿ� ���� ���) ���� ���� (������ �ȼ�)*(������ ���� ���))
	// �Ϲ������� ���ĺ����� ���ۿ��� ���� ����� ���ۿ��� ���ĸ�,
	// �������� ���� ����� ���ۿ��� ���� ������ �̿��մϴ�.
	// �׷��� �� �ȼ��� �����ϸ� 1.0f�� ������ �����Դϴ�.
	g_pD3DDevice9->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pD3DDevice9->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD); // ���� �̰͸� �����
	g_pD3DDevice9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA); // ���ۿ��� ����
	g_pD3DDevice9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA); // ���ۿ��� ���� ����
}

void InitVertexBuffer()
{
	VertexP3D vertex;
	::ZeroMemory(&vertex, sizeof(vertex));

	// ù��° �����Դϴ�.
	vertex.vPos.x  = -1.0f;
	vertex.vPos.y  =  1.0f;
	vertex.dwColor = D3DCOLOR_ARGB(20, 255, 255, 0);
	g_vecP3D.push_back(vertex);

	// �ι�° �����Դϴ�.
	vertex.vPos.x  =  1.0f;
	vertex.vPos.y  =  1.0f;
	vertex.dwColor = D3DCOLOR_ARGB(20, 255, 255, 0);
	g_vecP3D.push_back(vertex);

	// ����° �����Դϴ�.
	vertex.vPos.x = -1.0f;
	vertex.vPos.y = -1.0f;
	vertex.dwColor = D3DCOLOR_ARGB(20, 255, 255, 0);
	g_vecP3D.push_back(vertex);

	// �׹�° �����Դϴ�.
	vertex.vPos.x  =  1.0f;
	vertex.vPos.y  = -1.0f;
	vertex.dwColor = D3DCOLOR_ARGB(20, 255, 255, 0);
	g_vecP3D.push_back(vertex);

	// ���� ���� �����ϱ�
	INT32 verticesSize = g_vecP3D.size() * sizeof(vertex);
	g_DXResult = g_pD3DDevice9->CreateVertexBuffer(verticesSize, D3DUSAGE_WRITEONLY,
		VertexP3D::format, D3DPOOL_MANAGED, &g_pVertexBuffer, nullptr);
	DXERR_HANDLER(g_DXResult);

	// ���� ���ۿ� ���� ���� �ֱ�
	void* pData = nullptr;
	g_pVertexBuffer->Lock(0, verticesSize, &pData, D3DFLAG_NONE);
	::CopyMemory(pData, &g_vecP3D[0], verticesSize);
	g_pVertexBuffer->Unlock();
}

void InitIndexBuffer()
{
	Index16 index;
	::ZeroMemory(&index, sizeof(index));

	// ù��° �ε����Դϴ�.
	index.index = 0;
	g_vecIndex16.push_back(index);

	// �ι�° �ε����Դϴ�.
	index.index = 3;
	g_vecIndex16.push_back(index);

	// ����° �ε����Դϴ�.
	index.index = 2;
	g_vecIndex16.push_back(index);

	// �׹�° �ε����Դϴ�.
	index.index = 0;
	g_vecIndex16.push_back(index);

	// �ټ���° �ε����Դϴ�.
	index.index = 1;
	g_vecIndex16.push_back(index);

	// ������° �ε����Դϴ�.
	index.index = 3;
	g_vecIndex16.push_back(index);

	// �ε��� ���� �����ϱ�
	INT32 indicesSize = g_vecIndex16.size() * sizeof(index);
	g_DXResult = g_pD3DDevice9->CreateIndexBuffer(indicesSize, D3DUSAGE_WRITEONLY,
		Index16::format, D3DPOOL_MANAGED, &g_pIndexBuffer, nullptr);
	DXERR_HANDLER(g_DXResult);

	// �ε��� ���ۿ� �ε��� ���� �ֱ�
	void* pData = nullptr;
	g_pIndexBuffer->Lock(0, indicesSize, &pData, D3DFLAG_NONE);
	::CopyMemory(pData, &g_vecIndex16[0], indicesSize);
	g_pIndexBuffer->Unlock();
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

		g_pD3DDevice9->SetRenderState(D3DRS_SRCBLEND, g_exampleRenderInfoSrc.value);
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

		g_pD3DDevice9->SetRenderState(D3DRS_DESTBLEND, g_exampleRenderInfoDest.value);
	}

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

	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &matRot);
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

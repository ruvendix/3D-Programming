#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


// ====================================================================================
// ����ü ������Դϴ�.
enum class FILTER_TYPE : INT32
{
	NONE,
	POINT,         // ������
	LINEAR,        // ���� 
	ANISOTROPIC,   // ���漺(�̹漺, �̵�漺)
	PYRAMIDALQUAD, // �Ƕ�̵� ����
	GAUSSIANQUAD,  // ����þ� ����
	END,
};


// ====================================================================================
// ���� ���� ������Դϴ�.
HRESULT g_DXResult = S_OK;

IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
IDirect3DIndexBuffer9*  g_pIndexBuffer  = nullptr;
std::vector<VertexP3T1> g_vecP3T1;
std::vector<Index16>    g_vecIndex16;

namespace
{
	// ������ ���ͷθ� ���Ǵµ� �̹����� FLOAT 3���� ���� ������ ���ϴ�.
	D3DXVECTOR3 g_rotateAngle;

	// �ؽ�ó �������Դϴ�.
	IDirect3DTexture9* g_pTexture = nullptr;

	// ���� ��¿��Դϴ�.
	ExampleRenderInfo g_exampleRenderInfoMag;
	ExampleRenderInfo g_exampleRenderInfoMin;
	ExampleRenderInfo g_exampleRenderInfoMip;
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

// �⺻ �ؽ�ó ���ø� ������Ʈ�� �����մϴ�.
void DefaultSamplerState();

// ���� ���ۿ� ���� ������ �����մϴ�.
void InitVertexBuffer();

// �ε��� ���ۿ� �ε��� ������ �����մϴ�.
void InitIndexBuffer();

// ������� Ű���� �Ǵ� ���콺 �Է¿� ���� ó���� �մϴ�.
void OnUserInput();

// ���͸� ������ �������ݴϴ�.
const TCHAR* UpdateFilterType(ExampleRenderInfo* pInfo);

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
	DefaultSamplerState();

	// ���콺 Ŀ���� �����ݴϴ�.
	RX::ShowMouseCursor(true);

	// ���� ���ۿ� �ε��� ���ۿ� ������ �ֽ��ϴ�.
	InitVertexBuffer();
	InitIndexBuffer();

	// �ؽ�ó�� �ҷ��ɴϴ�.
	// �ؽ�ó ��θ� �ѱ� ���� �̽������� �������� �����ؾ� �մϴ�!
	// \�� �ƴ϶� \\���� �־�� ��θ� ����� ã���ϴ�.
	// �̽������� �������� ���� �ȴٸ� / �̰� ������ �˴ϴ�.
	g_DXResult = D3DXCreateTextureFromFile(
		D3DDEVICE9, // ���� ����̽� ������
		L"Resource/Texture/Kirby.jpg", // �ؽ�ó ������ �ִ� ���(�ַ�� ���� �������� ����)
		&g_pTexture); // �ؽ�ó ������
	DXERR_HANDLER(g_DXResult);

	// ���� ��¿� ������ �ʱ�ȭ�մϴ�.
	g_exampleRenderInfoMag.idx     = 0;
	g_exampleRenderInfoMag.value   = D3DTEXF_NONE;
	g_exampleRenderInfoMag.szValue = CONVERT_FLAG_TO_STRING(D3DTEXF_NONE);
	g_exampleRenderInfoMin = g_exampleRenderInfoMag;
	g_exampleRenderInfoMip = g_exampleRenderInfoMag;

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
	D3DDEVICE9->SetTexture(0, g_pTexture);
	D3DDEVICE9->SetFVF(VertexP3T1::format);
	D3DDEVICE9->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(VertexP3T1));
	D3DDEVICE9->SetIndices(g_pIndexBuffer);
	D3DDEVICE9->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 6, 0, 2);
	D3DDEVICE9->SetTexture(0, nullptr);

	TCHAR szText[DEFAULT_STRING_LENGTH];
	swprintf_s(szText, L"Ȯ��(T) ���(Y) �Ӹ�(U)\nȮ�� => %s\n��� => %s\n�Ӹ� => %s",
		g_exampleRenderInfoMag.szValue, g_exampleRenderInfoMin.szValue,
		g_exampleRenderInfoMip.szValue);
	RX::RXRendererDX9::Instance()->DrawTextOriginClientArea(
		szText, DT_LEFT, DXCOLOR_WHITE);
	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	SAFE_RELEASE(g_pTexture);
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

void DefaultSamplerState()
{
	// �ؽ�ó ��巹�� ��带 �����մϴ�. ����Ʈ�� Wrap ����Դϴ�.
	// U�� V ���� ��巹�� ��带 ������ �� �ֽ��ϴ�.
	D3DDEVICE9->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	D3DDEVICE9->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// ���͸��� �����մϴ�.
	// ���÷��� ���͸��� Ȯ��(Magnification), ���(Minification), �Ӹ�(Mipmap) �̷��� 3���Դϴ�.
	D3DDEVICE9->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	D3DDEVICE9->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	D3DDEVICE9->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
}

void InitVertexBuffer()
{
	VertexP3T1 vertex;
	::ZeroMemory(&vertex, sizeof(vertex));

	// ù��° �����Դϴ�.
	vertex.vPos.x = -1.0f;
	vertex.vPos.y =  1.0f;
	vertex.tex.u  =  0.0f;
	vertex.tex.v  =  0.0f;
	g_vecP3T1.push_back(vertex);

	// �ι�° �����Դϴ�.
	vertex.vPos.x =  1.0f;
	vertex.vPos.y =  1.0f;
	vertex.tex.u  =  1.0f;
	vertex.tex.v  =  0.0f;
	g_vecP3T1.push_back(vertex);

	// ����° �����Դϴ�.
	vertex.vPos.x = -1.0f;
	vertex.vPos.y = -1.0f;
	vertex.tex.u  =  0.0f;
	vertex.tex.v  =  1.0f;
	g_vecP3T1.push_back(vertex);

	// �׹�° �����Դϴ�.
	vertex.vPos.x =  1.0f;
	vertex.vPos.y = -1.0f;
	vertex.tex.u  =  1.0f;
	vertex.tex.v  =  1.0f;
	g_vecP3T1.push_back(vertex);

	// ���� ���� �����ϱ�
	INT32 verticesSize = g_vecP3T1.size() * sizeof(vertex);
	g_DXResult = D3DDEVICE9->CreateVertexBuffer(verticesSize, D3DUSAGE_WRITEONLY,
		VertexP3T1::format, D3DPOOL_MANAGED, &g_pVertexBuffer, nullptr);
	DXERR_HANDLER(g_DXResult);

	// ���� ���ۿ� ���� ���� �ֱ�
	void* pData = nullptr;
	g_pVertexBuffer->Lock(0, verticesSize, &pData, D3DFLAG_NONE);
	::CopyMemory(pData, &g_vecP3T1[0], verticesSize);
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
	g_DXResult = D3DDEVICE9->CreateIndexBuffer(indicesSize, D3DUSAGE_WRITEONLY,
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
		++g_exampleRenderInfoMag.idx;
		g_exampleRenderInfoMag.szValue = UpdateFilterType(&g_exampleRenderInfoMag);
		if (g_exampleRenderInfoMag.idx >= static_cast<INT32>(FILTER_TYPE::END))
		{
			g_exampleRenderInfoMag.idx     = 0;
			g_exampleRenderInfoMag.value   = D3DTEXF_NONE;
			g_exampleRenderInfoMag.szValue = CONVERT_FLAG_TO_STRING(D3DTEXF_NONE);
		}

		D3DDEVICE9->SetSamplerState(0, D3DSAMP_MAGFILTER, g_exampleRenderInfoMag.value);
	}

	if (::GetAsyncKeyState('Y') & 0x0001)
	{
		++g_exampleRenderInfoMin.idx;
		g_exampleRenderInfoMin.szValue = UpdateFilterType(&g_exampleRenderInfoMin);
		if (g_exampleRenderInfoMin.idx >= static_cast<INT32>(FILTER_TYPE::END))
		{
			g_exampleRenderInfoMin.idx     = 0;
			g_exampleRenderInfoMin.value   = D3DTEXF_NONE;
			g_exampleRenderInfoMin.szValue = CONVERT_FLAG_TO_STRING(D3DTEXF_NONE);
		}

		D3DDEVICE9->SetSamplerState(0, D3DSAMP_MINFILTER, g_exampleRenderInfoMin.value);
	}

	if (::GetAsyncKeyState('U') & 0x0001)
	{
		++g_exampleRenderInfoMip.idx;
		g_exampleRenderInfoMip.szValue = UpdateFilterType(&g_exampleRenderInfoMip);
		if (g_exampleRenderInfoMip.idx >= static_cast<INT32>(FILTER_TYPE::END))
		{
			g_exampleRenderInfoMip.idx     = 0;
			g_exampleRenderInfoMip.value   = D3DTEXF_NONE;
			g_exampleRenderInfoMip.szValue = CONVERT_FLAG_TO_STRING(D3DTEXF_NONE);
		}

		D3DDEVICE9->SetSamplerState(0, D3DSAMP_MIPFILTER, g_exampleRenderInfoMip.value);
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

	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matRot);
}

const TCHAR* UpdateFilterType(ExampleRenderInfo* pInfo)
{
	NULLCHK(pInfo);
	FILTER_TYPE value = static_cast<FILTER_TYPE>(pInfo->idx);
	switch (value)
	{
	case FILTER_TYPE::NONE:
	{
		pInfo->value = D3DTEXF_NONE;
		return CONVERT_FLAG_TO_STRING(D3DTEXF_NONE);
	}
	case FILTER_TYPE::POINT:
	{
		pInfo->value = D3DTEXF_POINT;
		return CONVERT_FLAG_TO_STRING(D3DTEXF_POINT);
	}
	case FILTER_TYPE::LINEAR:
	{
		pInfo->value = D3DTEXF_LINEAR;
		return CONVERT_FLAG_TO_STRING(D3DTEXF_LINEAR);
	}
	case FILTER_TYPE::ANISOTROPIC:
	{
		pInfo->value = D3DTEXF_ANISOTROPIC;
		D3DDEVICE9->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 8);
		return CONVERT_FLAG_TO_STRING(D3DTEXF_ANISOTROPIC);
	}
	case FILTER_TYPE::PYRAMIDALQUAD:
	{
		pInfo->value = D3DTEXF_PYRAMIDALQUAD;
		return CONVERT_FLAG_TO_STRING(D3DTEXF_PYRAMIDALQUAD);
	}
	case FILTER_TYPE::GAUSSIANQUAD:
	{
		pInfo->value = D3DTEXF_GAUSSIANQUAD;
		return CONVERT_FLAG_TO_STRING(D3DTEXF_GAUSSIANQUAD);
	}
	}

	return SZ_NULL;
}

#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


// ====================================================================================
// ����ü ������Դϴ�.
enum class ADDRESS_MODE : INT32
{
	WRAP = 1,   // ��(�ݺ�)
	MIRROR,     // �̷�(�ſ�ó�� �ݻ�)
	CLAMP,      // Ŭ����(��� �ȼ��� �ݺ�)
	BORDER,     // ����(��� ������ �����ؼ� �ݺ�)
	MIRRORONCE, // �ѹ��� �ݺ��ϴ� �̷�
	END,
};


// ====================================================================================
// ���� ���� ������Դϴ�.
IDirect3DDevice9* g_pD3DDevice9 = nullptr;
RX::RXMain_DX9*   g_pMainDX     = nullptr;

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
	ExampleRenderInfo g_exampleRenderInfoU;
	ExampleRenderInfo g_exampleRenderInfoV;
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

// ��巹�� ��� ������ �������ݴϴ�.
const TCHAR* UpdateAddressMode(ExampleRenderInfo* pInfo);

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
		g_pD3DDevice9, // ���� ����̽� ������
		L"Resource/Texture/Kirby.jpg", // �ؽ�ó ������ �ִ� ���(�ַ�� ���� �������� ����)
		&g_pTexture); // �ؽ�ó ������
	DXERR_HANDLER(g_DXResult);

	// ���� ��¿� ������ �ʱ�ȭ�մϴ�.
	g_exampleRenderInfoU.idx     = 1; // ���۰��� 1�� �Ϳ� ����!
	g_exampleRenderInfoU.value   = D3DTADDRESS_WRAP;
	g_exampleRenderInfoU.szValue = CONVERT_FLAG_TO_STRING(D3DTADDRESS_WRAP);
	g_exampleRenderInfoV = g_exampleRenderInfoU;

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
	g_pD3DDevice9->SetTexture(0, g_pTexture);
	g_pD3DDevice9->SetFVF(VertexP3T1::format);
	g_pD3DDevice9->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(VertexP3T1));
	g_pD3DDevice9->SetIndices(g_pIndexBuffer);
	g_pD3DDevice9->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 6, 0, 2);
	g_pD3DDevice9->SetTexture(0, nullptr);

	TCHAR szText[DEFAULT_STRING_LENGTH];
	swprintf_s(szText, L"U(T) V(Y)\nU => %s\nV => %s",
		g_exampleRenderInfoU.szValue, g_exampleRenderInfoV.szValue);
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
}

void DefaultSamplerState()
{
	// �ؽ�ó ��巹�� ��带 �����մϴ�. ����Ʈ�� Wrap ����Դϴ�.
	// U�� V ���� ��巹�� ��带 ������ �� �ֽ��ϴ�.
	g_pD3DDevice9->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	g_pD3DDevice9->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
}

void InitVertexBuffer()
{
	VertexP3T1 vertex;
	::ZeroMemory(&vertex, sizeof(vertex));

	// ù��° �����Դϴ�.
	vertex.vPos.x = -1.0f;
	vertex.vPos.y =  1.0f;
	vertex.tex.u  = -2.0f;
	vertex.tex.v  = -2.0f;
	g_vecP3T1.push_back(vertex);

	// �ι�° �����Դϴ�.
	vertex.vPos.x =  1.0f;
	vertex.vPos.y =  1.0f;
	vertex.tex.u  =  2.0f;
	vertex.tex.v  = -2.0f;
	g_vecP3T1.push_back(vertex);

	// ����° �����Դϴ�.
	vertex.vPos.x = -1.0f;
	vertex.vPos.y = -1.0f;
	vertex.tex.u  = -2.0f;
	vertex.tex.v  =  2.0f;
	g_vecP3T1.push_back(vertex);

	// �׹�° �����Դϴ�.
	vertex.vPos.x =  1.0f;
	vertex.vPos.y = -1.0f;
	vertex.tex.u  =  2.0f;
	vertex.tex.v  =  2.0f;
	g_vecP3T1.push_back(vertex);

	// ���� ���� �����ϱ�
	INT32 verticesSize = g_vecP3T1.size() * sizeof(vertex);
	g_DXResult = g_pD3DDevice9->CreateVertexBuffer(verticesSize, D3DUSAGE_WRITEONLY,
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
		g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	if (::GetAsyncKeyState('T') & 0x0001)
	{
		++g_exampleRenderInfoU.idx;
		g_exampleRenderInfoU.szValue = UpdateAddressMode(&g_exampleRenderInfoU);
		if (g_exampleRenderInfoU.idx >= static_cast<INT32>(ADDRESS_MODE::END))
		{
			g_exampleRenderInfoU.idx     = 1; // ���۰��� 1�� �Ϳ� ����!
			g_exampleRenderInfoU.value   = D3DTADDRESS_WRAP;
			g_exampleRenderInfoU.szValue = CONVERT_FLAG_TO_STRING(D3DTADDRESS_WRAP);
		}

		g_pD3DDevice9->SetSamplerState(0, D3DSAMP_ADDRESSU, g_exampleRenderInfoU.value);
	}

	if (::GetAsyncKeyState('Y') & 0x0001)
	{
		++g_exampleRenderInfoV.idx;
		g_exampleRenderInfoV.szValue = UpdateAddressMode(&g_exampleRenderInfoV);
		if (g_exampleRenderInfoV.idx >= static_cast<INT32>(ADDRESS_MODE::END))
		{
			g_exampleRenderInfoV.idx     = 1; // ���۰��� 1�� �Ϳ� ����!
			g_exampleRenderInfoV.value   = D3DTADDRESS_WRAP;
			g_exampleRenderInfoV.szValue = CONVERT_FLAG_TO_STRING(D3DTADDRESS_WRAP);
		}

		g_pD3DDevice9->SetSamplerState(0, D3DSAMP_ADDRESSV, g_exampleRenderInfoV.value);
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

const TCHAR* UpdateAddressMode(ExampleRenderInfo* pInfo)
{
	NULLCHK(pInfo);
	ADDRESS_MODE value = static_cast<ADDRESS_MODE>(pInfo->idx);
	switch (value)
	{
	case ADDRESS_MODE::WRAP:
	{
		pInfo->value = D3DTADDRESS_WRAP;
		return CONVERT_FLAG_TO_STRING(D3DTADDRESS_WRAP);
	}
	case ADDRESS_MODE::MIRROR:
	{
		pInfo->value = D3DTADDRESS_MIRROR;
		return CONVERT_FLAG_TO_STRING(D3DTADDRESS_MIRROR);
	}
	case ADDRESS_MODE::CLAMP:
	{
		pInfo->value = D3DTADDRESS_CLAMP;
		return CONVERT_FLAG_TO_STRING(D3DTADDRESS_CLAMP);
	}
	case ADDRESS_MODE::BORDER:
	{
		pInfo->value = D3DTADDRESS_BORDER;
		g_pD3DDevice9->SetSamplerState(0, D3DSAMP_BORDERCOLOR, DXCOLOR_GREEN);
		return CONVERT_FLAG_TO_STRING(D3DTADDRESS_BORDER);
	}
	case ADDRESS_MODE::MIRRORONCE:
	{
		pInfo->value = D3DTADDRESS_MIRRORONCE;
		return CONVERT_FLAG_TO_STRING(D3DTADDRESS_MIRRORONCE);
	}
	}

	return SZ_NULL;
}

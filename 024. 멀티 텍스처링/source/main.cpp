#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


// ====================================================================================
// ����ü ������Դϴ�.
enum class BLEND_OPERATION : INT32
{
	MODULATE,    // ����
	MODULATE2X,  // �����ϰ� 1��Ʈ ���� ����Ʈ(X2)
	MODULATE4X,  // �����ϰ� 2��Ʈ ���� ����Ʈ(X4)
	ADD,         // ����
	ADDSIGNED,   // �����ϰ� 0.5 ����(���̾)
	ADDSIGNED2X, // �����ϰ� 0.5 �� ������ 1��Ʈ ���� ����Ʈ(X2)
	SUBTRACT,    // ����
	ADDSMOOTH,   // �ΰ��� ���ؼ� �ΰ��� ���� ������ ��(A + B - AB => A + B(1-A))
	END,
};


// ====================================================================================
// ���� ���� ������Դϴ�.
HRESULT g_DXResult = S_OK;

IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
IDirect3DIndexBuffer9*  g_pIndexBuffer  = nullptr;
std::vector<VertexP3T2> g_vecP3T2;
std::vector<Index16>    g_vecIndex16;

namespace
{
	// ������ ���ͷθ� ���Ǵµ� �̹����� FLOAT 3���� ���� ������ ���ϴ�.
	D3DXVECTOR3 g_rotateAngle;

	// �ؽ�ó �������Դϴ�.
	IDirect3DTexture9* g_pTexture1 = nullptr;
	IDirect3DTexture9* g_pTexture2 = nullptr;

	// ���� ��¿��Դϴ�.
	ExampleRenderInfo g_exampleRenderInfo;
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

// �⺻ ��Ƽ �ؽ�ó�� ������Ʈ�� �����մϴ�.
void DefaultTextureStageState();

// ���� ���ۿ� ���� ������ �����մϴ�.
void InitVertexBuffer();

// �ε��� ���ۿ� �ε��� ������ �����մϴ�.
void InitIndexBuffer();

// ������� Ű���� �Ǵ� ���콺 �Է¿� ���� ó���� �մϴ�.
void OnUserInput();

// ���� ������ �������ݴϴ�.
const TCHAR* UpdateBlendOperation(ExampleRenderInfo* pInfo);

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
	DefaultTextureStageState();

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
		&g_pTexture1); // �ؽ�ó ������
	DXERR_HANDLER(g_DXResult);

	g_DXResult = D3DXCreateTextureFromFile(
		D3DDEVICE9, // ���� ����̽� ������
		L"Resource/Texture/LightMap.jpg", // �ؽ�ó ������ �ִ� ���(�ַ�� ���� �������� ����)
		&g_pTexture2); // �ؽ�ó ������
	DXERR_HANDLER(g_DXResult);

	// ���� ��¿� ������ �ʱ�ȭ�մϴ�.
	g_exampleRenderInfo.idx     = 0;
	g_exampleRenderInfo.value   = D3DTOP_MODULATE;
	g_exampleRenderInfo.szValue = CONVERT_FLAG_TO_STRING(D3DTOP_MODULATE);

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
	D3DDEVICE9->SetTexture(0, g_pTexture1);
	D3DDEVICE9->SetTexture(1, g_pTexture2);

	D3DDEVICE9->SetFVF(VertexP3T2::format);
	D3DDEVICE9->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(VertexP3T2));
	D3DDEVICE9->SetIndices(g_pIndexBuffer);
	D3DDEVICE9->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 6, 0, 2);

	D3DDEVICE9->SetTexture(0, nullptr);
	D3DDEVICE9->SetTexture(1, nullptr);

	TCHAR szText[DEFAULT_STRING_LENGTH];
	swprintf_s(szText, L"���� ����(T)\n���� ���� => %s", g_exampleRenderInfo.szValue);
	RX::RXRendererDX9::Instance()->DrawTextOriginClientArea(szText, DT_LEFT, DXCOLOR_WHITE);

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	SAFE_RELEASE(g_pTexture1);
	SAFE_RELEASE(g_pTexture2);
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

// ����� �ʿ� ���� �����ε� �����̹Ƿ� ���� �� �ۼ��ҰԿ�.
void DefaultTextureStageState()
{
	// �ؽ�ó �������� ������Ʈ�� �����մϴ�. (�ʼ� �ƴ�)
	D3DDEVICE9->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0); // ù��° UV ��ǥ
	D3DDEVICE9->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1); // �ι�° UV ��ǥ

	// ù��° ���������� ù��° �ȼ��� �ؽ�ó���� ��������
	// ���ٸ� ���� ���� �״�� ù��° �ȼ��� ����մϴ�. (�ʼ� �ƴ�)
	D3DDEVICE9->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	D3DDEVICE9->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

	// �ι�° ���������� ù��° �ȼ��� �ؽ�ó���� ��������
	// �ι�° �ȼ��� ���� �ܰ��� �ȼ� ���� ����� �����ͼ�
	// �� �ȼ��� ���� ���մϴ�. (Modulate, �ʼ�)
	D3DDEVICE9->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	D3DDEVICE9->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
	D3DDEVICE9->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);

	// 0���� �����ϹǷ� 2�� ����°�� �˴ϴ�.
	// ����°���ʹ� ������� �ʰڴٰ� �˸��ϴ�. (�ʼ� �ƴ�)
	D3DDEVICE9->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
}

void InitVertexBuffer()
{
	VertexP3T2 vertex;
	::ZeroMemory(&vertex, sizeof(vertex));

	// ù��° �����Դϴ�.
	vertex.vPos.x = -1.0f;
	vertex.vPos.y =  1.0f;
	vertex.tex1.u =  0.0f;
	vertex.tex1.v =  0.0f;
	vertex.tex2 = vertex.tex1;
	g_vecP3T2.push_back(vertex);

	// �ι�° �����Դϴ�.
	vertex.vPos.x =  1.0f;
	vertex.vPos.y =  1.0f;
	vertex.tex1.u =  1.0f;
	vertex.tex1.v =  0.0f;
	vertex.tex2 = vertex.tex1;
	g_vecP3T2.push_back(vertex);

	// ����° �����Դϴ�.
	vertex.vPos.x = -1.0f;
	vertex.vPos.y = -1.0f;
	vertex.tex1.u =  0.0f;
	vertex.tex1.v =  1.0f;
	vertex.tex2 = vertex.tex1;
	g_vecP3T2.push_back(vertex);

	// �׹�° �����Դϴ�.
	vertex.vPos.x =  1.0f;
	vertex.vPos.y = -1.0f;
	vertex.tex1.u =  1.0f;
	vertex.tex1.v =  1.0f;
	vertex.tex2 = vertex.tex1;
	g_vecP3T2.push_back(vertex);

	// ���� ���� �����ϱ�
	INT32 verticesSize = g_vecP3T2.size() * sizeof(vertex);
	g_DXResult = D3DDEVICE9->CreateVertexBuffer(verticesSize, D3DUSAGE_WRITEONLY,
		VertexP3T2::format, D3DPOOL_MANAGED, &g_pVertexBuffer, nullptr);
	DXERR_HANDLER(g_DXResult);

	// ���� ���ۿ� ���� ���� �ֱ�
	void* pData = nullptr;
	g_pVertexBuffer->Lock(0, verticesSize, &pData, D3DLOCK_READONLY);
	::CopyMemory(pData, &g_vecP3T2[0], verticesSize);
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
	g_pIndexBuffer->Lock(0, indicesSize, &pData, D3DLOCK_READONLY);
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
		++g_exampleRenderInfo.idx;
		g_exampleRenderInfo.szValue = UpdateBlendOperation(&g_exampleRenderInfo);
		if (g_exampleRenderInfo.idx >= static_cast<INT32>(BLEND_OPERATION::END))
		{
			g_exampleRenderInfo.idx     = 0;
			g_exampleRenderInfo.value   = D3DTOP_MODULATE;
			g_exampleRenderInfo.szValue = CONVERT_FLAG_TO_STRING(D3DTOP_MODULATE);
		}

		D3DDEVICE9->SetTextureStageState(1, D3DTSS_COLOROP, g_exampleRenderInfo.value);
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

const TCHAR* UpdateBlendOperation(ExampleRenderInfo * pInfo)
{
	NULLCHK(pInfo);
	BLEND_OPERATION value = static_cast<BLEND_OPERATION>(pInfo->idx);
	switch (value)
	{
	case BLEND_OPERATION::MODULATE:
	{
		pInfo->value = D3DTOP_MODULATE;
		return CONVERT_FLAG_TO_STRING(D3DTOP_MODULATE);
	}
	case BLEND_OPERATION::MODULATE2X:
	{
		pInfo->value = D3DTOP_MODULATE2X;
		return CONVERT_FLAG_TO_STRING(D3DTOP_MODULATE2X);
	}
	case BLEND_OPERATION::MODULATE4X:
	{
		pInfo->value = D3DTOP_MODULATE4X;
		return CONVERT_FLAG_TO_STRING(D3DTOP_MODULATE4X);
	}
	case BLEND_OPERATION::ADD:
	{
		pInfo->value = D3DTOP_ADD;
		return CONVERT_FLAG_TO_STRING(D3DTOP_ADD);
	}
	case BLEND_OPERATION::ADDSIGNED:
	{
		pInfo->value = D3DTOP_ADDSIGNED;
		return CONVERT_FLAG_TO_STRING(D3DTOP_ADDSIGNED);
	}
	case BLEND_OPERATION::ADDSIGNED2X:
	{
		pInfo->value = D3DTOP_ADDSIGNED2X;
		return CONVERT_FLAG_TO_STRING(D3DTOP_ADDSIGNED2X);
	}
	case BLEND_OPERATION::SUBTRACT:
	{
		pInfo->value = D3DTOP_SUBTRACT;
		return CONVERT_FLAG_TO_STRING(D3DTOP_SUBTRACT);
	}
	case BLEND_OPERATION::ADDSMOOTH:
	{
		pInfo->value = D3DTOP_ADDSMOOTH;
		return CONVERT_FLAG_TO_STRING(D3DTOP_ADDSMOOTH);
	}
	}

	return SZ_NULL;
}

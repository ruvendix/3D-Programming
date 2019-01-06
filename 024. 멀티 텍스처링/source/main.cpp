#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


// ====================================================================================
// 공용체 선언부입니다.
enum class BLEND_OPERATION : INT32
{
	MODULATE,    // 곱셈
	MODULATE2X,  // 곱셈하고 1비트 왼쪽 시프트(X2)
	MODULATE4X,  // 곱셈하고 2비트 왼쪽 시프트(X4)
	ADD,         // 덧셈
	ADDSIGNED,   // 덧셈하고 0.5 빼기(바이어스)
	ADDSIGNED2X, // 덧셈하고 0.5 뺀 다음에 1비트 왼쪽 시프트(X2)
	SUBTRACT,    // 뺄셈
	ADDSMOOTH,   // 두값을 더해서 두값을 곱한 값에서 뺌(A + B - AB => A + B(1-A))
	END,
};


// ====================================================================================
// 전역 변수 선언부입니다.
HRESULT g_DXResult = S_OK;

namespace
{
	IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
	IDirect3DIndexBuffer9*  g_pIndexBuffer  = nullptr;

	std::vector<VertexP3T2> g_vecP3T2;
	std::vector<Index16>    g_vecIndex16;

	D3DXVECTOR3 g_rotateAngle;

	IDirect3DTexture9* g_pTexture1 = nullptr;
	IDirect3DTexture9* g_pTexture2 = nullptr;

	ExampleRenderInfo g_exampleRenderInfo;
}


// ====================================================================================
// 함수 선언부입니다.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

void DefaultMatrix();
void DefaultRenderState();
void DefaultSamplerState();
void InitVertexBuffer();
void InitIndexBuffer();
void OnUserInput();

// 기본 멀티 텍스처링 스테이트를 설정합니다.
void DefaultTextureStageState();

// 블렌드 정보를 갱신해줍니다.
const TCHAR* UpdateBlendOperation(ExampleRenderInfo* pInfo);


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
	DefaultSamplerState();
	DefaultTextureStageState();

	RX::ShowMouseCursor(true);

	InitVertexBuffer();
	InitIndexBuffer();

	g_DXResult = D3DXCreateTextureFromFile(D3DDEVICE9, L"Resource/Texture/Kirby.jpg", &g_pTexture1);
	DXERR_HANDLER(g_DXResult);

	g_DXResult = D3DXCreateTextureFromFile(D3DDEVICE9, L"Resource/Texture/LightMap.jpg", &g_pTexture2);
	DXERR_HANDLER(g_DXResult);

	g_exampleRenderInfo.idx     = 0;
	g_exampleRenderInfo.value   = D3DTOP_MODULATE;
	g_exampleRenderInfo.szValue = CONVERT_FLAG_TO_STRING(D3DTOP_MODULATE);

	return S_OK;
}

HRESULT CALLBACK OnUpdate()
{
	OnUserInput();
	return S_OK;
}

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
	swprintf_s(szText, L"연산 변경(T)\n현재 연산 => %s", g_exampleRenderInfo.szValue);
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

// 몇몇은 필요 없는 과정인데 예제이므로 전부 다 작성할게요.
void DefaultTextureStageState()
{
	// 텍스처 스테이지 스테이트를 설정합니다. (필수 아님)
	D3DDEVICE9->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0); // 첫번째 UV 좌표
	D3DDEVICE9->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1); // 두번째 UV 좌표

	// 첫번째 스테이지의 첫번째 픽셀은 텍스처에서 가져오고
	// 별다른 연산 없이 그대로 첫번째 픽셀을 사용합니다. (필수 아님)
	D3DDEVICE9->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	D3DDEVICE9->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

	// 두번째 스테이지의 첫번째 픽셀은 텍스처에서 가져오고
	// 두번째 픽셀은 이전 단계의 픽셀 연산 결과를 가져와서
	// 두 픽셀을 서로 곱합니다. (Modulate, 필수)
	D3DDEVICE9->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	D3DDEVICE9->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
	D3DDEVICE9->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);

	// 0부터 시작하므로 2는 세번째가 됩니다.
	// 세번째부터는 사용하지 않겠다고 알립니다. (필수 아님)
	D3DDEVICE9->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
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

#pragma region 중복되는 이전 코드
void DefaultMatrix()
{
	// ==========================================================
	// 뷰행렬을 설정합니다.
	D3DXVECTOR3 vEye(0.0f, 0.0f, -4.0f);   // 카메라의 위치
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

void DefaultSamplerState()
{
	D3DDEVICE9->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	D3DDEVICE9->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	D3DDEVICE9->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	D3DDEVICE9->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	D3DDEVICE9->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
}

void InitVertexBuffer()
{
	VertexP3T2 vertex;
	::ZeroMemory(&vertex, sizeof(vertex));

	vertex.vPos.x = -1.0f;
	vertex.vPos.y = 1.0f;
	vertex.tex1.u = 0.0f;
	vertex.tex1.v = 0.0f;
	vertex.tex2 = vertex.tex1;
	g_vecP3T2.push_back(vertex);

	vertex.vPos.x = 1.0f;
	vertex.vPos.y = 1.0f;
	vertex.tex1.u = 1.0f;
	vertex.tex1.v = 0.0f;
	vertex.tex2 = vertex.tex1;
	g_vecP3T2.push_back(vertex);

	vertex.vPos.x = -1.0f;
	vertex.vPos.y = -1.0f;
	vertex.tex1.u = 0.0f;
	vertex.tex1.v = 1.0f;
	vertex.tex2 = vertex.tex1;
	g_vecP3T2.push_back(vertex);

	vertex.vPos.x = 1.0f;
	vertex.vPos.y = -1.0f;
	vertex.tex1.u = 1.0f;
	vertex.tex1.v = 1.0f;
	vertex.tex2 = vertex.tex1;
	g_vecP3T2.push_back(vertex);

	INT32 verticesSize = g_vecP3T2.size() * sizeof(vertex);
	g_DXResult = D3DDEVICE9->CreateVertexBuffer(verticesSize, D3DUSAGE_WRITEONLY,
		VertexP3T2::format, D3DPOOL_MANAGED, &g_pVertexBuffer, nullptr);
	DXERR_HANDLER(g_DXResult);

	void* pData = nullptr;
	g_pVertexBuffer->Lock(0, verticesSize, &pData, D3DLOCK_READONLY);
	::CopyMemory(pData, &g_vecP3T2[0], verticesSize);
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
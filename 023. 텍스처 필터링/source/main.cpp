#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


// ====================================================================================
// 공용체 선언부입니다.
enum class FILTER_TYPE : INT32
{
	NONE,
	POINT,         // 근접점
	LINEAR,        // 선형 
	ANISOTROPIC,   // 비등방성(이방성, 이등방성)
	PYRAMIDALQUAD, // 피라미드 쿼드
	GAUSSIANQUAD,  // 가우시안 쿼드
	END,
};


// ====================================================================================
// 전역 변수 선언부입니다.
HRESULT g_DXResult = S_OK;

namespace
{
	IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
	IDirect3DIndexBuffer9*  g_pIndexBuffer  = nullptr;

	std::vector<VertexP3T1> g_vecP3T1;
	std::vector<Index16>    g_vecIndex16;

	D3DXVECTOR3 g_rotateAngle;

	IDirect3DTexture9* g_pTexture = nullptr;

	ExampleRenderInfo g_exampleRenderInfoMag;
	ExampleRenderInfo g_exampleRenderInfoMin;
	ExampleRenderInfo g_exampleRenderInfoMip;
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

// 필터링 정보를 갱신해줍니다.
const TCHAR* UpdateFilterType(ExampleRenderInfo* pInfo);

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

	RX::ShowMouseCursor(true);

	InitVertexBuffer();
	InitIndexBuffer();

	g_DXResult = D3DXCreateTextureFromFile(D3DDEVICE9, L"Resource/Texture/Kirby.jpg", &g_pTexture);
	DXERR_HANDLER(g_DXResult);

	g_exampleRenderInfoMag.idx     = 0;
	g_exampleRenderInfoMag.value   = D3DTEXF_NONE;
	g_exampleRenderInfoMag.szValue = CONVERT_FLAG_TO_STRING(D3DTEXF_NONE);
	g_exampleRenderInfoMin = g_exampleRenderInfoMag;
	g_exampleRenderInfoMip = g_exampleRenderInfoMag;

	return S_OK;
}

HRESULT CALLBACK OnUpdate()
{
	OnUserInput();
	return S_OK;
}

HRESULT CALLBACK OnRender()
{
	D3DDEVICE9->SetTexture(0, g_pTexture);
	D3DDEVICE9->SetFVF(VertexP3T1::format);
	D3DDEVICE9->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(VertexP3T1));
	D3DDEVICE9->SetIndices(g_pIndexBuffer);
	D3DDEVICE9->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 6, 0, 2);
	D3DDEVICE9->SetTexture(0, nullptr);

	TCHAR szText[DEFAULT_STRING_LENGTH];
	swprintf_s(szText, L"확대(T) 축소(Y) 밉맵(U)\n확대 => %s\n축소 => %s\n밉맵 => %s",
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

void DefaultSamplerState()
{
	// 텍스처 어드레스 모드를 설정합니다. 디폴트는 Wrap 모드입니다.
	// U와 V 따로 어드레스 모드를 설정할 수 있습니다.
	D3DDEVICE9->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	D3DDEVICE9->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// 필터링을 설정합니다.
	// 샘플러의 필터링은 확대(Magnification), 축소(Minification), 밉맵(Mipmap) 이렇게 3개입니다.
	D3DDEVICE9->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	D3DDEVICE9->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	D3DDEVICE9->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
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

	g_rotateAngle.z = RX::AdjustAngle(g_rotateAngle.z);
	g_rotateAngle.x = RX::AdjustAngle(g_rotateAngle.x);

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

void InitVertexBuffer()
{
	VertexP3T1 vertex;
	::ZeroMemory(&vertex, sizeof(vertex));

	vertex.vPos.x = -1.0f;
	vertex.vPos.y = 1.0f;
	vertex.tex.u = -2.0f;
	vertex.tex.v = -2.0f;
	g_vecP3T1.push_back(vertex);

	vertex.vPos.x = 1.0f;
	vertex.vPos.y = 1.0f;
	vertex.tex.u = 2.0f;
	vertex.tex.v = -2.0f;
	g_vecP3T1.push_back(vertex);

	vertex.vPos.x = -1.0f;
	vertex.vPos.y = -1.0f;
	vertex.tex.u = -2.0f;
	vertex.tex.v = 2.0f;
	g_vecP3T1.push_back(vertex);

	vertex.vPos.x = 1.0f;
	vertex.vPos.y = -1.0f;
	vertex.tex.u = 2.0f;
	vertex.tex.v = 2.0f;
	g_vecP3T1.push_back(vertex);

	INT32 verticesSize = g_vecP3T1.size() * sizeof(vertex);
	g_DXResult = D3DDEVICE9->CreateVertexBuffer(verticesSize, D3DUSAGE_WRITEONLY,
		VertexP3T1::format, D3DPOOL_MANAGED, &g_pVertexBuffer, nullptr);
	DXERR_HANDLER(g_DXResult);

	void* pData = nullptr;
	g_pVertexBuffer->Lock(0, verticesSize, &pData, D3DLOCK_READONLY);
	::CopyMemory(pData, &g_vecP3T1[0], verticesSize);
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
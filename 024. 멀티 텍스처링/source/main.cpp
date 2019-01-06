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

IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
IDirect3DIndexBuffer9*  g_pIndexBuffer  = nullptr;
std::vector<VertexP3T2> g_vecP3T2;
std::vector<Index16>    g_vecIndex16;

namespace
{
	// 원래는 벡터로만 사용되는데 이번에는 FLOAT 3개를 묶은 것으로 봅니다.
	D3DXVECTOR3 g_rotateAngle;

	// 텍스처 포인터입니다.
	IDirect3DTexture9* g_pTexture1 = nullptr;
	IDirect3DTexture9* g_pTexture2 = nullptr;

	// 예제 출력용입니다.
	ExampleRenderInfo g_exampleRenderInfo;
}


// ====================================================================================
// 함수 선언부입니다.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

// 기본 행렬을 설정합니다.
void DefaultMatrix();

// 기본 렌더 스테이트를 설정합니다.
void DefaultRenderState();

// 기본 텍스처 샘플링 스테이트를 설정합니다.
void DefaultSamplerState();

// 기본 멀티 텍스처링 스테이트를 설정합니다.
void DefaultTextureStageState();

// 정점 버퍼에 정점 정보를 설정합니다.
void InitVertexBuffer();

// 인덱스 버퍼에 인덱스 정보를 설정합니다.
void InitIndexBuffer();

// 사용자의 키보드 또는 마우스 입력에 따른 처리를 합니다.
void OnUserInput();

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

// 초기화 함수입니다.
// 3D 렌더링은 연산이 많이 들어가므로 웬만한 작업은 초기화해줘야 합니다.
// 렌더링하면서 실시간으로 연산도 가능하지만 그렇게 되면 프레임이 떨어지게 됩니다.
// 일반적으로 렌더링할 때는 렌더링 작업만 처리합니다.
HRESULT CALLBACK OnInit()
{
	DefaultMatrix();
	DefaultRenderState();
	DefaultSamplerState();
	DefaultTextureStageState();

	// 마우스 커서를 보여줍니다.
	RX::ShowMouseCursor(true);

	// 정점 버퍼와 인덱스 버퍼에 정보를 넣습니다.
	InitVertexBuffer();
	InitIndexBuffer();

	// 텍스처를 불러옵니다.
	// 텍스처 경로를 넘길 때는 이스케이프 시퀸스를 주의해야 합니다!
	// \이 아니라 \\으로 넣어야 경로를 제대로 찾습니다.
	// 이스케이프 시퀸스를 쓰기 싫다면 / 이걸 넣으면 됩니다.
	g_DXResult = D3DXCreateTextureFromFile(
		D3DDEVICE9, // 가상 디바이스 포인터
		L"Resource/Texture/Kirby.jpg", // 텍스처 파일이 있는 경로(솔루션 폴더 기준으로 잡음)
		&g_pTexture1); // 텍스처 포인터
	DXERR_HANDLER(g_DXResult);

	g_DXResult = D3DXCreateTextureFromFile(
		D3DDEVICE9, // 가상 디바이스 포인터
		L"Resource/Texture/LightMap.jpg", // 텍스처 파일이 있는 경로(솔루션 폴더 기준으로 잡음)
		&g_pTexture2); // 텍스처 포인터
	DXERR_HANDLER(g_DXResult);

	// 예제 출력용 정보를 초기화합니다.
	g_exampleRenderInfo.idx     = 0;
	g_exampleRenderInfo.value   = D3DTOP_MODULATE;
	g_exampleRenderInfo.szValue = CONVERT_FLAG_TO_STRING(D3DTOP_MODULATE);

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

void DefaultMatrix()
{
	// =====================================================================
	// 뷰행렬을 설정합니다.
	D3DXVECTOR3 vEye(0.0f, 0.0f, -4.0f);   // 카메라의 위치
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // 카메라가 보는 지점
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // 카메라의 업 벡터

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	D3DDEVICE9->SetTransform(D3DTS_VIEW, &matView);
	
	// =====================================================================
	// 투영행렬을 설정합니다.
	D3DXMATRIXA16 matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection, (D3DX_PI / 4.0f),
		(static_cast<FLOAT>(RXMAIN_DX9->getClientRect()->CalcWidth()) /
		                   (RXMAIN_DX9->getClientRect()->CalcHeight())), 1.0f, 1000.0f);
	D3DDEVICE9->SetTransform(D3DTS_PROJECTION, &matProjection);
}

void DefaultRenderState()
{
	// rhw를 사용하지 않는다면 변환 이전의 공간좌표를
	// 사용하게 되므로 각종 변환 과정을 거쳐야 합니다.
	// 조명(라이팅, Lighting)도 그중 하나인데
	// 이번에는 조명을 사용하지 않으므로 조명을 꺼줍니다.
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, FALSE);

	// 필 모드를 설정합니다. 디폴트는 솔리드입니다.
	D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	//D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);

	// 컬링 모드를 설정합니다. 디폴트는 반시계방향 컬링입니다.
	// 큐브를 확인하기 위해서는 컬링 모드를 무시해야 합니다.
	D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	//D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
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

void InitVertexBuffer()
{
	VertexP3T2 vertex;
	::ZeroMemory(&vertex, sizeof(vertex));

	// 첫번째 정점입니다.
	vertex.vPos.x = -1.0f;
	vertex.vPos.y =  1.0f;
	vertex.tex1.u =  0.0f;
	vertex.tex1.v =  0.0f;
	vertex.tex2 = vertex.tex1;
	g_vecP3T2.push_back(vertex);

	// 두번째 정점입니다.
	vertex.vPos.x =  1.0f;
	vertex.vPos.y =  1.0f;
	vertex.tex1.u =  1.0f;
	vertex.tex1.v =  0.0f;
	vertex.tex2 = vertex.tex1;
	g_vecP3T2.push_back(vertex);

	// 세번째 정점입니다.
	vertex.vPos.x = -1.0f;
	vertex.vPos.y = -1.0f;
	vertex.tex1.u =  0.0f;
	vertex.tex1.v =  1.0f;
	vertex.tex2 = vertex.tex1;
	g_vecP3T2.push_back(vertex);

	// 네번째 정점입니다.
	vertex.vPos.x =  1.0f;
	vertex.vPos.y = -1.0f;
	vertex.tex1.u =  1.0f;
	vertex.tex1.v =  1.0f;
	vertex.tex2 = vertex.tex1;
	g_vecP3T2.push_back(vertex);

	// 정점 버퍼 생성하기
	INT32 verticesSize = g_vecP3T2.size() * sizeof(vertex);
	g_DXResult = D3DDEVICE9->CreateVertexBuffer(verticesSize, D3DUSAGE_WRITEONLY,
		VertexP3T2::format, D3DPOOL_MANAGED, &g_pVertexBuffer, nullptr);
	DXERR_HANDLER(g_DXResult);

	// 정점 버퍼에 정점 정보 넣기
	void* pData = nullptr;
	g_pVertexBuffer->Lock(0, verticesSize, &pData, D3DLOCK_READONLY);
	::CopyMemory(pData, &g_vecP3T2[0], verticesSize);
	g_pVertexBuffer->Unlock();
}

void InitIndexBuffer()
{
	Index16 index;
	::ZeroMemory(&index, sizeof(index));

	// 첫번째 인덱스입니다.
	index.index = 0;
	g_vecIndex16.push_back(index);

	// 두번째 인덱스입니다.
	index.index = 3;
	g_vecIndex16.push_back(index);

	// 세번째 인덱스입니다.
	index.index = 2;
	g_vecIndex16.push_back(index);

	// 네번째 인덱스입니다.
	index.index = 0;
	g_vecIndex16.push_back(index);

	// 다섯번째 인덱스입니다.
	index.index = 1;
	g_vecIndex16.push_back(index);

	// 여섯번째 인덱스입니다.
	index.index = 3;
	g_vecIndex16.push_back(index);

	// 인덱스 버퍼 생성하기
	INT32 indicesSize = g_vecIndex16.size() * sizeof(index);
	g_DXResult = D3DDEVICE9->CreateIndexBuffer(indicesSize, D3DUSAGE_WRITEONLY,
		Index16::format, D3DPOOL_MANAGED, &g_pIndexBuffer, nullptr);
	DXERR_HANDLER(g_DXResult);

	// 인덱스 버퍼에 인덱스 정보 넣기
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

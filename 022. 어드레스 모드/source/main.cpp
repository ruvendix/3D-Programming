#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


// ====================================================================================
// 공용체 선언부입니다.
enum class ADDRESS_MODE : INT32
{
	WRAP = 1,   // 랩(반복)
	MIRROR,     // 미러(거울처럼 반사)
	CLAMP,      // 클램프(경계 픽셀로 반복)
	BORDER,     // 보더(경계 색상을 지정해서 반복)
	MIRRORONCE, // 한번만 반복하는 미러
	END,
};


// ====================================================================================
// 전역 변수 선언부입니다.
IDirect3DDevice9* g_pD3DDevice9 = nullptr;
RX::RXMain_DX9*   g_pMainDX     = nullptr;

HRESULT g_DXResult = S_OK;

IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
IDirect3DIndexBuffer9*  g_pIndexBuffer  = nullptr;
std::vector<VertexP3T1> g_vecP3T1;
std::vector<Index16>    g_vecIndex16;

namespace
{
	// 원래는 벡터로만 사용되는데 이번에는 FLOAT 3개를 묶은 것으로 봅니다.
	D3DXVECTOR3 g_rotateAngle;

	// 텍스처 포인터입니다.
	IDirect3DTexture9* g_pTexture = nullptr;

	// 예제 출력용입니다.
	ExampleRenderInfo g_exampleRenderInfoU;
	ExampleRenderInfo g_exampleRenderInfoV;
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

// 정점 버퍼에 정점 정보를 설정합니다.
void InitVertexBuffer();

// 인덱스 버퍼에 인덱스 정보를 설정합니다.
void InitIndexBuffer();

// 사용자의 키보드 또는 마우스 입력에 따른 처리를 합니다.
void OnUserInput();

// 어드레스 모드 정보를 갱신해줍니다.
const TCHAR* UpdateAddressMode(ExampleRenderInfo* pInfo);

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

	g_pMainDX = RXNew RX::RXMain_DX9;
	NULLCHK(g_pMainDX);

	g_pMainDX->setSubFunc(OnInit,    SUBFUNC_TYPE::INIT);
	g_pMainDX->setSubFunc(OnUpdate,  SUBFUNC_TYPE::UPDATE);
	g_pMainDX->setSubFunc(OnRender,  SUBFUNC_TYPE::RENDER);
	g_pMainDX->setSubFunc(OnRelease, SUBFUNC_TYPE::RELEASE);

	// 메모리 할당 순서를 이용해서 메모리 누수를 찾습니다.
	// 평소에는 주석 처리하면 됩니다.
	//_CrtSetBreakAlloc(451);

	g_pMainDX->RunMainRoutine(hInstance, IDI_RUVENDIX_ICO);

	INT32 messageCode = g_pMainDX->getMessageCode();
	SAFE_DELTE(g_pMainDX);
	return messageCode;
}

// 초기화 함수입니다.
// 3D 렌더링은 연산이 많이 들어가므로 웬만한 작업은 초기화해줘야 합니다.
// 렌더링하면서 실시간으로 연산도 가능하지만 그렇게 되면 프레임이 떨어지게 됩니다.
// 일반적으로 렌더링할 때는 렌더링 작업만 처리합니다.
HRESULT CALLBACK OnInit()
{
	g_pD3DDevice9 = RX::RXRendererDX9::Instance()->getD3DDevice9();
	NULLCHK(g_pD3DDevice9);

	DefaultMatrix();
	DefaultRenderState();
	DefaultSamplerState();

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
		g_pD3DDevice9, // 가상 디바이스 포인터
		L"Resource/Texture/Kirby.jpg", // 텍스처 파일이 있는 경로(솔루션 폴더 기준으로 잡음)
		&g_pTexture); // 텍스처 포인터
	DXERR_HANDLER(g_DXResult);

	// 예제 출력용 정보를 초기화합니다.
	g_exampleRenderInfoU.idx     = 1; // 시작값이 1인 것에 주의!
	g_exampleRenderInfoU.value   = D3DTADDRESS_WRAP;
	g_exampleRenderInfoU.szValue = CONVERT_FLAG_TO_STRING(D3DTADDRESS_WRAP);
	g_exampleRenderInfoV = g_exampleRenderInfoU;

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
	// 뷰행렬을 설정합니다.
	D3DXVECTOR3 vEye(0.0f, 0.0f, -4.0f);   // 카메라의 위치
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // 카메라가 보는 지점
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // 카메라의 업 벡터

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	g_pD3DDevice9->SetTransform(D3DTS_VIEW, &matView);
	
	// =====================================================================
	// 투영행렬을 설정합니다.
	D3DXMATRIXA16 matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection, (D3DX_PI / 4.0f),
		(static_cast<FLOAT>(g_pMainDX->getClientWidth()) / (g_pMainDX->getClientHeight())),
		1.0f, 1000.0f);
	g_pD3DDevice9->SetTransform(D3DTS_PROJECTION, &matProjection);
}

void DefaultRenderState()
{
	// rhw를 사용하지 않는다면 변환 이전의 공간좌표를
	// 사용하게 되므로 각종 변환 과정을 거쳐야 합니다.
	// 조명(라이팅, Lighting)도 그중 하나인데
	// 이번에는 조명을 사용하지 않으므로 조명을 꺼줍니다.
	g_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, FALSE);

	// 필 모드를 설정합니다. 디폴트는 솔리드입니다.
	g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	//g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);

	// 컬링 모드를 설정합니다. 디폴트는 반시계방향 컬링입니다.
	// 큐브를 확인하기 위해서는 컬링 모드를 무시해야 합니다.
	g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	//g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}

void DefaultSamplerState()
{
	// 텍스처 어드레스 모드를 설정합니다. 디폴트는 Wrap 모드입니다.
	// U와 V 따로 어드레스 모드를 설정할 수 있습니다.
	g_pD3DDevice9->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	g_pD3DDevice9->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
}

void InitVertexBuffer()
{
	VertexP3T1 vertex;
	::ZeroMemory(&vertex, sizeof(vertex));

	// 첫번째 정점입니다.
	vertex.vPos.x = -1.0f;
	vertex.vPos.y =  1.0f;
	vertex.tex.u  = -2.0f;
	vertex.tex.v  = -2.0f;
	g_vecP3T1.push_back(vertex);

	// 두번째 정점입니다.
	vertex.vPos.x =  1.0f;
	vertex.vPos.y =  1.0f;
	vertex.tex.u  =  2.0f;
	vertex.tex.v  = -2.0f;
	g_vecP3T1.push_back(vertex);

	// 세번째 정점입니다.
	vertex.vPos.x = -1.0f;
	vertex.vPos.y = -1.0f;
	vertex.tex.u  = -2.0f;
	vertex.tex.v  =  2.0f;
	g_vecP3T1.push_back(vertex);

	// 네번째 정점입니다.
	vertex.vPos.x =  1.0f;
	vertex.vPos.y = -1.0f;
	vertex.tex.u  =  2.0f;
	vertex.tex.v  =  2.0f;
	g_vecP3T1.push_back(vertex);

	// 정점 버퍼 생성하기
	INT32 verticesSize = g_vecP3T1.size() * sizeof(vertex);
	g_DXResult = g_pD3DDevice9->CreateVertexBuffer(verticesSize, D3DUSAGE_WRITEONLY,
		VertexP3T1::format, D3DPOOL_MANAGED, &g_pVertexBuffer, nullptr);
	DXERR_HANDLER(g_DXResult);

	// 정점 버퍼에 정점 정보 넣기
	void* pData = nullptr;
	g_pVertexBuffer->Lock(0, verticesSize, &pData, D3DFLAG_NONE);
	::CopyMemory(pData, &g_vecP3T1[0], verticesSize);
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
	g_DXResult = g_pD3DDevice9->CreateIndexBuffer(indicesSize, D3DUSAGE_WRITEONLY,
		Index16::format, D3DPOOL_MANAGED, &g_pIndexBuffer, nullptr);
	DXERR_HANDLER(g_DXResult);

	// 인덱스 버퍼에 인덱스 정보 넣기
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
			g_exampleRenderInfoU.idx     = 1; // 시작값이 1인 것에 주의!
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
			g_exampleRenderInfoV.idx     = 1; // 시작값이 1인 것에 주의!
			g_exampleRenderInfoV.value   = D3DTADDRESS_WRAP;
			g_exampleRenderInfoV.szValue = CONVERT_FLAG_TO_STRING(D3DTADDRESS_WRAP);
		}

		g_pD3DDevice9->SetSamplerState(0, D3DSAMP_ADDRESSV, g_exampleRenderInfoV.value);
	}
	
	// 각도 보정
	g_rotateAngle.z = RX::AdjustAngle(g_rotateAngle.z);
	g_rotateAngle.x = RX::AdjustAngle(g_rotateAngle.x);

	// 회전행렬입니다. 순서는 Z -> X -> Y입니다.
	// 즉, Roll -> Pitch -> Yaw입니다.
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

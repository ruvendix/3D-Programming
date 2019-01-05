#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


// ====================================================================================
// 전역 변수 선언부입니다.
HRESULT g_DXResult = S_OK;

namespace
{
	IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
}


// ====================================================================================
// 함수 선언부입니다.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

void RotationBasisAxisTest(FLOAT rDegreeX, FLOAT rDegreeY, FLOAT rDegreeZ);
void RotationEulerAngleTest(FLOAT rDegreeX, FLOAT rDegreeY, FLOAT rDegreeZ);
void RotationFreeAxis(const D3DXVECTOR3& vAxis, FLOAT rDegree);
void RotationQuaternionAxisTest(const D3DXVECTOR3& vAxis, FLOAT rDegree);


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
	RXMAIN_DX9->setSubFunc(OnRender,  SUBFUNC_TYPE::RENDER);
	RXMAIN_DX9->setSubFunc(OnRelease, SUBFUNC_TYPE::RELEASE);

	RXMAIN_DX9->RunMainRoutine(hInstance, IDI_RUVENDIX_ICO);
	return RXMAIN_DX9->getMessageCode();
}

HRESULT CALLBACK OnInit()
{
	VertexP3D vertices[3] =
	{
		{ {  0.0f,  0.2f , 0.0f }, DXCOLOR_RED   },
	    { {  0.2f, -0.2f , 0.0f }, DXCOLOR_GREEN },
		{ { -0.2f, -0.2f , 0.0f }, DXCOLOR_BLUE  },
	};

	g_DXResult = D3DDEVICE9->CreateVertexBuffer(sizeof(VertexP3D) * 3, D3DUSAGE_WRITEONLY,
		VertexP3D::format, D3DPOOL_MANAGED, &g_pVertexBuffer, nullptr);
	DXERR_HANDLER(g_DXResult);
	NULLCHK_RETURN_EFAIL(g_pVertexBuffer, "정점 버퍼 초기화 실패!");
	
	void* pVertexData = nullptr;
	g_pVertexBuffer->Lock(0, sizeof(VertexP3D) * 3, &pVertexData, D3DLOCK_READONLY);
	::CopyMemory(pVertexData, vertices, sizeof(VertexP3D) * 3);
	g_pVertexBuffer->Unlock();

	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, false);

	return S_OK;
}

HRESULT CALLBACK OnRender()
{
	// 기준 축 회전변환을 테스트합니다.
	//RotationBasisAxisTest(10.0f, 30.0f, -10.0f);

	// 오일러 각 회전변환을 테스트합니다.
	//RotationEulerAngleTest(10.0f, 30.0f, -10.0f);

	// 임의의 축 회전변환을 테스트합니다.
	//D3DXVECTOR3 vAxis(0.0f, 0.0f, 1.0f);
	//RotationFreeAxis(vAxis, -70.0f);

	// 쿼터니언 회전변환을 테스트합니다.
	// 쿼터니언 회전에는 임의의 축이 필요합니다.
	//RotationQuaternionAxisTest(vAxis, -70.0f);

	D3DDEVICE9->SetFVF(VertexP3D::format);
	D3DDEVICE9->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(VertexP3D));

	D3DDEVICE9->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	SAFE_RELEASE(g_pVertexBuffer);
	return S_OK;
}

// 오일러 각 회전과 맞추기 위해 Z -> X -> Y 순으로만 회전합니다.
void RotationBasisAxisTest(FLOAT rDegreeX, FLOAT rDegreeY, FLOAT rDegreeZ)
{
	D3DXMATRIXA16 matRotZ;
	D3DXMatrixIdentity(&matRotZ);
	D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(rDegreeZ));

	D3DXMATRIXA16 matRotX;
	D3DXMatrixIdentity(&matRotX);
	D3DXMatrixRotationX(&matRotX, D3DXToRadian(rDegreeX));

	D3DXMATRIXA16 matRotY;
	D3DXMatrixIdentity(&matRotY);
	D3DXMatrixRotationY(&matRotY, D3DXToRadian(rDegreeY));

	// Z * X * Y 순으로 곱합니다.
	// 오일러 각 회전의 기본형입니다.
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);
	matWorld = matRotZ * matRotX * matRotY;

	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matWorld);
}

void RotationEulerAngleTest(FLOAT rDegreeX, FLOAT rDegreeY, FLOAT rDegreeZ)
{
	D3DXMATRIXA16 matRot;
	D3DXMatrixIdentity(&matRot);

	// Yaw   -> Y축
	// Pitch -> X축
	// Roll  -> Z축
	//
	// 오른쪽 인자부터 전달되므로 Roll -> Pitch -> Yaw 순으로 회전됩니다.
	D3DXMatrixRotationYawPitchRoll(&matRot,
		D3DXToRadian(rDegreeY), D3DXToRadian(rDegreeX), D3DXToRadian(rDegreeZ));
	
	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matRot);
}

void RotationFreeAxis(const D3DXVECTOR3& vAxis, FLOAT rDegree)
{
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationAxis(&matRot, &vAxis, D3DXToRadian(rDegree));
	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matRot);
}

void RotationQuaternionAxisTest(const D3DXVECTOR3& vAxis, FLOAT rDegree)
{
	// 설정한 임의의 축으로 쿼터니언을 설정합니다.
	D3DXQUATERNION quat;
	D3DXQuaternionRotationAxis(&quat, &vAxis, D3DXToRadian(rDegree));

	// 쿼터니언을 행렬로 변환시킵니다.
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationQuaternion(&matRot, &quat);

	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matRot);
}
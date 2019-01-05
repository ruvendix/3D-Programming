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

void ScalingTest(FLOAT rX, FLOAT rY, FLOAT rZ);
void Rotation2DTest(FLOAT rDegree);
void RotationQuaternion2DTest(FLOAT rDegree);
void TranslationTest(FLOAT rX, FLOAT rY, FLOAT rZ);


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
	// 정점 순서는 왼손좌표계이므로 시계방향입니다.
	// 프로그램의 정중앙이 원점(0, 0, 0)이 되므로
	// 기본 카메라에 맞게 좌표를 설정해야 합니다.
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

	// rhw를 사용하지 않는다면 변환 이전의 공간좌표를 사용하게 되므로
	// 각종 변환 과정을 거쳐야 합니다. 조명(라이팅, Lighting)도 그중 하나인데
	// 조명에 관한 연산을 따로 하지 않았으므로 조명은 꺼줘야 합니다.
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, false);

	return S_OK;
}

HRESULT CALLBACK OnRender()
{
	// 크기변환을 테스트합니다.
	//ScalingTest(2.0f, 2.0f, 0.0f);

	// 2차원에서의 기준 축 회전변환을 테스트합니다.
	//Rotation2DTest(30.0f);

	// 2차원에서의 쿼터니언 회전변환을 테스트합니다.
	//RotationQuaternion2DTest(30.0f);

	// 이동변환을 테스트합니다.
	//TranslationTest(0.4f, 0.4f, 0.0f);

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

void ScalingTest(FLOAT rX, FLOAT rY, FLOAT rZ)
{
	D3DXMATRIXA16 matScale;
	D3DXMatrixScaling(&matScale, rX, rY, rZ);
	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matScale);
}

void Rotation2DTest(FLOAT rDegree)
{
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationZ(&matRot, -D3DXToRadian(rDegree)); // 각도를 라디안으로 변경해야 합니다.
	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matRot);
}

void RotationQuaternion2DTest(FLOAT rDegree)
{	
	// 먼저 임의의 축을 설정해줍니다.
	// 2D에서의 회전은 Z축 기준입니다.
	D3DXVECTOR3 vAxis(0.0f, 0.0f, 1.0f);

	// 설정한 임의의 축으로 쿼터니언을 설정합니다.
	D3DXQUATERNION quat;
	D3DXQuaternionRotationAxis(&quat, &vAxis,
		-D3DXToRadian(rDegree)); // 각도를 라디안으로 변경해야 합니다.

	// 쿼터니언을 행렬로 변환시킵니다.
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationQuaternion(&matRot, &quat);

	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matRot);
}

void TranslationTest(FLOAT rX, FLOAT rY, FLOAT rZ)
{
	D3DXMATRIXA16 matTrans;
	D3DXMatrixTranslation(&matTrans, rX, rY, rZ);
	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matTrans);
}
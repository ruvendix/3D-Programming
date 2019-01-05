#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


// ====================================================================================
// ���� ���� ������Դϴ�.
HRESULT g_DXResult = S_OK;

namespace
{
	IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
}


// ====================================================================================
// �Լ� ������Դϴ�.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

void ScalingTest(FLOAT rX, FLOAT rY, FLOAT rZ);
void Rotation2DTest(FLOAT rDegree);
void RotationQuaternion2DTest(FLOAT rDegree);
void TranslationTest(FLOAT rX, FLOAT rY, FLOAT rZ);


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
	RXMAIN_DX9->setSubFunc(OnRender,  SUBFUNC_TYPE::RENDER);
	RXMAIN_DX9->setSubFunc(OnRelease, SUBFUNC_TYPE::RELEASE);

	RXMAIN_DX9->RunMainRoutine(hInstance, IDI_RUVENDIX_ICO);
	return RXMAIN_DX9->getMessageCode();
}

HRESULT CALLBACK OnInit()
{
	// ���� ������ �޼���ǥ���̹Ƿ� �ð�����Դϴ�.
	// ���α׷��� ���߾��� ����(0, 0, 0)�� �ǹǷ�
	// �⺻ ī�޶� �°� ��ǥ�� �����ؾ� �մϴ�.
	VertexP3D vertices[3] =
	{
		{ {  0.0f,  0.2f , 0.0f }, DXCOLOR_RED   },
	    { {  0.2f, -0.2f , 0.0f }, DXCOLOR_GREEN },
		{ { -0.2f, -0.2f , 0.0f }, DXCOLOR_BLUE  },
	};

	g_DXResult = D3DDEVICE9->CreateVertexBuffer(sizeof(VertexP3D) * 3, D3DUSAGE_WRITEONLY,
		VertexP3D::format, D3DPOOL_MANAGED, &g_pVertexBuffer, nullptr);
	DXERR_HANDLER(g_DXResult);
	NULLCHK_RETURN_EFAIL(g_pVertexBuffer, "���� ���� �ʱ�ȭ ����!");
	
	void* pVertexData = nullptr;
	g_pVertexBuffer->Lock(0, sizeof(VertexP3D) * 3, &pVertexData, D3DLOCK_READONLY);
	::CopyMemory(pVertexData, vertices, sizeof(VertexP3D) * 3);
	g_pVertexBuffer->Unlock();

	// rhw�� ������� �ʴ´ٸ� ��ȯ ������ ������ǥ�� ����ϰ� �ǹǷ�
	// ���� ��ȯ ������ ���ľ� �մϴ�. ����(������, Lighting)�� ���� �ϳ��ε�
	// ���� ���� ������ ���� ���� �ʾ����Ƿ� ������ ����� �մϴ�.
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, false);

	return S_OK;
}

HRESULT CALLBACK OnRender()
{
	// ũ�⺯ȯ�� �׽�Ʈ�մϴ�.
	//ScalingTest(2.0f, 2.0f, 0.0f);

	// 2���������� ���� �� ȸ����ȯ�� �׽�Ʈ�մϴ�.
	//Rotation2DTest(30.0f);

	// 2���������� ���ʹϾ� ȸ����ȯ�� �׽�Ʈ�մϴ�.
	//RotationQuaternion2DTest(30.0f);

	// �̵���ȯ�� �׽�Ʈ�մϴ�.
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
	D3DXMatrixRotationZ(&matRot, -D3DXToRadian(rDegree)); // ������ �������� �����ؾ� �մϴ�.
	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matRot);
}

void RotationQuaternion2DTest(FLOAT rDegree)
{	
	// ���� ������ ���� �������ݴϴ�.
	// 2D������ ȸ���� Z�� �����Դϴ�.
	D3DXVECTOR3 vAxis(0.0f, 0.0f, 1.0f);

	// ������ ������ ������ ���ʹϾ��� �����մϴ�.
	D3DXQUATERNION quat;
	D3DXQuaternionRotationAxis(&quat, &vAxis,
		-D3DXToRadian(rDegree)); // ������ �������� �����ؾ� �մϴ�.

	// ���ʹϾ��� ��ķ� ��ȯ��ŵ�ϴ�.
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
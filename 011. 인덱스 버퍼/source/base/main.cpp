#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


// ====================================================================================
// ���� ���� ������Դϴ�.
HRESULT g_DXResult = S_OK;

namespace
{
	IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
	IDirect3DIndexBuffer9*  g_pIndexBuffer  = nullptr;

	std::vector<VertexP3D>  g_vecP3D;         // ���� �����Դϴ�.
	std::vector<Index16>    g_vecIndex16;     // �ε��� �����Դϴ�.
	D3DXVECTOR3             g_vBaseVertex[8]; // ���� ���� �����Դϴ�.

	// ������ ���ͷθ� ���Ǵµ� �̹����� FLOAT 3���� ���� ������ ���ϴ�.
	D3DXVECTOR3 g_rotateAngle;
}

// ====================================================================================
// �Լ� ������Դϴ�.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

// ť�긦 �����ϴ� �Լ��Դϴ�.
// ť��� �� 2���� ������ ������ �� �ֽ��ϴ�.
// ���ο��� �� 2���� ���� ���� �ڿ� �ּڰ��� �ּڰ����� �з��մϴ�.
void CreateCube(FLOAT rPoint1, FLOAT rPoint2);

// ������ �Ǵ� ���� 8���� �ֽ��ϴ�.
void InsertBaseVertex(FLOAT rPoint1, FLOAT rPoint2);

// ť�긦 �����ϱ� ���� ���� ������ �����մϴ�.
void InitOnlyCubeVertex(FLOAT rPoint1, FLOAT rPoint2);

// ť�긦 �����ϱ� ���� ���� ������ �ε��� ������ �����մϴ�.
void InitCubeVertexAndIndex(FLOAT rPoint1, FLOAT rPoint2);

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

	RXMAIN_DX9->RunMainRoutine(hInstance, IDI_RUVENDIX_ICO);
	return RXMAIN_DX9->getMessageCode();
}

HRESULT CALLBACK OnInit()
{
	CreateCube(-1.0f, 1.0f);

	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, false);
	D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	//D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);

	// ���� ũ�� �����Դϴ�.
	// FLOAT�� DWORD�� �ؼ��ؾ� �ϹǷ� DWORD*�� ������ �ڿ� ���� �����ؾ� �մϴ�.
	//FLOAT rPointSize = 20.0f;
	//D3DDEVICE9->SetRenderState(D3DRS_POINTSIZE, *reinterpret_cast<DWORD*>(&rPointSize));

	D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	//D3DDEVICE9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	
	// ==========================================================
	// ������� �����մϴ�.
	D3DXVECTOR3 vEye(4.0f, 4.0f, -4.0f);
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	D3DDEVICE9->SetTransform(D3DTS_VIEW, &matView);
	// ==========================================================
	// ��������� �����մϴ�.
	D3DXMATRIXA16 matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection, (D3DX_PI / 4.0f),
		(static_cast<FLOAT>(RXMAIN_DX9->getClientRect()->CalcWidth()) /
		                   (RXMAIN_DX9->getClientRect()->CalcHeight())), 1.0f, 1000.0f);
	D3DDEVICE9->SetTransform(D3DTS_PROJECTION, &matProjection);
	// ==========================================================

	return S_OK;
}

HRESULT CALLBACK OnUpdate()
{
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
	}

	// ���� ����
	g_rotateAngle.z = RX::AdjustAngle(g_rotateAngle.z);
	g_rotateAngle.x = RX::AdjustAngle(g_rotateAngle.x);
	g_rotateAngle.y = RX::AdjustAngle(g_rotateAngle.y);

	// ȸ������Դϴ�. ������ Z -> X -> Y�Դϴ�.
	// ��, Roll -> Pitch -> Yaw�Դϴ�.
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot,
		D3DXToRadian(g_rotateAngle.y), D3DXToRadian(g_rotateAngle.x),
		D3DXToRadian(g_rotateAngle.z));
	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matRot);

	return S_OK;
}

HRESULT CALLBACK OnRender()
{
	D3DDEVICE9->SetFVF(VertexP3D::format);
	D3DDEVICE9->SetStreamSource(
		0,
		g_pVertexBuffer,
		0,
		sizeof(VertexP3D));

	if (g_pIndexBuffer == nullptr)
	{
		// �������� ����(ť��� �ﰢ�� 12��)
		g_DXResult = D3DDEVICE9->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 12);
	}
	else
	{
		D3DDEVICE9->SetIndices(g_pIndexBuffer); // �ε��� ���� ����
		D3DDEVICE9->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST, // �ε��� ���۴� D3DPT_TRIANGLELIST�� ����
			0,   // ù �ε����� �� ���� ������ ���� �ε���
			0,   // ����� ù �ε���(�ε����� 0, 1, 2, 3�� ���� �� 3�̸� 3���� ����)
			8,   // ����� ���� ����(��ü ���� ���� - (2��° ���� + 3��° ����))
			0,   // �ε��� ������ ������
			12); // �������� ����(ť��� �ﰢ�� 12��)
	}

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	SAFE_RELEASE(g_pIndexBuffer);
	SAFE_RELEASE(g_pVertexBuffer);
	return S_OK;
}

void CreateCube(FLOAT rPoint1, FLOAT rPoint2)
{
	if (rPoint1 > rPoint2) // rPoint1�� �ּڰ�, rPoint2�� �ִ��Դϴ�.
	{
		std::swap(rPoint1, rPoint2); // �� ���� �ٲ��ݴϴ�.
	}

	InsertBaseVertex(rPoint1, rPoint2);
	InitOnlyCubeVertex(rPoint1, rPoint2);
	//InitCubeVertexAndIndex(rPoint1, rPoint2);

	INT32 vertexCnt = g_vecP3D.size();
	if (vertexCnt > 0)
	{
		g_DXResult = D3DDEVICE9->CreateVertexBuffer(sizeof(VertexP3D) * vertexCnt,
			D3DUSAGE_WRITEONLY, VertexP3D::format, D3DPOOL_MANAGED, &g_pVertexBuffer, nullptr);
		DXERR_HANDLER(g_DXResult);
		NULLCHK_RETURN(g_pVertexBuffer, "���� ���� ���� �����߽��ϴ�!");

		void* pVertexData = nullptr;
		g_pVertexBuffer->Lock(0, sizeof(VertexP3D) * vertexCnt,
			&pVertexData, D3DLOCK_READONLY);

		// ���͸� �̷��� ������ �� �ִµ� ǥ�� �����Դϴ�.
		// ��, ���ʹ� ���ӵ� �޸𸮶�� ������ �ִ� �ǵ�
		// ���� �Ҿ��ϴٸ� �迭�� �����ص� �������ϴ�.
		::CopyMemory(pVertexData, &g_vecP3D[0], sizeof(VertexP3D) * vertexCnt);

		g_pVertexBuffer->Unlock();
	}
	
	INT32 indexCnt = g_vecIndex16.size();
	if (indexCnt > 0)
	{
		g_DXResult = D3DDEVICE9->CreateIndexBuffer(
			sizeof(Index16) * indexCnt, // �ε��� ������ �뷮�Դϴ�.
			D3DUSAGE_WRITEONLY, // �����ε� 0�� �⺻������ ����ȭ�� ���� ���� �������� �����մϴ�.
			Index16::format,    // �ε��� ����
			D3DPOOL_MANAGED,    // �޸�Ǯ �����Դϴ�.
			&g_pIndexBuffer,    // �ε��� ������ �����͸� �ѱ�ϴ�.
			nullptr);           // nullptr�� �����մϴ�.
		DXERR_HANDLER(g_DXResult);
		NULLCHK_RETURN(g_pIndexBuffer, "�ε��� ���� ���� �����߽��ϴ�!");

		void* pIndexData = nullptr;
		g_pIndexBuffer->Lock(
			0, // ������(Offset), �� ���� ��ġ�� �ǹ��ϴµ� ��ü ����� 0�Դϴ�.
			sizeof(Index16) * indexCnt, // ������ �ε��� ������ �뷮�� �Ѱ��ݴϴ�.
			&pIndexData, // ����� ���� ������ �ٷ� �� �ִ� �����͸� �������ݴϴ�.
			D3DLOCK_READONLY); // ��� �÷��״� D3DLOCK_READONLY�� �����մϴ�.

		// ���͸� �̷��� ������ �� �ִµ� ǥ�� �����Դϴ�.
		// ��, ���ʹ� ���ӵ� �޸𸮶�� ������ �ִ� �ǵ�
		// ���� �Ҿ��ϴٸ� �迭�� �����ص� �������ϴ�.
		::CopyMemory(pIndexData, &g_vecIndex16[0], sizeof(Index16) * indexCnt);

		g_pIndexBuffer->Unlock();
	}
}

void InsertBaseVertex(FLOAT rPoint1, FLOAT rPoint2)
{
	// �� �������� ���̸� ���մϴ�.
	FLOAT rDistance = rPoint2 - rPoint1;

	// ===============================================
	// ť�긦 �������Ϸ��� �ﰢ�� 12���� �ʿ��մϴ�.
	// �� �ﰢ���� ���ϱ� ���ؼ��� ���� 8���� �ʿ��մϴ�.
	// ���� ���� 8������ ���س��� �� ���մϴ�.
	// �� �κ��� �ּ����� ǥ���س����Կ�.
	// ===============================================
	::ZeroMemory(g_vBaseVertex, sizeof(D3DXVECTOR3) * _countof(g_vBaseVertex));

	// ���� ���� 4��
	RX::SetVector(&g_vBaseVertex[0], rPoint2 - rDistance, rPoint2, rPoint2 - rDistance); // 0
	RX::SetVector(&g_vBaseVertex[1], rPoint2, rPoint2, rPoint2 - rDistance);             // 1
	RX::SetVector(&g_vBaseVertex[2], rPoint2, rPoint2, rPoint2);                         // 2
	RX::SetVector(&g_vBaseVertex[3], rPoint2 - rDistance, rPoint2, rPoint2);             // 3

	// �Ʒ��� ���� 4��
	RX::SetVector(&g_vBaseVertex[4], rPoint1, rPoint1, rPoint1);                         // 4
	RX::SetVector(&g_vBaseVertex[5], rPoint1 + rDistance, rPoint1, rPoint1);             // 5
	RX::SetVector(&g_vBaseVertex[6], rPoint1 + rDistance, rPoint1, rPoint1 + rDistance); // 6
	RX::SetVector(&g_vBaseVertex[7], rPoint1, rPoint1, rPoint1 + rDistance);             // 7
}

// ���� 36�� �����մϴ�.
// 3 * 2 * 6 = 36
// �� ���� �������ϱ� ���� �ﰢ�� 2���� �ʿ��ϰ�
// �ﰢ�� 2���� ���� 6���Դϴ�. ���� 6���̹Ƿ� �ʿ��� ������ 36��!
void InitOnlyCubeVertex(FLOAT rPoint1, FLOAT rPoint2)
{
	// ===============================================
	// ���� �ﰢ�� 2��
	VertexP3D vertex;
	vertex.vPos    = g_vBaseVertex[0];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[1];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[5];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);
	// -----------------------------------------------
	vertex.vPos    = g_vBaseVertex[0];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[5];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[4];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);
	// ===============================================
	// ���� ���� �ﰢ�� 2��
	vertex.vPos    = g_vBaseVertex[0];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[4];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[3];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);
	// -----------------------------------------------
	vertex.vPos    = g_vBaseVertex[3];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[4];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[7];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);
	// ===============================================
	// ������ ���� �ﰢ�� 2��
	vertex.vPos    = g_vBaseVertex[1];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[2];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[5];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);
	// -----------------------------------------------
	vertex.vPos    = g_vBaseVertex[2];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[6];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[5];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);
	// ===============================================
	// ���� �ﰢ�� 2��
	vertex.vPos    = g_vBaseVertex[0];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[3];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[2];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);
	// -----------------------------------------------
	vertex.vPos    = g_vBaseVertex[0];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[2];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[1];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);
	// ===============================================
	// �Ʒ��� �ﰢ�� 2��
	vertex.vPos    = g_vBaseVertex[4];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[6];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[7];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);
	// -----------------------------------------------
	vertex.vPos    = g_vBaseVertex[4];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[5];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[6];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);
	// ===============================================
	// �޸� �ﰢ�� 2��
	vertex.vPos    = g_vBaseVertex[3];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[6];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[2];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);
	// -----------------------------------------------
	vertex.vPos    = g_vBaseVertex[3];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[7];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[6];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	// ���� ���۷� ť�� �ϳ��� �������ϱ� ���� ���� 36���� �ʿ��մϴ�.
	// �ߺ��Ǵ� ������ �����Ƿ� ����ȭ�� �ʿ��մϴ�.
}

// ���� 8���� �����մϴ�.
// �ε����� �̿��ؼ� �������� ���̹Ƿ� �ʿ��� �ε����� 36��!
void InitCubeVertexAndIndex(FLOAT rPoint1, FLOAT rPoint2)
{
	// ===============================================
	// �ε����� ���� ���� 8���� �˴ϴ�.
	VertexP3D vertex;
	vertex.vPos    = g_vBaseVertex[0];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[1];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[2];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[3];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[4];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[5];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[6];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[7];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);

	// ===============================================
	// �ε��� �����Դϴ�.
	// ���� �ﰢ�� 2��
	Index16 index;
	index.index = 0;
	g_vecIndex16.push_back(index);

	index.index = 1;
	g_vecIndex16.push_back(index);

	index.index = 5;
	g_vecIndex16.push_back(index);
	// -----------------------------------------------
	index.index = 0;
	g_vecIndex16.push_back(index);

	index.index = 5;
	g_vecIndex16.push_back(index);

	index.index = 4;
	g_vecIndex16.push_back(index);
	// ===============================================
	// ���� ���� �ﰢ�� 2��	
	index.index = 0;
	g_vecIndex16.push_back(index);

	index.index = 4;
	g_vecIndex16.push_back(index);

	index.index = 3;
	g_vecIndex16.push_back(index);
	// -----------------------------------------------
	index.index = 3;
	g_vecIndex16.push_back(index);

	index.index = 4;
	g_vecIndex16.push_back(index);

	index.index = 7;
	g_vecIndex16.push_back(index);
	// ===============================================
	// ������ ���� �ﰢ�� 2��
	index.index = 1;
	g_vecIndex16.push_back(index);

	index.index = 2;
	g_vecIndex16.push_back(index);

	index.index = 5;
	g_vecIndex16.push_back(index);
	// -----------------------------------------------
	index.index = 2;
	g_vecIndex16.push_back(index);

	index.index = 6;
	g_vecIndex16.push_back(index);

	index.index = 5;
	g_vecIndex16.push_back(index);
	// ===============================================
	// ���� �ﰢ�� 2��
	index.index = 0;
	g_vecIndex16.push_back(index);

	index.index = 3;
	g_vecIndex16.push_back(index);

	index.index = 2;
	g_vecIndex16.push_back(index);
	// -----------------------------------------------
	index.index = 0;
	g_vecIndex16.push_back(index);

	index.index = 2;
	g_vecIndex16.push_back(index);

	index.index = 1;
	g_vecIndex16.push_back(index);
	// ===============================================
	// �Ʒ��� �ﰢ�� 2��
	index.index = 4;
	g_vecIndex16.push_back(index);

	index.index = 6;
	g_vecIndex16.push_back(index);

	index.index = 7;
	g_vecIndex16.push_back(index);
	// -----------------------------------------------
	index.index = 4;
	g_vecIndex16.push_back(index);

	index.index = 5;
	g_vecIndex16.push_back(index);

	index.index = 6;
	g_vecIndex16.push_back(index);
	// ===============================================
	// �޸� �ﰢ�� 2��
	index.index = 3;
	g_vecIndex16.push_back(index);

	index.index = 6;
	g_vecIndex16.push_back(index);

	index.index = 2;
	g_vecIndex16.push_back(index);
	// -----------------------------------------------
	index.index = 3;
	g_vecIndex16.push_back(index);

	index.index = 7;
	g_vecIndex16.push_back(index);

	index.index = 6;
	g_vecIndex16.push_back(index);

	// �ε��� ���۷� ť�� �ϳ��� �������ϱ� ���� ���� 8���� �ʿ��մϴ�.
	// ���� 8���� �̿��ؼ� �׸��� ������ �ε����� �־��ָ� �˴ϴ�.
	// �ε��� ���۴� D3DPT_TRIANGLELIST�� �⺻�̰� ������ ������ ��������� �մϴ�.
	// ť�� �ϳ��� �������ϱ� ���ؼ��� �������� �ּ� 12�� �ʿ��մϴ�.
}
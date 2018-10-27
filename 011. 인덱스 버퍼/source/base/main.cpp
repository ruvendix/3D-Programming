#include "base_project.h"
#include "global_variable_declaration.h"
#include "main.h"
#include "RX\RXCubeDX9.h"

// ====================================================================================
// ��ũ�� ���Ǻ��Դϴ�.


// ====================================================================================
// ����ü �� ����ü ������Դϴ�.

// ���� �����Դϴ�.
struct CustomVertex
{
	D3DXVECTOR3 vPos;    // ��ġ
	DWORD       dwColor; // ����
	const static DWORD format = D3DFVF_XYZ | D3DFVF_DIFFUSE; // ����
};

// �ε��� �����Դϴ�.
struct CustomIndex
{
	WORD index; // �ε�����
	const static D3DFORMAT format = D3DFMT_INDEX16; // ������ 2����Ʈ�� ����մϴ�.
};

// ====================================================================================
// ���� ���� ������Դϴ�.
namespace
{
	RX::RXMain_DX9*           g_pMainDX       = nullptr;
	IDirect3DVertexBuffer9*   g_pVertexBuffer = nullptr;
	IDirect3DIndexBuffer9*    g_pIndexBuffer  = nullptr;
	D3DXVECTOR3               g_vBaseVertex[8];
	std::vector<CustomVertex> g_vecVertexData;
	std::vector<CustomIndex>  g_vecIndexData;
}

extern IDirect3DDevice9* g_pD3DDevice9 = nullptr;
extern HRESULT           g_DXResult    = S_OK;


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

	g_pMainDX = RXNew RX::RXMain_DX9;
	NULLCHK(g_pMainDX);

	g_pMainDX->setSubFunc(OnInit,    SUBFUNC_TYPE::INIT);
	g_pMainDX->setSubFunc(OnUpdate,  SUBFUNC_TYPE::UPDATE);
	g_pMainDX->setSubFunc(OnRender,  SUBFUNC_TYPE::RENDER);
	g_pMainDX->setSubFunc(OnRelease, SUBFUNC_TYPE::RELEASE);

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

	CreateCube(-1.0f, 1.0f);

	// rhw�� ������� �ʴ´ٸ� ��ȯ ������ ������ǥ�� ����ϰ� �ǹǷ�
	// ���� ��ȯ ������ ���ľ� �մϴ�. ����(������, Lighting)�� ���� �ϳ��ε�
	// ���� ���� ������ ���� ���� �ʾ����Ƿ� ������ ����� �մϴ�.
	g_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, false);

	// ������ ��带 �����մϴ�. ����Ʈ�� �ָ����Դϴ�.
	// ť�긦 Ȯ���ϱ� ���ؼ��� ���̾������� ���� �ٲ�� �մϴ�.
	g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	//g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);

	// ���� ũ�� �����Դϴ�.
	// FLOAT�� DWORD�� �ؼ��ؾ� �ϹǷ� DWORD*�� ������ �ڿ� ���� �����ؾ� �մϴ�.
	//FLOAT rPointSize = 20.0f;
	//g_pD3DDevice9->SetRenderState(D3DRS_POINTSIZE, *reinterpret_cast<DWORD*>(&rPointSize));
		
	// �ø� ��带 �����մϴ�. ����Ʈ�� �ݽð���� �ø��Դϴ�.
	// ť�긦 Ȯ���ϱ� ���ؼ��� �ø� ��带 �����ؾ� �մϴ�.
	g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	//g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// ������� �����մϴ�.
	D3DXVECTOR3 vEye(4.0f, 4.0f, -4.0f);   // ī�޶��� ��ġ
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // ī�޶� ���� ����
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // ī�޶��� �� ����

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	g_pD3DDevice9->SetTransform(D3DTS_VIEW, &matView);

	// ��������� �����մϴ�.
	D3DXMATRIXA16 matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection, (D3DX_PI / 4.0f),
		(g_pMainDX->getClientWidth() / g_pMainDX->getClientHeight()),
		1.0f, 1000.0f);
	g_pD3DDevice9->SetTransform(D3DTS_PROJECTION, &matProjection);

	return S_OK;
}

// ������Ʈ �Լ��Դϴ�.
// �������� ������ �ְų� ���α׷��� ������ �ִ�
// �������� �������Ӹ��� ������Ʈ�մϴ�.
HRESULT CALLBACK OnUpdate()
{
	static FLOAT rAngleZ = 0.0f;
	static FLOAT rAngleX = 0.0f;
	static FLOAT rAngleY = 0.0f;

	if (::GetAsyncKeyState('A'))
	{
		rAngleZ += 4.0f;
	}

	if (::GetAsyncKeyState('D'))
	{
		rAngleZ -= 4.0f;
	}

	if (::GetAsyncKeyState('W'))
	{
		rAngleX += 4.0f;
	}

	if (::GetAsyncKeyState('S'))
	{
		rAngleX -= 4.0f;
	}

	if (::GetAsyncKeyState('Q'))
	{
		rAngleY += 4.0f;
	}

	if (::GetAsyncKeyState('E'))
	{
		rAngleY -= 4.0f;
	}

	if (::GetAsyncKeyState('R'))
	{
		rAngleZ = 0.0f;
		rAngleX = 0.0f;
		rAngleY = 0.0f;
	}

	// ���� ����
	rAngleZ = RX::AdjustAngle(rAngleZ);
	rAngleX = RX::AdjustAngle(rAngleX);
	rAngleY = RX::AdjustAngle(rAngleY);

	// ȸ������Դϴ�. ������ Z -> X -> Y�Դϴ�.
	// ��, Roll -> Pitch -> Yaw�Դϴ�.
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot,
		D3DXToRadian(rAngleY), D3DXToRadian(rAngleX), D3DXToRadian(rAngleZ));
	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &matRot);

	return S_OK;
}

// ������ �Լ��Դϴ�.
// ���� ������ �۾��� Draw Call�� ó���˴ϴ�.
// Draw Call�� �����Ӵ� ȣ��Ǵ� ������ �Լ��� ���ϴµ� ȣ��Ǵ� �󵵼���
// �����ϸ� Draw Call Count�� �˾Ƴ� �� �ֽ��ϴ�.
HRESULT CALLBACK OnRender()
{
	g_pD3DDevice9->SetFVF(CustomVertex::format); // ���� ���� ����
	g_pD3DDevice9->SetStreamSource(
		0,                     // ����� ��Ʈ�� �ε���
		g_pVertexBuffer,       // ������ ���� ����
		0,                     // ���� ������ ������
		sizeof(CustomVertex)); // ���� �뷮

	if (g_pIndexBuffer == nullptr)
	{
		g_DXResult = g_pD3DDevice9->DrawPrimitive(
			D3DPT_TRIANGLELIST, // �������� �⺻ ����
			0,   // �������� ������ ���� ��ȣ,
			12); // �������� ����(ť��� �ﰢ�� 12��)
	}
	else
	{
		g_pD3DDevice9->SetIndices(g_pIndexBuffer); // �ε��� ���� ����
		g_pD3DDevice9->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST, // �ε��� ���۴� Ʈ���̾ޱ۸���Ʈ�� ����
			0,   // ù �ε����� �� ���� ������ ���� �ε���
			0,   // ����� ù �ε���(�ε����� 0, 1, 2, 3�� ���� �� 3�̸� 3���� ����)
			8,   // ����� ���� ����(2��° ���� + 3��° ���ڸ�ŭ �� ��)
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

	INT32 vertexCnt = g_vecVertexData.size();
	if (vertexCnt > 0)
	{
		g_DXResult = g_pD3DDevice9->CreateVertexBuffer(
			sizeof(CustomVertex) * vertexCnt, // ���� �� �뷮
			D3DUSAGE_NONE,        // ���� ������ �뵵
			CustomVertex::format, // ���� ����
			D3DPOOL_MANAGED,      // ���ϴ� �޸�Ǯ
			&g_pVertexBuffer,     // ���� ������ �ּ�
			nullptr);             // �� ���� �Ķ����
		DXERR_HANDLER(g_DXResult);
		NULLCHK_RETURN(g_pVertexBuffer, "���� ���� ���� �����߽��ϴ�!");

		void* pVertexData = nullptr;
		g_pVertexBuffer->Lock(
			0, // ���� ������ ������
			sizeof(CustomVertex) * vertexCnt, // ���� �� ���� �� �뷮
			&pVertexData,  // ���� �޸��� ��ġ
			D3DFLAG_NONE); // �� �÷���

		// ���͸� �̷��� ������ �� �ִµ� ǥ�� �����Դϴ�.
		// ��, ���ʹ� ���ӵ� �޸𸮶�� ������ �ִ� �ǵ�
		// ���� �Ҿ��ϴٸ� �迭�� �����ص� �������ϴ�.
		::CopyMemory(pVertexData, &g_vecVertexData[0], sizeof(CustomVertex) * vertexCnt);

		g_pVertexBuffer->Unlock();
	}
	
	INT32 indexCnt = g_vecIndexData.size();
	if (indexCnt > 0)
	{
		g_DXResult = g_pD3DDevice9->CreateIndexBuffer(
			sizeof(CustomIndex) * indexCnt, // �ε��� �� �뷮
			D3DUSAGE_NONE,       // �ε��� ������ �뵵
			CustomIndex::format, // �ε��� ����
			D3DPOOL_MANAGED,     // ���ϴ� �޸�Ǯ
			&g_pIndexBuffer,     // �ε��� ������ �ּ�
			nullptr);            // �� ���� �Ķ����
		DXERR_HANDLER(g_DXResult);
		NULLCHK_RETURN(g_pIndexBuffer, "�ε��� ���� ���� �����߽��ϴ�!");

		void* pIndexData = nullptr;
		g_pIndexBuffer->Lock(
			0, // ���� ������ ������
			sizeof(CustomIndex) * indexCnt, // ���� �� �ε��� �� �뷮
			&pIndexData,   // ���� �޸��� ��ġ
			D3DFLAG_NONE); // �� �÷���

		// ���͸� �̷��� ������ �� �ִµ� ǥ�� �����Դϴ�.
		// ��, ���ʹ� ���ӵ� �޸𸮶�� ������ �ִ� �ǵ�
		// ���� �Ҿ��ϴٸ� �迭�� �����ص� �������ϴ�.
		::CopyMemory(pIndexData, &g_vecIndexData[0], sizeof(CustomIndex) * indexCnt);

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
	CustomVertex vertex;
	vertex.vPos    = g_vBaseVertex[0];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[1];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[5];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecVertexData.push_back(vertex);
	// -----------------------------------------------
	vertex.vPos    = g_vBaseVertex[0];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[5];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[4];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);
	// ===============================================
	// ���� ���� �ﰢ�� 2��
	vertex.vPos    = g_vBaseVertex[0];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[4];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[3];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);
	// -----------------------------------------------
	vertex.vPos    = g_vBaseVertex[3];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[4];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[7];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);
	// ===============================================
	// ������ ���� �ﰢ�� 2��
	vertex.vPos    = g_vBaseVertex[1];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[2];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[5];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecVertexData.push_back(vertex);
	// -----------------------------------------------
	vertex.vPos    = g_vBaseVertex[2];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[6];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[5];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecVertexData.push_back(vertex);
	// ===============================================
	// ���� �ﰢ�� 2��
	vertex.vPos    = g_vBaseVertex[0];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[3];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[2];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecVertexData.push_back(vertex);
	// -----------------------------------------------
	vertex.vPos    = g_vBaseVertex[0];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[2];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[1];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);
	// ===============================================
	// �Ʒ��� �ﰢ�� 2��
	vertex.vPos    = g_vBaseVertex[4];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[6];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[7];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);
	// -----------------------------------------------
	vertex.vPos    = g_vBaseVertex[4];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[5];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[6];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);
	// ===============================================
	// �޸� �ﰢ�� 2��
	vertex.vPos    = g_vBaseVertex[3];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[6];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[2];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecVertexData.push_back(vertex);
	// -----------------------------------------------
	vertex.vPos    = g_vBaseVertex[3];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[7];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[6];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	// ���� ���۷� ť�� �ϳ��� �������ϱ� ���� ���� 36���� �ʿ��մϴ�.
	// �ߺ��Ǵ� ������ �����Ƿ� ����ȭ�� �ʿ��մϴ�.
}

// ���� 8���� �����մϴ�.
// �ε����� �̿��ؼ� �������� ���̹Ƿ� �ʿ��� �ε����� 36��!
void InitCubeVertexAndIndex(FLOAT rPoint1, FLOAT rPoint2)
{
	// ===============================================
	// �ε����� ���� ���� 8���� �˴ϴ�.
	CustomVertex vertex;
	vertex.vPos    = g_vBaseVertex[0];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[1];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[2];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[3];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[4];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[5];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[6];
	vertex.dwColor = DXCOLOR_RED;
	g_vecVertexData.push_back(vertex);

	vertex.vPos    = g_vBaseVertex[7];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecVertexData.push_back(vertex);

	// ===============================================
	// �ε��� �����Դϴ�.
	// ���� �ﰢ�� 2��
	CustomIndex index;
	index.index = 0;
	g_vecIndexData.push_back(index);

	index.index = 1;
	g_vecIndexData.push_back(index);

	index.index = 5;
	g_vecIndexData.push_back(index);
	// -----------------------------------------------
	index.index = 0;
	g_vecIndexData.push_back(index);

	index.index = 5;
	g_vecIndexData.push_back(index);

	index.index = 4;
	g_vecIndexData.push_back(index);
	// ===============================================
	// ���� ���� �ﰢ�� 2��	
	index.index = 0;
	g_vecIndexData.push_back(index);

	index.index = 4;
	g_vecIndexData.push_back(index);

	index.index = 3;
	g_vecIndexData.push_back(index);
	// -----------------------------------------------
	index.index = 3;
	g_vecIndexData.push_back(index);

	index.index = 4;
	g_vecIndexData.push_back(index);

	index.index = 7;
	g_vecIndexData.push_back(index);
	// ===============================================
	// ������ ���� �ﰢ�� 2��
	index.index = 1;
	g_vecIndexData.push_back(index);

	index.index = 2;
	g_vecIndexData.push_back(index);

	index.index = 5;
	g_vecIndexData.push_back(index);
	// -----------------------------------------------
	index.index = 2;
	g_vecIndexData.push_back(index);

	index.index = 6;
	g_vecIndexData.push_back(index);

	index.index = 5;
	g_vecIndexData.push_back(index);
	// ===============================================
	// ���� �ﰢ�� 2��
	index.index = 0;
	g_vecIndexData.push_back(index);

	index.index = 3;
	g_vecIndexData.push_back(index);

	index.index = 2;
	g_vecIndexData.push_back(index);
	// -----------------------------------------------
	index.index = 0;
	g_vecIndexData.push_back(index);

	index.index = 2;
	g_vecIndexData.push_back(index);

	index.index = 1;
	g_vecIndexData.push_back(index);
	// ===============================================
	// �Ʒ��� �ﰢ�� 2��
	index.index = 4;
	g_vecIndexData.push_back(index);

	index.index = 6;
	g_vecIndexData.push_back(index);

	index.index = 7;
	g_vecIndexData.push_back(index);
	// -----------------------------------------------
	index.index = 4;
	g_vecIndexData.push_back(index);

	index.index = 5;
	g_vecIndexData.push_back(index);

	index.index = 6;
	g_vecIndexData.push_back(index);
	// ===============================================
	// �޸� �ﰢ�� 2��
	index.index = 3;
	g_vecIndexData.push_back(index);

	index.index = 6;
	g_vecIndexData.push_back(index);

	index.index = 2;
	g_vecIndexData.push_back(index);
	// -----------------------------------------------
	index.index = 3;
	g_vecIndexData.push_back(index);

	index.index = 7;
	g_vecIndexData.push_back(index);

	index.index = 6;
	g_vecIndexData.push_back(index);

	// �ε��� ���۷� ť�� �ϳ��� �������ϱ� ���� ���� 8���� �ʿ��մϴ�.
	// ���� 8���� �̿��ؼ� �׸��� ������ �ε����� �־��ָ� �˴ϴ�.
	// �ε��� ���۴� D3DPT_TRIANGLELIST�� �⺻�̰� ������ ������ ��������� �մϴ�.
	// ť�� �ϳ��� �������ϱ� ���ؼ��� �������� �ּ� 12�� �ʿ��մϴ�.
}
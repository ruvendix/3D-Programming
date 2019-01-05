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

	std::vector<VertexP3D> g_vecP3D;
	std::vector<Index16>   g_vecIndex16;
	D3DXVECTOR3            g_vBaseVertex[8];
	D3DXVECTOR3            g_vCubeTriangleNormal[12];

	// ������ ���� �ݻ� �� ����� �����ϴ� �����Դϴ�.
	// ����(0.0f)���� ���(1.0f)���� ������ ������ ������ �� �ֽ��ϴ�.
	D3DXMATERIAL g_mtrl;
}


// ====================================================================================
// �Լ� ������Դϴ�.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();


void InputKeyboard();
void CalcTriangleNormal();
void CreateCube(FLOAT rPoint1, FLOAT rPoint2);
void InsertBaseVertex(FLOAT rPoint1, FLOAT rPoint2);
void InitCubeVertexAndIndex(FLOAT rPoint1, FLOAT rPoint2);
void AdjustColorRange(D3DCOLORVALUE* pColor);

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
	
	// ==========================================================
	// ������� �����մϴ�.
	D3DXVECTOR3 vEye(4.0f, 4.0f, -4.0f);   // ī�޶��� ��ġ
	D3DXVECTOR3 vLookAt(0.0f, 0.0f, 0.0f); // ī�޶� ���� ����
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // ī�޶��� �� ����

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

	CalcTriangleNormal();

	RX::ShowMouseCursor(true);

	// rhw�� ������� �ʴ´ٸ� ��ȯ ������ ������ǥ��
	// ����ϰ� �ǹǷ� ���� ��ȯ ������ ���ľ� �մϴ�.
	// ����(������, Lighting)�� ���� �ϳ��ε�
	// �̹����� ������ ����ϹǷ� ������ ���ݴϴ�.
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, true);

	// ������ Ȯ���ϱ� ���� ���� �ֺ����� �����մϴ�.
	D3DDEVICE9->SetRenderState(D3DRS_AMBIENT, DXCOLOR_WHITE);

	// ������ �ʱ�ȭ�մϴ�.
	::ZeroMemory(&g_mtrl, sizeof(g_mtrl));

	return S_OK;
}

// ������Ʈ �Լ��Դϴ�.
// �������� ������ �ְų� ���α׷��� ������ �ִ�
// �������� �������Ӹ��� ������Ʈ�մϴ�.
HRESULT CALLBACK OnUpdate()
{
	InputKeyboard();
	AdjustColorRange(&g_mtrl.MatD3D.Ambient);

	return S_OK;
}

HRESULT CALLBACK OnRender()
{
	// ������ ����մϴ�.
	// ������ �ϳ��� ��� �����մϴ�.
	// ���� ������ ���� �ٲٸ鼭 �������ؾ� �մϴ�.
	D3DDEVICE9->SetMaterial(&g_mtrl.MatD3D);

	D3DDEVICE9->SetFVF(VertexP3D::format);
	D3DDEVICE9->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(VertexP3D));
	D3DDEVICE9->SetIndices(g_pIndexBuffer);

	D3DDEVICE9->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	SAFE_RELEASE(g_pIndexBuffer);
	SAFE_RELEASE(g_pVertexBuffer);
	return S_OK;
}

void InputKeyboard()
{
	static FLOAT rAngleZ = 0.0f;
	static FLOAT rAngleX = 0.0f;
	static FLOAT rAngleY = 0.0f;

	FLOAT rDeltaSeconds = RX::RXProgramFPSMgr::Instance()->getTimer()->getDeltaSeconds();
	if (::GetAsyncKeyState('A') & 0x8000)
	{
		rAngleZ += 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('D') & 0x8000)
	{
		rAngleZ -= 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('W') & 0x8000)
	{
		rAngleX += 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('S') & 0x8000)
	{
		rAngleX -= 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('Q') & 0x8000)
	{
		rAngleY += 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('E') & 0x8000)
	{
		rAngleY -= 180.0f * rDeltaSeconds;
	}

	if (::GetAsyncKeyState('R') & 0x8000)
	{
		rAngleZ = 0.0f;
		rAngleX = 0.0f;
		rAngleY = 0.0f;

		g_mtrl.MatD3D.Ambient.a = 1.0f;
		g_mtrl.MatD3D.Ambient.r = 1.0f;
		g_mtrl.MatD3D.Ambient.g = 1.0f;
		g_mtrl.MatD3D.Ambient.b = 1.0f;

		D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	rAngleZ = RX::AdjustAngle(rAngleZ);
	rAngleX = RX::AdjustAngle(rAngleX);
	rAngleY = RX::AdjustAngle(rAngleY);

	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot,
		D3DXToRadian(rAngleY), D3DXToRadian(rAngleX), D3DXToRadian(rAngleZ));
	D3DDEVICE9->SetTransform(D3DTS_WORLD, &matRot);

	if (::GetAsyncKeyState('Z') & 0x8000)
	{
		D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	if (::GetAsyncKeyState('X') & 0x8000)
	{
		D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}

	// ====================================
	// ����
	if (::GetAsyncKeyState('T') & 0x8000)
	{
		g_mtrl.MatD3D.Ambient.r += 0.05f;
	}

	if (::GetAsyncKeyState('Y') & 0x8000)
	{
		g_mtrl.MatD3D.Ambient.r -= 0.05f;
	}
	// ====================================
	// �ʷ�
	if (::GetAsyncKeyState('U') & 0x8000)
	{
		g_mtrl.MatD3D.Ambient.g += 0.05f;
	}

	if (::GetAsyncKeyState('I') & 0x8000)
	{
		g_mtrl.MatD3D.Ambient.g -= 0.05f;
	}
	// ====================================
	// �Ķ�
	if (::GetAsyncKeyState('O') & 0x8000)
	{
		g_mtrl.MatD3D.Ambient.b += 0.05f;
	}

	if (::GetAsyncKeyState('P') & 0x8000)
	{
		g_mtrl.MatD3D.Ambient.b -= 0.05f;
	}
	// ====================================
}

// ������ ������ �������ִ� �Լ��Դϴ�.
void AdjustColorRange(D3DCOLORVALUE* pColor)
{
	if (pColor == nullptr)
	{
		return;
	}

	if (pColor->a < 0.0f)
	{
		pColor->a = 0.0f;
	}
	else if (pColor->a > 1.0f)
	{
		pColor->a = 1.0f;
	}

	if (pColor->r < 0.0f)
	{
		pColor->r = 0.0f;
	}
	else if (pColor->r > 1.0f)
	{
		pColor->r = 1.0f;
	}

	if (pColor->g < 0.0f)
	{
		pColor->g = 0.0f;
	}
	else if (pColor->g > 1.0f)
	{
		pColor->g = 1.0f;
	}

	if (pColor->b < 0.0f)
	{
		pColor->b = 0.0f;
	}
	else if (pColor->b > 1.0f)
	{
		pColor->b = 1.0f;
	}
}

#pragma region ť�� ���� �� �������� ���ϴ� ����
void CalcTriangleNormal()
{
	::ZeroMemory(g_vCubeTriangleNormal, sizeof(D3DXVECTOR3) * 12);

	INT32 index = -1;
	for (INT32 i = 0; i < 12; ++i)
	{
		// �ε��� ���ۿ� ���ϴ� ������ ������� �������Ƿ�
		// �ε����� �ϳ��� ������Ű�� ���ϴ� ������ �˾Ƴ� �� �ֽ��ϴ�.
		// ���� ������ ������ ���� 2���� �˾Ƴ��� �������͸� ���� �� �ֽ��ϴ�.
		D3DXVECTOR3 v1 = g_vBaseVertex[g_vecIndex16[++index].index];
		D3DXVECTOR3 v2 = g_vBaseVertex[g_vecIndex16[++index].index];
		D3DXVECTOR3 v3 = g_vBaseVertex[g_vecIndex16[++index].index];
		g_vCubeTriangleNormal[i] = RX::CalcNormalVector(v1, v2, v3);
	}
}

void CreateCube(FLOAT rPoint1, FLOAT rPoint2)
{
	if (rPoint1 > rPoint2)
	{
		std::swap(rPoint1, rPoint2);
	}

	InsertBaseVertex(rPoint1, rPoint2);
	InitCubeVertexAndIndex(rPoint1, rPoint2);

	INT32 vertexCnt = g_vecP3D.size();
	g_DXResult = D3DDEVICE9->CreateVertexBuffer(sizeof(VertexP3D) * vertexCnt, D3DUSAGE_WRITEONLY,
		VertexP3D::format, D3DPOOL_MANAGED, &g_pVertexBuffer, nullptr);
	DXERR_HANDLER(g_DXResult);
	NULLCHK_RETURN(g_pVertexBuffer, "���� ���� ���� �����߽��ϴ�!");

	void* pVertexData = nullptr;
	g_pVertexBuffer->Lock(0, sizeof(VertexP3D) * vertexCnt, &pVertexData, D3DLOCK_READONLY);
	::CopyMemory(pVertexData, &g_vecP3D[0], sizeof(VertexP3D) * vertexCnt);
	g_pVertexBuffer->Unlock();

	// ================================================================================

	INT32 indexCnt = g_vecIndex16.size();
	g_DXResult = D3DDEVICE9->CreateIndexBuffer(sizeof(Index16) * indexCnt, D3DUSAGE_WRITEONLY,
		Index16::format, D3DPOOL_MANAGED, &g_pIndexBuffer, nullptr);
	DXERR_HANDLER(g_DXResult);
	NULLCHK_RETURN(g_pIndexBuffer, "�ε��� ���� ���� �����߽��ϴ�!");

	void* pIndexData = nullptr;
	g_pIndexBuffer->Lock(0, sizeof(Index16) * indexCnt, &pIndexData, D3DLOCK_READONLY);
	::CopyMemory(pIndexData, &g_vecIndex16[0], sizeof(Index16) * indexCnt);
	g_pIndexBuffer->Unlock();
}

void InsertBaseVertex(FLOAT rPoint1, FLOAT rPoint2)
{
	FLOAT rDistance = rPoint2 - rPoint1;

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

void InitCubeVertexAndIndex(FLOAT rPoint1, FLOAT rPoint2)
{
	// ===============================================
	// �ε����� ���� ���� 8���� �˴ϴ�.
	VertexP3D vertex;
	vertex.vPos = g_vBaseVertex[0];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos = g_vBaseVertex[1];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);

	vertex.vPos = g_vBaseVertex[2];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);

	vertex.vPos = g_vBaseVertex[3];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos = g_vBaseVertex[4];
	vertex.dwColor = DXCOLOR_GREEN;
	g_vecP3D.push_back(vertex);

	vertex.vPos = g_vBaseVertex[5];
	vertex.dwColor = DXCOLOR_BLUE;
	g_vecP3D.push_back(vertex);

	vertex.vPos = g_vBaseVertex[6];
	vertex.dwColor = DXCOLOR_RED;
	g_vecP3D.push_back(vertex);

	vertex.vPos = g_vBaseVertex[7];
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
}
#pragma endregion
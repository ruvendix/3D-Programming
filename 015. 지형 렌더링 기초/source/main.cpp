#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"
#include "RX\RX3DAxisDX9.h"


// ====================================================================================
// ��ũ�� ���Ǻ��Դϴ�.
#define NORMAL_VECTOR_RENDERING_VERTEX   0x0001
#define NORMAL_VECTOR_RENDERING_TRIANGLE 0x0010
#define NORMAL_VECTOR_RENDERING_PLANE    0x0100


// ====================================================================================
// ���� ��� ������Դϴ�.
const INT32 TERRAIN_ROW      = 15;   // ������ ��(���� �����Դϴ�)
const INT32 TERRAIN_COLUMN   = 15;   // ������ ��(���� �����Դϴ�)
const FLOAT TERRAIN_DISTANCE = 0.4f; // ���� ���� �Ÿ�


// ====================================================================================
// ���� ���� ������Դϴ�.
HRESULT g_DXResult = S_OK;

namespace
{
	IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
	IDirect3DIndexBuffer9*  g_pIndexBuffer = nullptr;
	ID3DXLine*              g_pLine = nullptr; // ���� �׸��� ���� ��

	D3DXMATRIXA16 g_matViewAndProjection; // �̸� ����ص� ����� * �������

	DWORD g_dwNormalVectorRenderingFlag = 0;

	INT32 g_terrainVertexCnt; // ������ ���� ����
	INT32 g_terrainIndexCnt;  // ������ �ε��� ����

	std::vector<VertexP3N>   g_vecTerrainVertex; // ������ ���� ����
	std::vector<Index16>     g_vecTerrainIndex;  // ������ �ε��� ����
	std::vector<D3DXVECTOR3> g_vecTerrainVertexNormal;   // ���������� �������� ����
	std::vector<D3DXVECTOR3> g_vecTerrainTriangleNormal; // �ﰢ�������� �������� ����

	RX::RX3DAxisDX9 g_axis; // 3D���� �ٷ�� ���� ��
}

// ====================================================================================
// �Լ� ������Դϴ�.
HRESULT CALLBACK OnInit();
HRESULT CALLBACK OnUpdate();
HRESULT CALLBACK OnRender();
HRESULT CALLBACK OnRelease();

// �⺻ ����� �� ������������� �����մϴ�.
void DefaultViewAndProjection();

// �⺻ ������ �����մϴ�.
void DefaultLight();

// �⺻ ���� ������Ʈ�� �����մϴ�.
void DefaultRenderState();

// ���� �������� ���� ���� ���۸� �����մϴ�.
void CreateTerrainVertex();

// ���� �������� ���� �ε��� ���۸� �����մϴ�.
void CreateTerrainIndex();

// ���������� �������͸� ���մϴ�.
void CalcTerrainNormalVector();

// ������� Ű���� �Ǵ� ���콺 �Է¿� ���� ó���� �մϴ�.
void OnUserInput();

// �������͸� �������մϴ�.
void RenderNormalVector();

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

	// �޸� �Ҵ� ������ �̿��ؼ� �޸� ������ ã���ϴ�.
	// ��ҿ��� �ּ� ó���ϸ� �˴ϴ�.
	//_CrtSetBreakAlloc(451);

	RXMAIN_DX9->RunMainRoutine(hInstance, IDI_RUVENDIX_ICO);
	return RXMAIN_DX9->getMessageCode();
}

HRESULT CALLBACK OnInit()
{
	CreateTerrainVertex();
	CreateTerrainIndex();

	DefaultViewAndProjection();

	DefaultLight();
	DefaultRenderState();

	D3DXCreateLine(D3DDEVICE9, &g_pLine);

	// ���� �����մϴ�.
	g_axis.CreateAxis(20.0f);

	RX::ShowMouseCursor(true);

	return S_OK;
}

HRESULT CALLBACK OnUpdate()
{
	OnUserInput();
	return S_OK;
}

HRESULT CALLBACK OnRender()
{
	// ���� ���� �������� ������ �����Ƿ� ������ �����ؾ� �մϴ�.
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_axis.DrawAxis();
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, TRUE);

	D3DDEVICE9->SetFVF(VertexP3N::format);
	D3DDEVICE9->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(VertexP3N));
	D3DDEVICE9->SetIndices(g_pIndexBuffer);

	D3DDEVICE9->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
		g_terrainVertexCnt, 0, g_terrainIndexCnt / 3);

	RenderNormalVector();

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	g_vecTerrainVertex.clear();
	g_vecTerrainIndex.clear();

	g_axis.Release();

	SAFE_RELEASE(g_pIndexBuffer);
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pLine);
	return S_OK;
}

void DefaultViewAndProjection()
{
	// ==========================================================
	// ������� �����մϴ�.
	D3DXVECTOR3 vEye(2.0f, 5.0f, -7.0f);   // ī�޶��� ��ġ
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

	g_matViewAndProjection = matView * matProjection;
}

void DefaultLight()
{
	// ������ �ֺ����� ��ȫ���� ����մϴ�.
	// ������ �ϳ��� ��� �����մϴ�.
	// ���� ������ ���� �ٲ㰡�� �������� ���� �����ϴ�.
	D3DXMATERIAL mtrl;
	::ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.MatD3D.Ambient = D3DXCOLOR(DXCOLOR_MAGENTA);

	D3DDEVICE9->SetMaterial(&(mtrl.MatD3D));
}

void DefaultRenderState()
{
	D3DDEVICE9->SetRenderState(D3DRS_LIGHTING, TRUE);
	D3DDEVICE9->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	D3DDEVICE9->SetRenderState(D3DRS_AMBIENT, DXCOLOR_MAGENTA);
}

void CreateTerrainVertex()
{
	// �ʱ� ���� ��ġ�� �����ϴµ� Y���� ������ X, Z�����θ� �Ǵ��մϴ�.
	// Y���� ���� �������� ���Ǳ� �����ε� ������ �� �ƴմϴ�.
	// ����� �𸮾� ���������� Z���� ���� �������� ���˴ϴ�.
	//
	// �ʱ� ���� ��ġ�� ������ ���� �����˴ϴ�.
	// X = -1 * (���� �� ���� * ���� ���� �Ÿ� * 0.5)
	// Y = 0 �Ǵ� ���̰�
	// Z = +1 * (���� �� ���� * ���� ���� �Ÿ� * 0.5)
	// X, Z�� �������� ������ ������ ������ ���ߴ� �����Դϴ�.
	// �޼���ǥ���� Z���� �����ϸ� Z���� +�� �Ǵ� ������ �� �� �ֽ��ϴ�.
	//
	// ������ ������ ������ �ƴ� ������ �ΰ� ������
	// �ʱ� ���� ��ġ�� ���� �ڿ� �������ָ� �˴ϴ�.
	D3DXVECTOR3 vStart;
	vStart.x = -(TERRAIN_COLUMN * TERRAIN_DISTANCE * 0.5f);
	vStart.y = 0.1f;
	vStart.z = (TERRAIN_COLUMN * TERRAIN_DISTANCE * 0.5f);

	// �������� ����� ���� ������ ���մϴ�.
	// ���� �������� ((�� + 1) * (�� + 1))�Դϴ�.
	// �ε��� ���۸� ������� �ʴ´ٸ� (�� * �� * 3 * 2)�̷��� �ؾ� �մϴ�.
	// �簢�� ���� �������Ϸ��� ���� 6���� �ʿ��ѵ� �̷��� �ϸ� �ߺ��� �������� ����ϴ�.
	g_terrainVertexCnt = (TERRAIN_ROW + 1) * (TERRAIN_COLUMN + 1);

	// ���� ������ ���� ���ۿ� ����ϹǷ�
	// ���� ������ 1ȸ�����θ� ����ص� �˴ϴ�.
	g_vecTerrainVertex.reserve(g_terrainVertexCnt);

	// �ʱ� ���� ��ġ�� �̿��ؼ� ���� ������ �ֽ��ϴ�.
	// ������ ��� ���� ���� ���� �Ÿ��� �����ϸ� ���� ���� �� �ֽ��ϴ�.
	// ���� ������ ���� �������� 1��ŭ �����Ƿ� �ݺ��� Ƚ���� �����ؾ� �մϴ�.
	for (INT32 row = 0; row < TERRAIN_ROW + 1; ++row)
	{
		for (INT32 col = 0; col < TERRAIN_COLUMN + 1; ++col)
		{
			INT32 idx = row * (TERRAIN_COLUMN + 1) + col;
			VertexP3N vertex;

			vertex.vPos.x = vStart.x + (col * TERRAIN_DISTANCE);
			//vertex.vPos.y = vStart.y;
			vertex.vPos.y = vStart.y + ((rand() / (float)RAND_MAX) * 0.4f);
			vertex.vPos.z = vStart.z + -(row * TERRAIN_DISTANCE);

			g_vecTerrainVertex.push_back(vertex);
		}
	}

	CalcTerrainNormalVector();

	g_DXResult = D3DDEVICE9->CreateVertexBuffer(g_terrainVertexCnt * sizeof(VertexP3N),
		D3DUSAGE_WRITEONLY, VertexP3N::format, D3DPOOL_MANAGED, &g_pVertexBuffer, nullptr);
	DXERR_HANDLER(g_DXResult);
	NULLCHK_RETURN(g_pVertexBuffer, "���� ���� ���� �����߽��ϴ�!");

	void* pVertices = nullptr;
	g_pVertexBuffer->Lock(0, g_terrainVertexCnt * sizeof(VertexP3N), &pVertices, D3DFLAG_NONE);
	::CopyMemory(pVertices, &g_vecTerrainVertex[0], g_terrainVertexCnt * sizeof(VertexP3N));
	g_pVertexBuffer->Unlock();
}

void CreateTerrainIndex()
{
	// �������� ����� �ε��� ������ ���մϴ�.
	// ���� �������� (�� * �� * 6)�Դϴ�.
	// �ϳ��� ���� �׸��� ���ؼ��� �ε����� 6�� �ʿ��ϱ� �����Դϴ�.
	g_terrainIndexCnt = TERRAIN_ROW * TERRAIN_COLUMN * 6;

	// �ε��� ������ �ε��� ���ۿ� ����ϹǷ�
	// �ε��� ������ 1ȸ�����θ� ����ص� �˴ϴ�.
	// �ε��� ��ġ�� �龦�����̹Ƿ� std::vector�� �̿��մϴ�.
	g_vecTerrainIndex.reserve(g_terrainIndexCnt);

	// ������ �� ������ �̿��ؼ� �ε����� �����ݴϴ�.
	// �ﰢ�� 2���� �׷��� �ϴµ� ���Ӹ� �ƴ϶� �൵ ������ ��Ĩ�ϴ�.
	// ����� �밢�� ��ġ�� ��ģ�ٴ� �� �����ؾ� �մϴ�.
	// �ε����� �����ϴ� ����� ������ �����ϴ�.
	//
	// ���� ���� ù��° ���� ��ġ -> ���� ���� �ι�° ���� ��ġ -> ���� ���� ù��° ���� ��ġ
	// ���� ���� ù��° ���� ��ġ -> ���� ���� �ι�° ���� ��ġ -> ���� ���� �ι�° ���� ��ġ
	// 
	// �ε����� ���� ���ʹ� �޸� ������ ��� ����ŭ�� �ݺ����ָ� �˴ϴ�.
	for (INT32 row = 0; row < TERRAIN_ROW; ++row)
	{
		for (INT32 col = 0; col < TERRAIN_COLUMN; ++col)
		{
			Index16 idx;
			WORD baseIndices[6];

			// ù��° �ε���(���� ���� ù��° ���� ��ġ)
			baseIndices[0] = row * (TERRAIN_COLUMN + 1) + col;
			idx.index = baseIndices[0];
			g_vecTerrainIndex.push_back(idx);

			// �ι�° �ε���(���� ���� �ι�° ���� ��ġ)
			baseIndices[1] = (row + 1) * (TERRAIN_COLUMN + 1) + col + 1;
			idx.index = baseIndices[1];
			g_vecTerrainIndex.push_back(idx);

			// ����° �ε���(���� ���� ù��° ���� ��ġ)
			baseIndices[2] = (row + 1) * (TERRAIN_COLUMN + 1) + col;
			idx.index = baseIndices[2];
			g_vecTerrainIndex.push_back(idx);

			// �׹�° �ε���(���� ���� ù��° ���� ��ġ)
			baseIndices[3] = baseIndices[0];
			idx.index = baseIndices[3];
			g_vecTerrainIndex.push_back(idx);

			// �ټ���° �ε���(���� ���� �ι�° ���� ��ġ)
			baseIndices[4] = baseIndices[3] + 1;
			idx.index = baseIndices[4];
			g_vecTerrainIndex.push_back(idx);

			// ������° �ε���(���� ���� �ι�° ���� ��ġ)
			baseIndices[5] = baseIndices[1];
			idx.index = baseIndices[5];
			g_vecTerrainIndex.push_back(idx);
		}
	}

	g_DXResult = D3DDEVICE9->CreateIndexBuffer(g_terrainIndexCnt * sizeof(Index16),
		D3DUSAGE_WRITEONLY, Index16::format, D3DPOOL_MANAGED, &g_pIndexBuffer, nullptr);
	DXERR_HANDLER(g_DXResult);
	NULLCHK_RETURN(g_pIndexBuffer, "�ε��� ���� ���� �����߽��ϴ�!");

	void* pIndices = nullptr;
	g_pIndexBuffer->Lock(0, g_terrainIndexCnt * sizeof(Index16), &pIndices, D3DFLAG_NONE);
	::CopyMemory(pIndices, &g_vecTerrainIndex[0], g_terrainIndexCnt * sizeof(Index16));
	g_pIndexBuffer->Unlock();
}

void CalcTerrainNormalVector()
{
	// ������ ���̸� �����ؼ� �������ʹ� ������ �ﰢ���� ���մϴ�.
	// ���� ���������� ���ǰ� �� ��, �� �ﰢ�������� �������͸� �ǹ��մϴ�.
	// �� �鿡���� �������ʹ� �� �鿡 ���� ��� ������ �������Ϳ� �����մϴ�.
	// ��, ���� ������ �ƴ϶� ���� �������� ���� �մϴ�..
	// �ﰢ�������� �������ʹ� (���� �� ���� * ���� �� ���� * 2)��ŭ �����մϴ�.
	// ���������� ���ǿ� ���� �ﰢ�������� �������͸� ���ؼ� �� ������ �־��ָ�
	// �װ� ���������� �������Ͱ� �˴ϴ�. ���� ���������� �������ʹ� ���� ������ �ʿ䰡 �����ϴ�.
	//
	// ������ ���������� �������� ������ �ε��� ������ �Ȱ��ٴ� �Ϳ� �����ؾ� �մϴ�.
	// ���� ������ ��� ��Ĵ�� �������� ��ġ�� �����̱� �����Դϴ�.
	// ������ �ε��� ���۸� ����ϹǷ� ���������� �������ʹ� �ϳ��� ������ �� ������...
	// �������� �������� ��� �������͸� �������غ��ڽ��ϴ�.
	g_vecTerrainVertexNormal.reserve(g_terrainIndexCnt);
	g_vecTerrainTriangleNormal.reserve(TERRAIN_ROW * TERRAIN_COLUMN * 2);
	for (INT32 row = 0; row < TERRAIN_ROW; ++row)
	{
		for (INT32 col = 0; col < TERRAIN_COLUMN; ++col)
		{
			// ============================================================================
			// ù��° �鿡���� �������͸� ���մϴ�.
			D3DXVECTOR3 vNormal;
			INT32 baseIdx   = row * (TERRAIN_COLUMN + 1) + col;
			INT32 firstIdx  = (row + 1) * (TERRAIN_COLUMN + 1) + col + 1;
			INT32 secondIdx = (row + 1) * (TERRAIN_COLUMN + 1) + col;

			vNormal = RX::CalcNormalVector(g_vecTerrainVertex[baseIdx].vPos,
				g_vecTerrainVertex[firstIdx].vPos, g_vecTerrainVertex[secondIdx].vPos);
			
			g_vecTerrainVertex[baseIdx].vN   = vNormal;
			g_vecTerrainVertex[firstIdx].vN  = vNormal;
			g_vecTerrainVertex[secondIdx].vN = vNormal;

			g_vecTerrainVertexNormal.push_back(vNormal);
			g_vecTerrainVertexNormal.push_back(vNormal);
			g_vecTerrainVertexNormal.push_back(vNormal);

			g_vecTerrainTriangleNormal.push_back(vNormal);
			// ============================================================================
			// �ι�° �鿡���� �������͸� ���մϴ�.
			secondIdx = firstIdx;
			firstIdx  = baseIdx + 1;

			vNormal = RX::CalcNormalVector(g_vecTerrainVertex[baseIdx].vPos,
				g_vecTerrainVertex[firstIdx].vPos, g_vecTerrainVertex[secondIdx].vPos);

			// ����� ù��° �鿡���� �������͸� �����մϴ�.
			//g_vecTerrainVertex[baseIdx].vN   = vNormal;
			//g_vecTerrainVertex[firstIdx].vN  = vNormal;
			//g_vecTerrainVertex[secondIdx].vN = vNormal;

			g_vecTerrainVertexNormal.push_back(vNormal);
			g_vecTerrainVertexNormal.push_back(vNormal);
			g_vecTerrainVertexNormal.push_back(vNormal);

			g_vecTerrainTriangleNormal.push_back(vNormal);
		}
	}
}

void OnUserInput()
{
	if (::GetAsyncKeyState('A') & 0x8000)
	{
		D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	if (::GetAsyncKeyState('S') & 0x8000)
	{
		D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}

	if (::GetAsyncKeyState('D') & 0x8000)
	{
		D3DDEVICE9->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
	}

	if (::GetAsyncKeyState('F') & 0x0001)
	{
		g_axis.FlipEnable();
	}

	if (::GetAsyncKeyState('Q') & 0x0001)
	{
		TOGGLE_BIT(g_dwNormalVectorRenderingFlag, NORMAL_VECTOR_RENDERING_VERTEX);
	}

	if (::GetAsyncKeyState('W') & 0x0001)
	{
		TOGGLE_BIT(g_dwNormalVectorRenderingFlag, NORMAL_VECTOR_RENDERING_TRIANGLE);
	}
}

void RenderNormalVector()
{
	INT32 vertexIdx; // �̹� �ε��� ������ ���� �����̹Ƿ� �ܼ��ϰ� �ε����θ� �����մϴ�.
	D3DXVECTOR3 vList[2];

	if (IS_BIT(g_dwNormalVectorRenderingFlag, NORMAL_VECTOR_RENDERING_VERTEX))
	{
		vertexIdx = -1; // �ε��� �ʱ�ȭ
		for (INT32 row = 0; row < TERRAIN_ROW; ++row)
		{
			for (INT32 col = 0; col < TERRAIN_COLUMN; ++col)
			{
				for (INT32 terrainIdx = 0; terrainIdx < 6; ++terrainIdx)
				{
					++vertexIdx; // �ε��� ����

					g_pLine->Begin();
					vList[0] = g_vecTerrainVertex[g_vecTerrainIndex[vertexIdx].index].vPos;
					vList[1] = vList[0] + g_vecTerrainVertexNormal[vertexIdx];

					g_pLine->DrawTransform(vList, 2, &g_matViewAndProjection, DXCOLOR_WHITE);
					g_pLine->End();
				}
			}
		}
	}

	if (IS_BIT(g_dwNormalVectorRenderingFlag, NORMAL_VECTOR_RENDERING_TRIANGLE))
	{
		vertexIdx = -1; // �ε��� �ʱ�ȭ
		for (INT32 row = 0; row < TERRAIN_ROW; ++row)
		{
			for (INT32 col = 0; col < TERRAIN_COLUMN; ++col)
			{
				for (INT32 triangleIdx = 0; triangleIdx < 2; ++triangleIdx)
				{
					g_pLine->Begin();

					INT32 firstIdx  = ++vertexIdx;
					INT32 secondIdx = ++vertexIdx;
					INT32 thirdIdx  = ++vertexIdx;

					// ������ ���մϴ�.
					// ������ �� ��ǥ�� ���ؼ� ���ϴ¸�ŭ �����ָ� �˴ϴ�.
					vList[0] = (g_vecTerrainVertex[g_vecTerrainIndex[firstIdx].index].vPos +
						g_vecTerrainVertex[g_vecTerrainIndex[secondIdx].index].vPos +
						g_vecTerrainVertex[g_vecTerrainIndex[thirdIdx].index].vPos) / 3.0f;

					INT32 realIdx = (row * TERRAIN_COLUMN) + (col * 2) + triangleIdx;
					vList[1] = vList[0] + g_vecTerrainTriangleNormal[realIdx];
					g_pLine->DrawTransform(vList, 2, &g_matViewAndProjection, DXCOLOR_GREEN);
					g_pLine->End();
				}
			}
		}
	}
}
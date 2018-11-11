#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"
#include "RX\RX3DAxisDX9.h"


// ====================================================================================
// ���� ��� ������Դϴ�.
const INT32 TERRAIN_ROW      = 15;   // ������ ��(���� �����Դϴ�)
const INT32 TERRAIN_COLUMN   = 15;   // ������ ��(���� �����Դϴ�)
const FLOAT TERRAIN_DISTANCE = 0.4f; // ���� ���� �Ÿ�


// ====================================================================================
// ���� ���� ������Դϴ�.
IDirect3DDevice9*       g_pD3DDevice9   = nullptr;
RX::RXMain_DX9*         g_pMainDX       = nullptr;
IDirect3DVertexBuffer9* g_pVertexBuffer = nullptr;
IDirect3DIndexBuffer9*  g_pIndexBuffer  = nullptr;

D3DXMATRIXA16 g_matViewAndProjection; // �̸� ����ص� ����� * �������
D3DXMATRIXA16 g_matProjection;        // �̸� ����ص� �������

HRESULT g_DXResult = S_OK;

namespace
{
	INT32 g_terrainVertexCnt; // ������ ���� ����
	INT32 g_terrainIndexCnt;  // ������ �ε��� ����

	std::vector<VertexP3N>   g_vecTerrainVertex; // ������ ���� ����
	std::vector<Index16>     g_vecTerrainIndex;  // ������ �ε��� ����

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

	// �޸� �Ҵ� ������ �̿��ؼ� �޸� ������ ã���ϴ�.
	// ��ҿ��� �ּ� ó���ϸ� �˴ϴ�.
	//_CrtSetBreakAlloc(451);

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

	CreateTerrainVertex();
	CreateTerrainIndex();

	DefaultViewAndProjection();

	DefaultLight();
	DefaultRenderState();

	// ���� �����մϴ�.
	g_axis.CreateAxis(20.0f);

	// ���콺 Ŀ���� �����ݴϴ�.
	RX::ShowMouseCursor(true);

	return S_OK;
}

// ������Ʈ �Լ��Դϴ�.
// �������� ������ �ְų� ���α׷��� ������ �ִ�
// �������� �������Ӹ��� ������Ʈ�մϴ�.
HRESULT CALLBACK OnUpdate()
{
	OnUserInput();
	return S_OK;
}

// ������ �Լ��Դϴ�.
// ���� ������ �۾��� Draw Call�� ó���˴ϴ�.
// Draw Call�� �����Ӵ� ȣ��Ǵ� ������ �Լ��� ���ϴµ� ȣ��Ǵ� �󵵼���
// �����ϸ� Draw Call Count�� �˾Ƴ� �� �ֽ��ϴ�.
HRESULT CALLBACK OnRender()
{
	g_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_axis.DrawAxis();
	g_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, TRUE);

	g_pD3DDevice9->SetFVF(VertexP3N::format); // ���� ���� ����
	g_pD3DDevice9->SetStreamSource(
		0,                  // ����� ��Ʈ�� �ε���
		g_pVertexBuffer,    // ������ ���� ����
		0,                  // ���� ������ ������
		sizeof(VertexP3N)); // ���� �뷮

	g_pD3DDevice9->SetIndices(g_pIndexBuffer); // �ε��� ���� ����
	g_pD3DDevice9->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, // �ε��� ���۴� Ʈ���̾ޱ۸���Ʈ�� ����
		0, // ù �ε����� �� ���� ������ ���� �ε���
		0, // ����� ù �ε���(�ε����� 0, 1, 2, 3�� ���� �� 3�̸� 3���� ����)
		g_terrainVertexCnt, // ����� ���� ����(2��° ���� + 3��° ���ڸ�ŭ �� ��)
		0, // �ε��� ������ ������
		g_terrainIndexCnt / 3); // �������� ����(ť��� �ﰢ�� 12��)

	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matTrans;
	D3DXMATRIXA16 matRotateZ;

	// ���� ����� ��������� �����ɴϴ�.
	g_pD3DDevice9->GetTransform(D3DTS_WORLD, &matWorld);

	// ���� ��
	D3DXMatrixTranslation(&matTrans, -3.1f, 3.1f, 0.0f);
	D3DXMatrixRotationZ(&matRotateZ, D3DXToRadian(-90.0f));
	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &(matRotateZ * matTrans));
	g_pD3DDevice9->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, // �ε��� ���۴� Ʈ���̾ޱ۸���Ʈ�� ����
		0, // ù �ε����� �� ���� ������ ���� �ε���
		0, // ����� ù �ε���(�ε����� 0, 1, 2, 3�� ���� �� 3�̸� 3���� ����)
		g_terrainVertexCnt, // ����� ���� ����(2��° ���� + 3��° ���ڸ�ŭ �� ��)
		0, // �ε��� ������ ������
		g_terrainIndexCnt / 3); // �������� ����(ť��� �ﰢ�� 12��)

	// ���� ��
	D3DXMatrixTranslation(&matTrans, 0.0f, 6.2f, 0.0f);
	D3DXMatrixRotationZ(&matRotateZ, D3DXToRadian(180.0f));
	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &(matRotateZ * matTrans));
	g_pD3DDevice9->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, // �ε��� ���۴� Ʈ���̾ޱ۸���Ʈ�� ����
		0, // ù �ε����� �� ���� ������ ���� �ε���
		0, // ����� ù �ε���(�ε����� 0, 1, 2, 3�� ���� �� 3�̸� 3���� ����)
		g_terrainVertexCnt, // ����� ���� ����(2��° ���� + 3��° ���ڸ�ŭ �� ��)
		0, // �ε��� ������ ������
		g_terrainIndexCnt / 3); // �������� ����(ť��� �ﰢ�� 12��)

	// ������ ��
	D3DXMatrixTranslation(&matTrans, 3.1f, 3.1f, 0.0f);
	D3DXMatrixRotationZ(&matRotateZ, D3DXToRadian(90.0f));
	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &(matRotateZ * matTrans));
	g_pD3DDevice9->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, // �ε��� ���۴� Ʈ���̾ޱ۸���Ʈ�� ����
		0, // ù �ε����� �� ���� ������ ���� �ε���
		0, // ����� ù �ε���(�ε����� 0, 1, 2, 3�� ���� �� 3�̸� 3���� ����)
		g_terrainVertexCnt, // ����� ���� ����(2��° ���� + 3��° ���ڸ�ŭ �� ��)
		0, // �ε��� ������ ������
		g_terrainIndexCnt / 3); // �������� ����(ť��� �ﰢ�� 12��)

	// ���� ����� ��������� �����ɴϴ�.
	g_pD3DDevice9->SetTransform(D3DTS_WORLD, &matWorld);

	return S_OK;
}

HRESULT CALLBACK OnRelease()
{
	g_vecTerrainVertex.clear();
	g_vecTerrainIndex.clear();

	g_axis.Release();

	SAFE_RELEASE(g_pIndexBuffer);
	SAFE_RELEASE(g_pVertexBuffer);
	return S_OK;
}

void DefaultViewAndProjection()
{
	// ������� �����մϴ�.
	D3DXVECTOR3 vEye(0.0f, 3.0f, -11.0f);  // ī�޶��� ��ġ
	D3DXVECTOR3 vLookAt(0.0f, 3.1f, 0.0f); // ī�޶� ���� ����
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);     // ī�޶��� �� ����

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEye, &vLookAt, &vUp);
	g_pD3DDevice9->SetTransform(D3DTS_VIEW, &matView);

	// ��������� �����մϴ�.
	D3DXMatrixPerspectiveFovLH(&g_matProjection, (D3DX_PI / 4.0f),
		(static_cast<FLOAT>(g_pMainDX->getClientWidth()) / (g_pMainDX->getClientHeight())),
		1.0f, 1000.0f);
	g_pD3DDevice9->SetTransform(D3DTS_PROJECTION, &g_matProjection);

	// ������� �ʱ�ȭ�Դϴ�.
	g_matViewAndProjection = matView * g_matProjection;
}

void DefaultLight()
{
	// ������ �����ϰ� ����մϴ�.
	// �̹����� �� ������ �̿��մϴ�.
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_POINT;
	light.Position = D3DXVECTOR3(0.0f, 3.1f, 0.0f); // �� ������ ��ġ�� �����մϴ�.
	light.Range = 3.6f; // �� ������ ������ �����մϴ�.
	light.Diffuse = D3DXCOLOR(DXCOLOR_WHITE);
	
	// �� �������� ���� �������� ������ ������ �� �ֽ��ϴ�.
	light.Attenuation0 = 0.15f; // ���� 1�ܰ��Դϴ�.
	light.Attenuation1 = 0.15f; // ���� 2�ܰ��Դϴ�.
	light.Attenuation2 = 0.0f;  // ���� 3�ܰ��Դϴ�.

	// ������ ����ϰ� Ȱ��ȭ��ŵ�ϴ�.
	g_pD3DDevice9->SetLight(0, &light);
	g_pD3DDevice9->LightEnable(0, TRUE);

	// ������ �ϳ��� ��� �����մϴ�.
	// ���� ������ ���� �ٲ㰡�� �������� ���� �����ϴ�.
	D3DXMATERIAL mtrl;
	::ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.MatD3D.Ambient = D3DXCOLOR(DXCOLOR_BLUE);
	mtrl.MatD3D.Diffuse = D3DXCOLOR(DXCOLOR_RED);

	g_pD3DDevice9->SetMaterial(&(mtrl.MatD3D));
}

void DefaultRenderState()
{
	// rhw�� ������� �ʴ´ٸ� ��ȯ ������ ������ǥ��
	// ����ϰ� �ǹǷ� ���� ��ȯ ������ ���ľ� �մϴ�.
	// ����(������, Lighting)�� ���� �ϳ��ε�
	// �̹����� ������ ����ϹǷ� ������ ���ݴϴ�.
	g_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, TRUE);

	// �� ��带 �����մϴ�. ����Ʈ�� �ָ����Դϴ�.
	g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	//g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);

	// �ø� ��带 �����մϴ�. ����Ʈ�� �ݽð���� �ø��Դϴ�.
	// ť�긦 Ȯ���ϱ� ���ؼ��� �ø� ��带 �����ؾ� �մϴ�.
	g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	//g_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// �������͸� �ڵ����� ������ִ� �����Դϴ�.
	// ��! �� ������ �̿��ϰ� �Ǹ� ����� �� ž�ϴ�...
	// ����Ʈ�� FALSE�Դϴ�.
	g_pD3DDevice9->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// ���� �ֺ����� �����մϴ�.
	g_pD3DDevice9->SetRenderState(D3DRS_AMBIENT, DXCOLOR_MAGENTA);
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
			vertex.vPos.y = vStart.y;
			//vertex.vPos.y = vStart.y + ((rand() / (float)RAND_MAX) * 0.4f);
			vertex.vPos.z = vStart.z + -(row * TERRAIN_DISTANCE);

			g_vecTerrainVertex.push_back(vertex);
		}
	}

	// ������ �������͸� ���մϴ�.
	CalcTerrainNormalVector();

	// ���� ���۸� �����մϴ�.
	g_DXResult = g_pD3DDevice9->CreateVertexBuffer(
		g_terrainVertexCnt * sizeof(VertexP3N), // �뷮
		D3DUSAGE_WRITEONLY, // ��������(�ӵ� ���)
		VertexP3N::format,  // FVF
		D3DPOOL_MANAGED,    // �޸�Ǯ(�ý��� + VRAM)
		&g_pVertexBuffer,   // ���� ���� �������� �ּ�
		nullptr); // �� ���� ��
	DXERR_HANDLER(g_DXResult);

	void* pVertices = nullptr;

	// ���� ������ ���� ���ۿ� �ֽ��ϴ�.
	g_DXResult = g_pVertexBuffer->Lock(
		0, // ������
		g_terrainVertexCnt * sizeof(VertexP3N), // �뷮
		&pVertices,    // ���� ������ ���� ������ ���� ���� �ּ�
		D3DFLAG_NONE); // ����� ������� ����
	DXERR_HANDLER(g_DXResult);
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

	// �ε��� ���۸� �����մϴ�.
	g_DXResult = g_pD3DDevice9->CreateIndexBuffer(
		g_terrainIndexCnt * sizeof(Index16), // �뷮
		D3DUSAGE_WRITEONLY, // ��������(�ӵ� ���)
		Index16::format,    // ����
		D3DPOOL_MANAGED,    // �޸�Ǯ(�ý��� + VRAM)
		&g_pIndexBuffer,    // �ε��� ���� �������� �ּ�
		nullptr); // �� ���� ��
	DXERR_HANDLER(g_DXResult);

	void* pIndices = nullptr;

	// �ε��� ������ �ε��� ���ۿ� �ֽ��ϴ�.
	g_DXResult = g_pIndexBuffer->Lock(
		0, // ������
		g_terrainIndexCnt * sizeof(Index16), // �뷮
		&pIndices,     // �ε��� ������ �ε��� ������ ���� ���� �ּ�
		D3DFLAG_NONE); // ����� ������� ����
	DXERR_HANDLER(g_DXResult);
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
		}
	}
}

void OnUserInput()
{
	if (::GetAsyncKeyState('A') & 0x8000)
	{
		g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	if (::GetAsyncKeyState('S') & 0x8000)
	{
		g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}

	if (::GetAsyncKeyState('D') & 0x8000)
	{
		g_pD3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
	}

	if (::GetAsyncKeyState('F') & 0x0001)
	{
		g_axis.FlipEnable();
	}
}
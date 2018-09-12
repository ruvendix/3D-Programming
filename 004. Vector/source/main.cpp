#include "base_project.h"
#include "global_variable_definition.h"

// ====================================================================================
// ��ũ�� ���Ǻ��Դϴ�.


// ====================================================================================
// ����ü �� ����ü ������Դϴ�.


// ====================================================================================
// ���� ���� ������Դϴ�.


// ====================================================================================
// �Լ� ������Դϴ�.
void ZeroVector(D3DXVECTOR3* pV);
void SetVector(D3DXVECTOR3* pV, FLOAT rX, FLOAT rY, FLOAT rZ);
void AddTest();
void SubtractTest();
void ScalarMultiplyTest();
void ScalarDivideTest();

// ====================================================================================
INT32 main()
{
	//AddTest();
	//SubtractTest();
	//ScalarMultiplyTest();
	ScalarDivideTest();
	return 0;
}

void ZeroVector(D3DXVECTOR3 * pV)
{
	NULLCHK_RETURN_NOCOMENT(pV);

	// ���ʹ� �̷��� ���� �����մϴ�.
	pV->x = 0.0f;
	pV->y = 0.0f;
	pV->z = 0.0f;
}

void SetVector(D3DXVECTOR3* pV, FLOAT rX, FLOAT rY, FLOAT rZ)
{
	NULLCHK_RETURN_NOCOMENT(pV);

	// ���ʹ� �̷��� ���� �����մϴ�.
	pV->x = rX;
	pV->y = rY;
	pV->z = rZ;
}

void AddTest()
{
	// 3���� ������ �⺻���Դϴ�.
	// �����Ⱚ�� �����Ƿ� �ʱ�ȭ�� �ʼ��Դϴ�.
	D3DXVECTOR3 vPos1(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vPos2(0.0f, 0.0f, 0.0f);

	// �����ͷ� ����� �ʱ�ȭ�ε�
	// �̷��� �Լ��� ���� �� ���� �ֽ��ϴ�.
	ZeroVector(&vPos1);
	ZeroVector(&vPos2);

	// ������ ���� �����ϴ� �ǵ�
	// �̷��� �Լ��� ���� �� ���� �ֽ��ϴ�.
	SetVector(&vPos1, 1.0f, 2.0f, 3.0f);

	// ������ �����Դϴ�.
	D3DXVECTOR3 vResult(0.0f, 0.0f, 0.0f);
	vResult = vPos1 + vPos2;
	printf("vPos1 + vPos2 = (%.2f, %.2f, %.2f)\n", vResult.x, vResult.y, vResult.z);

	// �Լ��� ����ص� ���� ����Դϴ�.
	ZeroVector(&vResult);
	D3DXVec3Add(&vResult, &vPos1, &vPos2);
	printf("vPos1 + vPos2 = (%.2f, %.2f, %.2f)\n", vResult.x, vResult.y, vResult.z);
	printf("=========================================\n");
}

void SubtractTest()
{
	// 3���� ������ �⺻���Դϴ�.
	// �����Ⱚ�� �����Ƿ� �ʱ�ȭ�� �ʼ��Դϴ�.
	D3DXVECTOR3 vPos1(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vPos2(0.0f, 0.0f, 0.0f);

	// �����ͷ� ����� �ʱ�ȭ�ε�
	// �̷��� �Լ��� ���� �� ���� �ֽ��ϴ�.
	ZeroVector(&vPos1);
	ZeroVector(&vPos2);

	// ������ ���� �����ϴ� �ǵ�
	// �̷��� �Լ��� ���� �� ���� �ֽ��ϴ�.
	SetVector(&vPos2, 1.0f, 2.0f, 3.0f);

	// ������ �����Դϴ�.
	D3DXVECTOR3 vResult(0.0f, 0.0f, 0.0f);
	vResult = vPos1 - vPos2;
	printf("vPos1 - vPos2 = (%.2f, %.2f, %.2f)\n", vResult.x, vResult.y, vResult.z);

	// �Լ��� ����ص� ���� ����Դϴ�.
	ZeroVector(&vResult);
	D3DXVec3Subtract(&vResult, &vPos1, &vPos2);
	printf("vPos1 - vPos2 = (%.2f, %.2f, %.2f)\n", vResult.x, vResult.y, vResult.z);
	printf("=========================================\n");
}

void ScalarMultiplyTest()
{
	// 3���� ������ �⺻���Դϴ�.
	// �����Ⱚ�� �����Ƿ� �ʱ�ȭ�� �ʼ��Դϴ�.
	D3DXVECTOR3 vPos(0.0f, 0.0f, 0.0f);

	// �����ͷ� ����� �ʱ�ȭ�ε�
	// �̷��� �Լ��� ���� �� ���� �ֽ��ϴ�.
	ZeroVector(&vPos);

	// ������ ���� �����ϴ� �ǵ�
	// �̷��� �Լ��� ���� �� ���� �ֽ��ϴ�.
	SetVector(&vPos, 1.0f, 2.0f, 3.0f);

	// ������ ��Į�� �����Դϴ�.
	D3DXVECTOR3 vResult(0.0f, 0.0f, 0.0f);
	vResult = vPos * 2;
	printf("vPos * 2 = (%.2f, %.2f, %.2f)\n", vResult.x, vResult.y, vResult.z);

	// �Լ��� ����ص� ���� ����Դϴ�.
	ZeroVector(&vResult);
	D3DXVec3Scale(&vResult, &vPos, 2.0f);
	printf("vPos * 2 = (%.2f, %.2f, %.2f)\n", vResult.x, vResult.y, vResult.z);
	printf("=========================================\n");
}

void ScalarDivideTest()
{
	// 3���� ������ �⺻���Դϴ�.
	// �����Ⱚ�� �����Ƿ� �ʱ�ȭ�� �ʼ��Դϴ�.
	D3DXVECTOR3 vPos(0.0f, 0.0f, 0.0f);

	// �����ͷ� ����� �ʱ�ȭ�ε�
	// �̷��� �Լ��� ���� �� ���� �ֽ��ϴ�.
	ZeroVector(&vPos);

	// ������ ���� �����ϴ� �ǵ�
	// �̷��� �Լ��� ���� �� ���� �ֽ��ϴ�.
	SetVector(&vPos, 1.0f, 2.0f, 3.0f);

	// ������ ��Į�� �������Դϴ�.
	D3DXVECTOR3 vResult(0.0f, 0.0f, 0.0f);
	vResult = vPos / 2;
	printf("vPos / 2 = (%.2f, %.2f, %.2f)\n", vResult.x, vResult.y, vResult.z);

	// �Լ��� ����ص� ���� ����Դϴ�.
	ZeroVector(&vResult);
	D3DXVec3Scale(&vResult, &vPos, 1.0f / 2.0f);
	printf("vPos / 2 = (%.2f, %.2f, %.2f)\n", vResult.x, vResult.y, vResult.z);
	printf("=========================================\n");
}
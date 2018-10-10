#include "base_project.h"
#include "global_variable_declaration.h"
#include "main.h"


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
void NormalizeTest();
void DotProductTest();
void CrossProductTest();


// ====================================================================================
INT32 main()
{
	//AddTest();
	//SubtractTest();
	//ScalarMultiplyTest();
	//ScalarDivideTest();
	//NormalizeTest();
	//DotProductTest();
	//CrossProductTest();

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

void NormalizeTest()
{
	// 3���� ������ �⺻���Դϴ�.
	// �����Ⱚ�� �����Ƿ� �ʱ�ȭ�� �ʼ��Դϴ�.
	D3DXVECTOR3 vPos(3.0f, 3.0f, 3.0f);

	// ������ ���̸� �������� ���մϴ�.
	// ������ ������ ������尡 ũ�Ƿ� �ܼ��� ���� ����
	// ���̸� �������� �Ǵ��ϴ� �� �����ϴ�.
	FLOAT rLengthSq = D3DXVec3LengthSq(&vPos);
	printf("rLengthSq = %.2f\n", rLengthSq);

	// ������ ���̸� ���մϴ�.
	// ������ ���� ���̿� ��Ʈ�� ����� �Ŷ�� �����ϸ� �˴ϴ�.
	FLOAT rLength = D3DXVec3Length(&vPos);
	printf("rLength   = %.2f\n", rLength);

	// ���͸� ����ȭ�մϴ�.
	// ��, ���͸� �������ͷ� ����ϴ�.
	// �Լ��� ���ڴ� �����ʺ��� �Ű������� �����Ƿ�
	// �Ϲ������� ����ȭ�� �̷��� ���� ������ ó���մϴ�.
	// �������͸� ���� �� ������ ���� ����ϹǷ� �ͼ������� �մϴ�.
	D3DXVec3Normalize(&vPos, &vPos);
	printf("����ȭ�� vPos = (%.2f, %.2f, %.2f)\n", vPos.x, vPos.y, vPos.z);
	printf("=========================================\n");
}

void DotProductTest()
{
	// 3���� ������ �⺻���Դϴ�.
	// �����Ⱚ�� �����Ƿ� �ʱ�ȭ�� �ʼ��Դϴ�.
	D3DXVECTOR3 vPos1(1.0f, 1.0f, 1.0f);
	D3DXVECTOR3 vPos2(2.0f, 0.0f, 3.0f);

	// �� ���͸� �̿��ؼ� ������ ���մϴ�.
	// ����������δ� �� ���г��� ���� ���ε� ��ġ���� �����Դϴ�.
	// �����������δ� �� ���Ͱ� �̷�� ���� ���翵�� �� �� �ֽ��ϴ�.
	// �� �� �˾ƾ� ������ �ʹݿ��� ��������� �ǹ̸� �˾Ƶ� �����մϴ�.
	FLOAT rResult = D3DXVec3Dot(&vPos1, &vPos2);
	printf("vPos1 �� vPos2 = %.2f\n", rResult);
	
	// ������ ������ �ٲ㵵 ����� �����մϴ�.
	rResult = D3DXVec3Dot(&vPos2, &vPos1);
	printf("vPos2 �� vPos1 = %.2f\n", rResult);
	printf("=========================================\n");

	// ������ ����� �� ���Ͱ� �̷�� ���� �����̰�,
	// ������ 0�̸� �� ���Ͱ� �̷�� ���� �����̰�,
	// ������ ������ �� ���Ͱ� �̷�� ���� �а��Դϴ�.
	//
	// ���߿� ī�޶� ���� �Ǵµ� ī�޶� ������ ���ϴ� ���Ϳ�
	// �������� �������Ϳ��� ������ ���� ������ �ø��� �����˴ϴ�.
}

void CrossProductTest()
{
	// 3���� ������ �⺻���Դϴ�.
	// �����Ⱚ�� �����Ƿ� �ʱ�ȭ�� �ʼ��Դϴ�.
	D3DXVECTOR3 vPos1(1.0f, 1.0f, 1.0f);
	D3DXVECTOR3 vPos2(2.0f, 0.0f, 3.0f);

	// �� ���͸� �̿��ؼ� ������ ���մϴ�.
	// ����������δ� �� ���г��� �����ϴ� ���ε� ��ġ���� �����Դϴ�.
	// �����������δ� �� ���Ϳ� ������ �����ε� �޼���ǥ��� ��������ǥ�迡 ���� ������ �ٸ��ϴ�.
	// ���⼭ �� �򰥸� �� �ִµ� ����������δ� �޼���ǥ��� ��������ǥ��� ���� �����մϴ�.
	// �ٸ� Z���� ������ ���� �ݴ��̹Ƿ� ���� ��� ������ Z���� �ָ��ؾ� �մϴ�.
	D3DXVECTOR3 vResult(0.0f, 0.0f, 0.0f);
	D3DXVec3Cross(&vResult, &vPos1, &vPos2);
	printf("vPos1 X vPos2 = (%+5.2f, %+5.2f, %+5.2f)\n", vResult.x, vResult.y, vResult.z);

	// ������ ������ �ٲٸ� ����� �ٲ�ϴ�.
	// �޼���ǥ��� ��������ǥ��ʹ� ������ �����ϴ�.
	D3DXVec3Cross(&vResult, &vPos2, &vPos1);
	printf("vPos2 X vPos1 = (%+5.2f, %+5.2f, %+5.2f)\n", vResult.x, vResult.y, vResult.z);
	printf("=========================================\n");

	// ����� �������͸� �������� ���ϰ� �Ǵµ� �߿��� �� �����̹Ƿ�
	// ���� ����ȭ�� ���� �ʾƵ� ������ �ʿ��� ���� ����ȭ�� �ؾ� �մϴ�.
	// �޼���ǥ��� ��������ǥ�迡 ���� Z���� ������ �ٲ�� �Ǵµ�
	// �� ���⿡ ���� �ø��� �����˴ϴ�. �ø��� ���ؼ��� �������� �����߽��ϴ�.
	// �������͸� ���ϴ� ����� �����̹Ƿ� ������ ����ǰ� �˴ϴ�.
}
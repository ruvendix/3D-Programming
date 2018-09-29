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
//
// r => row
// c => column
void ZeroMatrix(D3DXMATRIXA16* pMat);
void SetMatrixRow(D3DXMATRIXA16* pMat, INT32 r, FLOAT c1, FLOAT c2, FLOAT c3, FLOAT c4);
void SetMatrix1stRow(D3DXMATRIXA16* pMat, FLOAT c11, FLOAT c12, FLOAT c13, FLOAT c14);
void SetMatrix2ndRow(D3DXMATRIXA16* pMat, FLOAT c21, FLOAT c22, FLOAT c23, FLOAT c24);
void SetMatrix3rdRow(D3DXMATRIXA16* pMat, FLOAT c31, FLOAT c32, FLOAT c33, FLOAT c34);
void SetMatrix4thRow(D3DXMATRIXA16* pMat, FLOAT c41, FLOAT c42, FLOAT c43, FLOAT c44);
void ShowMatrixValue(D3DXMATRIXA16* pMat);

void SetupTest();
void AddTest();
void SubtractTest();
void ScalarMultiplyTest();
void ScalarDivideTest();

// ====================================================================================
INT32 main()
{
	//SetupTest();
	//AddTest();
	//SubtractTest();
	//ScalarMultiplyTest();
	//ScalarDivideTest();

	return 0;
}

void ZeroMatrix(D3DXMATRIXA16* pMat)
{
	NULLCHK_RETURN_NOCOMENT(pMat);

	// ����� �̷��� ���� �����մϴ�.
	pMat->_11 = 0.0f; pMat->_12 = 0.0f; pMat->_13 = 0.0f; pMat->_14 = 0.0f;
	pMat->_21 = 0.0f; pMat->_22 = 0.0f; pMat->_23 = 0.0f; pMat->_24 = 0.0f;
	pMat->_31 = 0.0f; pMat->_32 = 0.0f; pMat->_33 = 0.0f; pMat->_34 = 0.0f;
	pMat->_41 = 0.0f; pMat->_42 = 0.0f; pMat->_43 = 0.0f; pMat->_44 = 0.0f;

	// �̷��� �ص� �˴ϴ�.
	//::ZeroMemory(pMat, sizeof(D3DXMATRIXA16));
}

void SetMatrixRow(D3DXMATRIXA16* pMat, INT32 r, FLOAT c1, FLOAT c2, FLOAT c3, FLOAT c4)
{
	NULLCHK_RETURN_NOCOMENT(pMat);

	// 4�� 4�� �����̹Ƿ� ���� 0���� �۰ų� 3���� ũ�� ������ ó���մϴ�.
	if ( (r < 0) ||
		 (r > 3) )
	{
		RXERRLOG("���� ������ ������ϴ�! �� : %d", r);
		return;
	}

	// ����� ���� �̷��Ե� ���� �����մϴ�.
	pMat->m[r][0] = c1;
	pMat->m[r][1] = c2;
	pMat->m[r][2] = c3;
	pMat->m[r][3] = c4;
}

void SetMatrix1stRow(D3DXMATRIXA16* pMat, FLOAT c11, FLOAT c12, FLOAT c13, FLOAT c14)
{
	NULLCHK_RETURN_NOCOMENT(pMat);
	pMat->_11 = c11; pMat->_12 = c12; pMat->_13 = c13; pMat->_14 = c14;
}

void SetMatrix2ndRow(D3DXMATRIXA16* pMat, FLOAT c21, FLOAT c22, FLOAT c23, FLOAT c24)
{
	NULLCHK_RETURN_NOCOMENT(pMat);
	pMat->_21 = c21; pMat->_22 = c22; pMat->_23 = c23; pMat->_24 = c24;
}

void SetMatrix3rdRow(D3DXMATRIXA16* pMat, FLOAT c31, FLOAT c32, FLOAT c33, FLOAT c34)
{
	NULLCHK_RETURN_NOCOMENT(pMat);
	pMat->_31 = c31; pMat->_32 = c32; pMat->_33 = c33; pMat->_34 = c34;
}

void SetMatrix4thRow(D3DXMATRIXA16* pMat, FLOAT c41, FLOAT c42, FLOAT c43, FLOAT c44)
{
	NULLCHK_RETURN_NOCOMENT(pMat);
	pMat->_41 = c41; pMat->_42 = c42; pMat->_43 = c43; pMat->_44 = c44;
}

void ShowMatrixValue(D3DXMATRIXA16* pMat)
{
	NULLCHK_RETURN_NOCOMENT(pMat);

	printf("=======================================\n");
	for (INT32 row = 0; row < 4; ++row)
	{
		for (INT32 col = 0; col < 4; ++col)
		{
			printf("%.2f\t", pMat->m[row][col]);
		}
		printf("\n");
	}
	printf("=======================================\n\n");
}

void SetupTest()
{
	D3DXMATRIXA16 mat;

	// ����� ��� ���� 0�� �����
	// ������̶�� �մϴ�.
	printf("������� �׽�Ʈ�մϴ�.\n");
	ZeroMatrix(&mat);
	ShowMatrixValue(&mat);

	// ����� 0����� ī��Ʈ�Ǵµ�
	// 3D���� ����ϴ� ����� 4�� 4���Դϴ�.
	// ���� 0�� ~ 3������� ���� �����Դϴ�.
	printf("1���� ���� �����մϴ�.\n");
	SetMatrixRow(&mat, 1, 1.0f, 2.0f, 3.0f, 4.0f);
	ShowMatrixValue(&mat);

	// 3���� ���� �����غ��ϴ�.
	printf("3���� ���� �����մϴ�.\n");
	SetMatrix4thRow(&mat, 4.0f, 3.0f, 2.0f, 1.0f);
	ShowMatrixValue(&mat);
}

void AddTest()
{
	// 4�� 4�� ����� �⺻���Դϴ�.
	D3DXMATRIXA16 mat1;
	D3DXMATRIXA16 mat2;

	// ����ķ� ����ϴ�.
	ZeroMatrix(&mat1);
	ZeroMatrix(&mat2);

	for (INT32 i = 0; i < 4; ++i)
	{
		SetMatrixRow(&mat1, i, 1.0f, 2.0f, 2.0f, 3.0f);
	}

	// ����� �����Դϴ�.
	// ����� ������ �Լ��� ���� �������� �ʽ��ϴ�.
	D3DXMATRIXA16 matResult;
	ZeroMatrix(&matResult);
	matResult = mat1 + mat2;
	printf("mat1 + mat2�� ����Դϴ�.\n");
	ShowMatrixValue(&matResult);
}

void SubtractTest()
{
	// 4�� 4�� ����� �⺻���Դϴ�.
	D3DXMATRIXA16 mat1;
	D3DXMATRIXA16 mat2;

	// ����ķ� ����ϴ�.
	ZeroMatrix(&mat1);
	ZeroMatrix(&mat2);

	for (INT32 i = 0; i < 4; ++i)
	{
		SetMatrixRow(&mat1, i, 1.0f, 2.0f, 2.0f, 3.0f);
	}

	// ����� �����Դϴ�.
	// ����� ������ �Լ��� ���� �������� �ʽ��ϴ�.
	D3DXMATRIXA16 matResult;
	ZeroMatrix(&matResult);
	matResult = mat2 - mat1;
	printf("mat2 - mat1�� ����Դϴ�.\n");
	ShowMatrixValue(&matResult);
}

void ScalarMultiplyTest()
{
	// 4�� 4�� ����� �⺻���Դϴ�.
	D3DXMATRIXA16 mat;

	// ����ķ� ����ϴ�.
	ZeroMatrix(&mat);

	for (INT32 i = 0; i < 4; ++i)
	{
		SetMatrixRow(&mat, i, 1.0f, 2.0f, 3.0f, 4.0f);
	}

	// ����� ��Į�� �����Դϴ�.
	// ����� ��Į�� ������ �Լ��� ���� �������� �ʽ��ϴ�.
	D3DXMATRIXA16 matResult;
	ZeroMatrix(&matResult);
	matResult = mat * 2.0f;
	printf("mat * 2.0f�� ����Դϴ�.\n");
	ShowMatrixValue(&matResult);
}

void ScalarDivideTest()
{
	// 4�� 4�� ����� �⺻���Դϴ�.
	D3DXMATRIXA16 mat;

	// ����ķ� ����ϴ�.
	ZeroMatrix(&mat);

	for (INT32 i = 0; i < 4; ++i)
	{
		SetMatrixRow(&mat, i, 2.0f, 4.0f, 6.0f, 8.0f);
	}

	// ����� ��Į�� �������Դϴ�.
	// ����� ��Į�� �������� �Լ��� ���� �������� �ʽ��ϴ�.
	D3DXMATRIXA16 matResult;
	ZeroMatrix(&matResult);
	matResult = mat / 2.0f;
	printf("mat / 2.0f�� ����Դϴ�.\n");
	ShowMatrixValue(&matResult);
}
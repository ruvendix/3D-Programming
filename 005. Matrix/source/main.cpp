#include "base_project.h"
#include "global_variable_definition.h"

// ====================================================================================
// 매크로 정의부입니다.


// ====================================================================================
// 구조체 및 공용체 선언부입니다.


// ====================================================================================
// 전역 변수 선언부입니다.


// ====================================================================================
// 함수 선언부입니다.
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

	// 행렬은 이렇게 값을 설정합니다.
	pMat->_11 = 0.0f; pMat->_12 = 0.0f; pMat->_13 = 0.0f; pMat->_14 = 0.0f;
	pMat->_21 = 0.0f; pMat->_22 = 0.0f; pMat->_23 = 0.0f; pMat->_24 = 0.0f;
	pMat->_31 = 0.0f; pMat->_32 = 0.0f; pMat->_33 = 0.0f; pMat->_34 = 0.0f;
	pMat->_41 = 0.0f; pMat->_42 = 0.0f; pMat->_43 = 0.0f; pMat->_44 = 0.0f;

	// 이렇게 해도 됩니다.
	//::ZeroMemory(pMat, sizeof(D3DXMATRIXA16));
}

void SetMatrixRow(D3DXMATRIXA16* pMat, INT32 r, FLOAT c1, FLOAT c2, FLOAT c3, FLOAT c4)
{
	NULLCHK_RETURN_NOCOMENT(pMat);

	// 4행 4열 기준이므로 행이 0보다 작거나 3보다 크면 오류로 처리합니다.
	if ( (r < 0) ||
		 (r > 3) )
	{
		RXERRLOG("행의 범위를 벗어났습니다! 행 : %d", r);
		return;
	}

	// 행렬의 값은 이렇게도 설정 가능합니다.
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

	// 행렬의 모든 값이 0인 행렬을
	// 영행렬이라고 합니다.
	printf("영행렬을 테스트합니다.\n");
	ZeroMatrix(&mat);
	ShowMatrixValue(&mat);

	// 행렬은 0행부터 카운트되는데
	// 3D에서 사용하는 행렬은 4행 4열입니다.
	// 따라서 0행 ~ 3행까지가 행의 범위입니다.
	printf("1행의 값을 설정합니다.\n");
	SetMatrixRow(&mat, 1, 1.0f, 2.0f, 3.0f, 4.0f);
	ShowMatrixValue(&mat);

	// 3행의 값을 설정해봅니다.
	printf("3행의 값을 설정합니다.\n");
	SetMatrix4thRow(&mat, 4.0f, 3.0f, 2.0f, 1.0f);
	ShowMatrixValue(&mat);
}

void AddTest()
{
	// 4행 4열 행렬의 기본형입니다.
	D3DXMATRIXA16 mat1;
	D3DXMATRIXA16 mat2;

	// 영행렬로 만듭니다.
	ZeroMatrix(&mat1);
	ZeroMatrix(&mat2);

	for (INT32 i = 0; i < 4; ++i)
	{
		SetMatrixRow(&mat1, i, 1.0f, 2.0f, 2.0f, 3.0f);
	}

	// 행렬의 덧셈입니다.
	// 행렬의 덧셈은 함수가 따로 제공되지 않습니다.
	D3DXMATRIXA16 matResult;
	ZeroMatrix(&matResult);
	matResult = mat1 + mat2;
	printf("mat1 + mat2의 결과입니다.\n");
	ShowMatrixValue(&matResult);
}

void SubtractTest()
{
	// 4행 4열 행렬의 기본형입니다.
	D3DXMATRIXA16 mat1;
	D3DXMATRIXA16 mat2;

	// 영행렬로 만듭니다.
	ZeroMatrix(&mat1);
	ZeroMatrix(&mat2);

	for (INT32 i = 0; i < 4; ++i)
	{
		SetMatrixRow(&mat1, i, 1.0f, 2.0f, 2.0f, 3.0f);
	}

	// 행렬의 덧셈입니다.
	// 행렬의 덧셈은 함수가 따로 제공되지 않습니다.
	D3DXMATRIXA16 matResult;
	ZeroMatrix(&matResult);
	matResult = mat2 - mat1;
	printf("mat2 - mat1의 결과입니다.\n");
	ShowMatrixValue(&matResult);
}

void ScalarMultiplyTest()
{
	// 4행 4열 행렬의 기본형입니다.
	D3DXMATRIXA16 mat;

	// 영행렬로 만듭니다.
	ZeroMatrix(&mat);

	for (INT32 i = 0; i < 4; ++i)
	{
		SetMatrixRow(&mat, i, 1.0f, 2.0f, 3.0f, 4.0f);
	}

	// 행렬의 스칼라 곱셈입니다.
	// 행렬의 스칼라 곱셈은 함수가 따로 제공되지 않습니다.
	D3DXMATRIXA16 matResult;
	ZeroMatrix(&matResult);
	matResult = mat * 2.0f;
	printf("mat * 2.0f의 결과입니다.\n");
	ShowMatrixValue(&matResult);
}

void ScalarDivideTest()
{
	// 4행 4열 행렬의 기본형입니다.
	D3DXMATRIXA16 mat;

	// 영행렬로 만듭니다.
	ZeroMatrix(&mat);

	for (INT32 i = 0; i < 4; ++i)
	{
		SetMatrixRow(&mat, i, 2.0f, 4.0f, 6.0f, 8.0f);
	}

	// 행렬의 스칼라 나눗셈입니다.
	// 행렬의 스칼라 나눗셈은 함수가 따로 제공되지 않습니다.
	D3DXMATRIXA16 matResult;
	ZeroMatrix(&matResult);
	matResult = mat / 2.0f;
	printf("mat / 2.0f의 결과입니다.\n");
	ShowMatrixValue(&matResult);
}
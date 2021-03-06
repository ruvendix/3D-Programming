#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


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
void IdentityTest();
void TransposeTest();
void MultiplyTest();
void IdentityMultiplyTest();
void DeterminantTest();
void InverseTest();


// ====================================================================================
INT32 main()
{
	//SetupTest();
	//AddTest();
	//SubtractTest();
	//ScalarMultiplyTest();
	//ScalarDivideTest();
	//IdentityTest();
	//TransposeTest();
	//MultiplyTest();
	//IdentityMultiplyTest();
	//DeterminantTest();
	//InverseTest();

	return 0;
}

void ZeroMatrix(D3DXMATRIXA16* pMat)
{
	for (INT32 i = 0; i < 4; ++i)
	{
		SetMatrixRow(pMat, i, 0.0f, 0.0f, 0.0f, 0.0f);
	}

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
			printf("%5.2f\t", pMat->m[row][col]);
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
	D3DXMATRIXA16 mat1;
	for (INT32 i = 0; i < 4; ++i)
	{
		SetMatrixRow(&mat1, i, 1.0f, 2.0f, 2.0f, 3.0f);
	}

	D3DXMATRIXA16 mat2;
	ZeroMatrix(&mat2);

	D3DXMATRIXA16 matResult;
	ZeroMatrix(&matResult);

	// 행렬의 덧셈입니다.
	// 행렬의 덧셈은 함수가 따로 제공되지 않습니다.
	matResult = mat1 + mat2;
	printf("mat1 + mat2의 결과입니다.\n");
	ShowMatrixValue(&matResult);
}

void SubtractTest()
{
	D3DXMATRIXA16 mat1;
	for (INT32 i = 0; i < 4; ++i)
	{
		SetMatrixRow(&mat1, i, 1.0f, 2.0f, 2.0f, 3.0f);
	}

	D3DXMATRIXA16 mat2;
	ZeroMatrix(&mat2);

	D3DXMATRIXA16 matResult;
	ZeroMatrix(&matResult);

	// 행렬의 뺄셈입니다.
	// 행렬의 뺄셈은 함수가 따로 제공되지 않습니다.
	matResult = mat2 - mat1;
	printf("mat2 - mat1의 결과입니다.\n");
	ShowMatrixValue(&matResult);
}

void ScalarMultiplyTest()
{
	D3DXMATRIXA16 mat;
	for (INT32 i = 0; i < 4; ++i)
	{
		SetMatrixRow(&mat, i, 1.0f, 2.0f, 3.0f, 4.0f);
	}

	D3DXMATRIXA16 matResult;
	ZeroMatrix(&matResult);

	// 행렬의 스칼라 곱셈입니다.
	// 행렬의 스칼라 곱셈은 함수가 따로 제공되지 않습니다.
	matResult = mat * 2.0f;
	printf("mat * 2.0f의 결과입니다.\n");
	ShowMatrixValue(&matResult);
}

void ScalarDivideTest()
{
	D3DXMATRIXA16 mat;
	for (INT32 i = 0; i < 4; ++i)
	{
		SetMatrixRow(&mat, i, 2.0f, 4.0f, 6.0f, 8.0f);
	}

	D3DXMATRIXA16 matResult;
	ZeroMatrix(&matResult);

	// 행렬의 스칼라 나눗셈입니다.
	// 행렬의 스칼라 나눗셈은 함수가 따로 제공되지 않습니다.
	matResult = mat / 2.0f;
	printf("mat / 2.0f의 결과입니다.\n");
	ShowMatrixValue(&matResult);
}

void IdentityTest()
{
	D3DXMATRIXA16 mat;

	// 단위행렬로 만듭니다.
	D3DXMatrixIdentity(&mat);

	// 단위행렬의 값을 출력합니다.
	printf("단위행렬의 값을 출력합니다.\n");
	ShowMatrixValue(&mat);
}

void TransposeTest()
{
	D3DXMATRIXA16 mat;
	for (INT32 i = 0; i < 4; ++i)
	{
		SetMatrixRow(&mat, i, 1.0f, 2.0f, 3.0f, 4.0f);
	}

	printf("전치되기 전 mat의 값을 출력합니다.\n");
	ShowMatrixValue(&mat);

	// mat을 전치시킵니다.
	// 행과 열의 값이 서로 교환됩니다.
	D3DXMatrixTranspose(&mat, &mat);

	// 전치된 행렬의 값을 출력합니다.
	printf("전치된 후 mat의 값을 출력합니다.\n");
	ShowMatrixValue(&mat);
}

void MultiplyTest()
{
	D3DXMATRIXA16 mat1;
	for (INT32 i = 0; i < 4; ++i)
	{
		SetMatrixRow(&mat1, i, 1.0f, 2.0f, 1.0f, 2.0f);
	}

	D3DXMATRIXA16 mat2;
	for (INT32 i = 0; i < 4; ++i)
	{
		SetMatrixRow(&mat2, i, 2.0f, 1.0f, 2.0f, 1.0f);
	}

	// 두 행렬을 서로 곱합니다.
	// 행렬의 곱셈은 벡터의 내적과 외적처럼 특별하게 취급합니다.
	D3DXMATRIXA16 matResult = mat1 * mat2;
	printf("mat1 * mat2의 결과입니다.\n");
	ShowMatrixValue(&matResult);

	// 함수를 사용해도 같은 결과입니다.
	ZeroMatrix(&matResult);
	D3DXMatrixMultiply(&matResult, &mat1, &mat2);
	printf("mat1 * mat2의 결과입니다.\n");
	ShowMatrixValue(&matResult);

	// 두 행렬을 곱한 뒤에 전치시키는 함수도 있습니다.
	ZeroMatrix(&matResult);
	D3DXMatrixMultiplyTranspose(&matResult, &mat1, &mat2);
	printf("mat1 * mat2를 전치시킨 결과입니다.\n");
	ShowMatrixValue(&matResult);
}

void IdentityMultiplyTest()
{
	D3DXMATRIXA16 mat1;
	for (INT32 i = 0; i < 4; ++i)
	{
		SetMatrixRow(&mat1, i, 1.0f, 2.0f, 1.0f, 2.0f);
	}

	D3DXMATRIXA16 mat2;
	for (INT32 i = 0; i < 4; ++i)
	{
		SetMatrixRow(&mat2, i, 2.0f, 1.0f, 2.0f, 1.0f);
	}

	// 행렬의 곱셈은 순서가 중요합니다.
	// (A * B)와 (B * A)의 결과는 다릅니다. 벡터의 외적과 같죠.
	// 하지만 스칼라 곱셈에서는 순서에 영향을 받지 않습니다.
	D3DXMATRIXA16 matResult = mat1 * mat2;
	printf("mat1 * mat2의 결과입니다.\n");
	ShowMatrixValue(&matResult);

	// 함수를 사용해도 같은 결과입니다.
	ZeroMatrix(&matResult);
	D3DXMatrixMultiply(&matResult, &mat2, &mat1);
	printf("mat2 * mat1의 결과입니다.\n");
	ShowMatrixValue(&matResult);

	// 단위행렬은 곱하는 순서에 영향을 받지 않습니다.
	D3DXMatrixIdentity(&mat2);
	matResult = mat1 * mat2;
	printf("mat1 * mat2의 결과입니다.\n");
	ShowMatrixValue(&matResult);

	matResult = mat2 * mat1;
	printf("mat2 * mat1의 결과입니다.\n");
	ShowMatrixValue(&matResult);
}

void DeterminantTest()
{
	D3DXMATRIXA16 mat;
	SetMatrix1stRow(&mat, 1.0f, 2.0f, 3.0f, 4.0f);
	SetMatrix2ndRow(&mat, 4.0f, 3.0f, 2.0f, 1.0f);
	SetMatrix3rdRow(&mat, 3.0f, 1.0f, 2.0f, 4.0f);
	SetMatrix4thRow(&mat, 2.0f, 1.0f, 4.0f, 3.0f);

	printf("행렬식을 구합니다.\n");
	FLOAT rDet = D3DXMatrixDeterminant(&mat);
	printf("구한 행렬식 : %.2f\n", rDet);

	// 행렬식이 0이 아니라면 역행렬이 존재하는 가역행렬입니다.
}

void InverseTest()
{
	D3DXMATRIXA16 mat;
	SetMatrix1stRow(&mat, 1.0f, 2.0f, 3.0f, 4.0f);
	SetMatrix2ndRow(&mat, 4.0f, 3.0f, 2.0f, 1.0f);
	SetMatrix3rdRow(&mat, 3.0f, 1.0f, 2.0f, 4.0f);
	SetMatrix4thRow(&mat, 2.0f, 1.0f, 4.0f, 3.0f);
	
	// 역행렬을 구합니다.
	FLOAT rDet = 0.0f;
	D3DXMATRIXA16 matResult;
	ZeroMatrix(&matResult);
	D3DXMatrixInverse(&matResult, &rDet, &mat);
	
	printf("역행렬을 구합니다.\n");
	printf("행렬식은 %.2f입니다.\n", rDet);
	ShowMatrixValue(&matResult);

	rDet = 0.0f;
	printf("행렬식을 구합니다.\n");
	rDet = D3DXMatrixDeterminant(&mat);
	printf("구한 행렬식 : %.2f\n", rDet);
}
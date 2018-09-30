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
void ZeroVector(D3DXVECTOR3* pV);
void SetVector(D3DXVECTOR3* pV, FLOAT rX, FLOAT rY, FLOAT rZ);

// r => row
// c => column
void ZeroMatrix(D3DXMATRIXA16* pMat);
void SetMatrixRow(D3DXMATRIXA16* pMat, INT32 r, FLOAT c1, FLOAT c2, FLOAT c3, FLOAT c4);
void SetMatrix1stRow(D3DXMATRIXA16* pMat, FLOAT c11, FLOAT c12, FLOAT c13, FLOAT c14);
void SetMatrix2ndRow(D3DXMATRIXA16* pMat, FLOAT c21, FLOAT c22, FLOAT c23, FLOAT c24);
void SetMatrix3rdRow(D3DXMATRIXA16* pMat, FLOAT c31, FLOAT c32, FLOAT c33, FLOAT c34);
void SetMatrix4thRow(D3DXMATRIXA16* pMat, FLOAT c41, FLOAT c42, FLOAT c43, FLOAT c44);
void ShowMatrixValue(D3DXMATRIXA16* pMat);

void VectorMultiplyMatrixTest();
void PointMultiplyMatrixTest();
void VectorMultiplyTransformationMatrix();

// ====================================================================================
INT32 main()
{
	//VectorMultiplyMatrixTest();
	//PointMultiplyMatrixTest();
	//VectorMultiplyTransformationMatrix();

	return 0;
}

void ZeroVector(D3DXVECTOR3 * pV)
{
	NULLCHK_RETURN_NOCOMENT(pV);

	// 벡터는 이렇게 값을 설정합니다.
	pV->x = 0.0f;
	pV->y = 0.0f;
	pV->z = 0.0f;
}

void SetVector(D3DXVECTOR3* pV, FLOAT rX, FLOAT rY, FLOAT rZ)
{
	NULLCHK_RETURN_NOCOMENT(pV);

	// 벡터는 이렇게 값을 설정합니다.
	pV->x = rX;
	pV->y = rY;
	pV->z = rZ;
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
	if ((r < 0) ||
		(r > 3))
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

void VectorMultiplyMatrixTest()
{
	D3DXVECTOR3 v(2.0f, 3.0f, 1.0f);
	D3DXMATRIXA16 mat;

	D3DXMatrixIdentity(&mat);
	SetMatrix2ndRow(&mat, 2.0f, 4.0f, 1.0f, 1.0f);

	ShowMatrixValue(&mat);
	printf("변환되기 전 : (%.2f, %.2f, %.2f)\n", v.x, v.y, v.z);

	// 벡터와 행렬의 곱셈입니다.
	// 변환되기 이전에 해당됩니다.
	D3DXVec3TransformNormal(&v, &v, &mat);

	printf("변환된 후   : (%.2f, %.2f, %.2f)\n\n", v.x, v.y, v.z);
}

void PointMultiplyMatrixTest()
{
	D3DXVECTOR3 v(2.0f, 3.0f, 1.0f);
	D3DXMATRIXA16 mat;

	D3DXMatrixIdentity(&mat);
	SetMatrix4thRow(&mat, 2.0f, 4.0f, 1.0f, 1.0f);

	ShowMatrixValue(&mat);
	printf("변환되기 전 : (%.2f, %.2f, %.2f)\n", v.x, v.y, v.z);

	// 벡터와 행렬의 곱셈입니다.
	// 변환되기 이전에 해당됩니다.
	D3DXVec3TransformCoord(&v, &v, &mat);

	printf("변환된 후   : (%.2f, %.2f, %.2f)\n\n", v.x, v.y, v.z);
}

void VectorMultiplyTransformationMatrix()
{
	D3DXVECTOR3 v(2.0f, 3.0f, 1.0f);

	// 가상 공간의 행렬입니다.
	// 월드행렬이라고 합니다.
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);

	// 행렬의 곱셈에는 정해진 순서가 있는데
	// 크기변환행렬 * 회전행렬(자전) * 이동행렬 * 회전행렬(공전) * 부모행렬
	// 지금 당장은 부모행렬은 생각하지 않아도 됩니다.
	// 회전행렬은 곱하는 위치에 따라 자전과 공전으로 구분됩니다.

	// 크기변환행렬입니다.
	D3DXMATRIXA16 matScale;
	D3DXMatrixScaling(&matScale, 2.0f, 2.0f, 0.0f);
	//ShowMatrixValue(&matScale);

	// 회전행렬(자전)입니다.
	D3DXMATRIXA16 matRotation;
	D3DXMatrixRotationZ(&matRotation, D3DXToRadian(-30.0f));
	//ShowMatrixValue(&matRotation);

	// 이동행렬입니다.
	D3DXMATRIXA16 matTranslation;
	D3DXMatrixTranslation(&matTranslation, 0.4f, 0.4f, 0.0f);
	//ShowMatrixValue(&matTranslation);

	// 회전행렬(공전)입니다.
	D3DXMATRIXA16 matRevolution;
	D3DXMatrixRotationZ(&matRevolution, D3DXToRadian(-60.0f));
	//ShowMatrixValue(&matRevolution);

	// 순서에 따라 변환행렬을 곱하고 월드행렬에 넣습니다.
	matWorld = matScale * matRotation * matTranslation * matRevolution;
	ShowMatrixValue(&matWorld);

	printf("변환되기 전 : (%.2f, %.2f, %.2f)\n", v.x, v.y, v.z);

	// 벡터와 행렬의 곱셈입니다.
	// 변환되기 이전에 해당됩니다.
	D3DXVec3TransformCoord(&v, &v, &matWorld);

	printf("변환된 후   : (%.2f, %.2f, %.2f)\n\n", v.x, v.y, v.z);
}
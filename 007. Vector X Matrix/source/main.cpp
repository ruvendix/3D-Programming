#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


// ====================================================================================
// 함수 선언부입니다.
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

void VectorMultiplyMatrixTest()
{
	D3DXVECTOR3 v(2.0f, 3.0f, 1.0f);
	D3DXMATRIXA16 mat;

	D3DXMatrixIdentity(&mat);
	RX::SetMatrix2ndRow(&mat, 2.0f, 4.0f, 1.0f, 1.0f);

	RX::ShowMatrixValue(&mat);
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
	RX::SetMatrix4thRow(&mat, 2.0f, 4.0f, 1.0f, 1.0f);

	RX::ShowMatrixValue(&mat);
	printf("변환되기 전 : (%.2f, %.2f, %.2f)\n", v.x, v.y, v.z);

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
	//RX::ShowMatrixValue(&matScale);

	// 회전행렬(자전)입니다.
	D3DXMATRIXA16 matRotation;
	D3DXMatrixRotationZ(&matRotation, D3DXToRadian(-30.0f));
	//RX::ShowMatrixValue(&matRotation);

	// 이동행렬입니다.
	D3DXMATRIXA16 matTranslation;
	D3DXMatrixTranslation(&matTranslation, 0.4f, 0.4f, 0.0f);
	//RX::ShowMatrixValue(&matTranslation);

	// 회전행렬(공전)입니다.
	D3DXMATRIXA16 matRevolution;
	D3DXMatrixRotationZ(&matRevolution, D3DXToRadian(-60.0f));
	//RX::ShowMatrixValue(&matRevolution);

	// 순서에 따라 변환행렬을 곱하고 월드행렬에 넣습니다.
	matWorld = matScale * matRotation * matTranslation * matRevolution;
	RX::ShowMatrixValue(&matWorld);

	printf("변환되기 전 : (%.2f, %.2f, %.2f)\n", v.x, v.y, v.z);

	// 벡터와 행렬의 곱셈입니다.
	// 변환되기 이전에 해당됩니다.
	D3DXVec3TransformCoord(&v, &v, &matWorld);

	printf("변환된 후   : (%.2f, %.2f, %.2f)\n\n", v.x, v.y, v.z);
}
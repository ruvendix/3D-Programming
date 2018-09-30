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
	if ((r < 0) ||
		(r > 3))
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
	printf("��ȯ�Ǳ� �� : (%.2f, %.2f, %.2f)\n", v.x, v.y, v.z);

	// ���Ϳ� ����� �����Դϴ�.
	// ��ȯ�Ǳ� ������ �ش�˴ϴ�.
	D3DXVec3TransformNormal(&v, &v, &mat);

	printf("��ȯ�� ��   : (%.2f, %.2f, %.2f)\n\n", v.x, v.y, v.z);
}

void PointMultiplyMatrixTest()
{
	D3DXVECTOR3 v(2.0f, 3.0f, 1.0f);
	D3DXMATRIXA16 mat;

	D3DXMatrixIdentity(&mat);
	SetMatrix4thRow(&mat, 2.0f, 4.0f, 1.0f, 1.0f);

	ShowMatrixValue(&mat);
	printf("��ȯ�Ǳ� �� : (%.2f, %.2f, %.2f)\n", v.x, v.y, v.z);

	// ���Ϳ� ����� �����Դϴ�.
	// ��ȯ�Ǳ� ������ �ش�˴ϴ�.
	D3DXVec3TransformCoord(&v, &v, &mat);

	printf("��ȯ�� ��   : (%.2f, %.2f, %.2f)\n\n", v.x, v.y, v.z);
}

void VectorMultiplyTransformationMatrix()
{
	D3DXVECTOR3 v(2.0f, 3.0f, 1.0f);

	// ���� ������ ����Դϴ�.
	// ��������̶�� �մϴ�.
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);

	// ����� �������� ������ ������ �ִµ�
	// ũ�⺯ȯ��� * ȸ�����(����) * �̵���� * ȸ�����(����) * �θ����
	// ���� ������ �θ������ �������� �ʾƵ� �˴ϴ�.
	// ȸ������� ���ϴ� ��ġ�� ���� ������ �������� ���е˴ϴ�.

	// ũ�⺯ȯ����Դϴ�.
	D3DXMATRIXA16 matScale;
	D3DXMatrixScaling(&matScale, 2.0f, 2.0f, 0.0f);
	//ShowMatrixValue(&matScale);

	// ȸ�����(����)�Դϴ�.
	D3DXMATRIXA16 matRotation;
	D3DXMatrixRotationZ(&matRotation, D3DXToRadian(-30.0f));
	//ShowMatrixValue(&matRotation);

	// �̵�����Դϴ�.
	D3DXMATRIXA16 matTranslation;
	D3DXMatrixTranslation(&matTranslation, 0.4f, 0.4f, 0.0f);
	//ShowMatrixValue(&matTranslation);

	// ȸ�����(����)�Դϴ�.
	D3DXMATRIXA16 matRevolution;
	D3DXMatrixRotationZ(&matRevolution, D3DXToRadian(-60.0f));
	//ShowMatrixValue(&matRevolution);

	// ������ ���� ��ȯ����� ���ϰ� ������Ŀ� �ֽ��ϴ�.
	matWorld = matScale * matRotation * matTranslation * matRevolution;
	ShowMatrixValue(&matWorld);

	printf("��ȯ�Ǳ� �� : (%.2f, %.2f, %.2f)\n", v.x, v.y, v.z);

	// ���Ϳ� ����� �����Դϴ�.
	// ��ȯ�Ǳ� ������ �ش�˴ϴ�.
	D3DXVec3TransformCoord(&v, &v, &matWorld);

	printf("��ȯ�� ��   : (%.2f, %.2f, %.2f)\n\n", v.x, v.y, v.z);
}
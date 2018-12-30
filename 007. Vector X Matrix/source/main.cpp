#include "base_project.h"
#include "main.h"
#include "global_variable_declaration.h"


// ====================================================================================
// �Լ� ������Դϴ�.
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
	RX::SetMatrix4thRow(&mat, 2.0f, 4.0f, 1.0f, 1.0f);

	RX::ShowMatrixValue(&mat);
	printf("��ȯ�Ǳ� �� : (%.2f, %.2f, %.2f)\n", v.x, v.y, v.z);

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
	//RX::ShowMatrixValue(&matScale);

	// ȸ�����(����)�Դϴ�.
	D3DXMATRIXA16 matRotation;
	D3DXMatrixRotationZ(&matRotation, D3DXToRadian(-30.0f));
	//RX::ShowMatrixValue(&matRotation);

	// �̵�����Դϴ�.
	D3DXMATRIXA16 matTranslation;
	D3DXMatrixTranslation(&matTranslation, 0.4f, 0.4f, 0.0f);
	//RX::ShowMatrixValue(&matTranslation);

	// ȸ�����(����)�Դϴ�.
	D3DXMATRIXA16 matRevolution;
	D3DXMatrixRotationZ(&matRevolution, D3DXToRadian(-60.0f));
	//RX::ShowMatrixValue(&matRevolution);

	// ������ ���� ��ȯ����� ���ϰ� ������Ŀ� �ֽ��ϴ�.
	matWorld = matScale * matRotation * matTranslation * matRevolution;
	RX::ShowMatrixValue(&matWorld);

	printf("��ȯ�Ǳ� �� : (%.2f, %.2f, %.2f)\n", v.x, v.y, v.z);

	// ���Ϳ� ����� �����Դϴ�.
	// ��ȯ�Ǳ� ������ �ش�˴ϴ�.
	D3DXVec3TransformCoord(&v, &v, &matWorld);

	printf("��ȯ�� ��   : (%.2f, %.2f, %.2f)\n\n", v.x, v.y, v.z);
}
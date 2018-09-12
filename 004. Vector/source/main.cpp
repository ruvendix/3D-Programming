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

void AddTest()
{
	// 3차원 벡터의 기본형입니다.
	// 쓰레기값이 있으므로 초기화는 필수입니다.
	D3DXVECTOR3 vPos1(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vPos2(0.0f, 0.0f, 0.0f);

	// 영벡터로 만드는 초기화인데
	// 이렇게 함수를 만들어서 할 수도 있습니다.
	ZeroVector(&vPos1);
	ZeroVector(&vPos2);

	// 벡터의 값을 설정하는 건데
	// 이렇게 함수를 만들어서 할 수도 있습니다.
	SetVector(&vPos1, 1.0f, 2.0f, 3.0f);

	// 벡터의 덧셈입니다.
	D3DXVECTOR3 vResult(0.0f, 0.0f, 0.0f);
	vResult = vPos1 + vPos2;
	printf("vPos1 + vPos2 = (%.2f, %.2f, %.2f)\n", vResult.x, vResult.y, vResult.z);

	// 함수를 사용해도 같은 결과입니다.
	ZeroVector(&vResult);
	D3DXVec3Add(&vResult, &vPos1, &vPos2);
	printf("vPos1 + vPos2 = (%.2f, %.2f, %.2f)\n", vResult.x, vResult.y, vResult.z);
	printf("=========================================\n");
}

void SubtractTest()
{
	// 3차원 벡터의 기본형입니다.
	// 쓰레기값이 있으므로 초기화는 필수입니다.
	D3DXVECTOR3 vPos1(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vPos2(0.0f, 0.0f, 0.0f);

	// 영벡터로 만드는 초기화인데
	// 이렇게 함수를 만들어서 할 수도 있습니다.
	ZeroVector(&vPos1);
	ZeroVector(&vPos2);

	// 벡터의 값을 설정하는 건데
	// 이렇게 함수를 만들어서 할 수도 있습니다.
	SetVector(&vPos2, 1.0f, 2.0f, 3.0f);

	// 벡터의 뺄셈입니다.
	D3DXVECTOR3 vResult(0.0f, 0.0f, 0.0f);
	vResult = vPos1 - vPos2;
	printf("vPos1 - vPos2 = (%.2f, %.2f, %.2f)\n", vResult.x, vResult.y, vResult.z);

	// 함수를 사용해도 같은 결과입니다.
	ZeroVector(&vResult);
	D3DXVec3Subtract(&vResult, &vPos1, &vPos2);
	printf("vPos1 - vPos2 = (%.2f, %.2f, %.2f)\n", vResult.x, vResult.y, vResult.z);
	printf("=========================================\n");
}

void ScalarMultiplyTest()
{
	// 3차원 벡터의 기본형입니다.
	// 쓰레기값이 있으므로 초기화는 필수입니다.
	D3DXVECTOR3 vPos(0.0f, 0.0f, 0.0f);

	// 영벡터로 만드는 초기화인데
	// 이렇게 함수를 만들어서 할 수도 있습니다.
	ZeroVector(&vPos);

	// 벡터의 값을 설정하는 건데
	// 이렇게 함수를 만들어서 할 수도 있습니다.
	SetVector(&vPos, 1.0f, 2.0f, 3.0f);

	// 벡터의 스칼라 곱셈입니다.
	D3DXVECTOR3 vResult(0.0f, 0.0f, 0.0f);
	vResult = vPos * 2;
	printf("vPos * 2 = (%.2f, %.2f, %.2f)\n", vResult.x, vResult.y, vResult.z);

	// 함수를 사용해도 같은 결과입니다.
	ZeroVector(&vResult);
	D3DXVec3Scale(&vResult, &vPos, 2.0f);
	printf("vPos * 2 = (%.2f, %.2f, %.2f)\n", vResult.x, vResult.y, vResult.z);
	printf("=========================================\n");
}

void ScalarDivideTest()
{
	// 3차원 벡터의 기본형입니다.
	// 쓰레기값이 있으므로 초기화는 필수입니다.
	D3DXVECTOR3 vPos(0.0f, 0.0f, 0.0f);

	// 영벡터로 만드는 초기화인데
	// 이렇게 함수를 만들어서 할 수도 있습니다.
	ZeroVector(&vPos);

	// 벡터의 값을 설정하는 건데
	// 이렇게 함수를 만들어서 할 수도 있습니다.
	SetVector(&vPos, 1.0f, 2.0f, 3.0f);

	// 벡터의 스칼라 나눗셈입니다.
	D3DXVECTOR3 vResult(0.0f, 0.0f, 0.0f);
	vResult = vPos / 2;
	printf("vPos / 2 = (%.2f, %.2f, %.2f)\n", vResult.x, vResult.y, vResult.z);

	// 함수를 사용해도 같은 결과입니다.
	ZeroVector(&vResult);
	D3DXVec3Scale(&vResult, &vPos, 1.0f / 2.0f);
	printf("vPos / 2 = (%.2f, %.2f, %.2f)\n", vResult.x, vResult.y, vResult.z);
	printf("=========================================\n");
}
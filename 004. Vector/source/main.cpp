#include "base_project.h"
#include "global_variable_declaration.h"
#include "main.h"


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

void NormalizeTest()
{
	// 3차원 벡터의 기본형입니다.
	// 쓰레기값이 있으므로 초기화는 필수입니다.
	D3DXVECTOR3 vPos(3.0f, 3.0f, 3.0f);

	// 벡터의 길이를 제곱으로 구합니다.
	// 제곱근 연산은 오버헤드가 크므로 단순한 비교일 때는
	// 길이를 제곱으로 판단하는 게 좋습니다.
	FLOAT rLengthSq = D3DXVec3LengthSq(&vPos);
	printf("rLengthSq = %.2f\n", rLengthSq);

	// 벡터의 길이를 구합니다.
	// 위에서 구한 길이에 루트를 씌우는 거라고 생각하면 됩니다.
	FLOAT rLength = D3DXVec3Length(&vPos);
	printf("rLength   = %.2f\n", rLength);

	// 벡터를 정규화합니다.
	// 즉, 벡터를 단위벡터로 만듭니다.
	// 함수의 인자는 오른쪽부터 매개변수에 넣으므로
	// 일반적으로 정규화는 이렇게 같은 변수로 처리합니다.
	// 법선벡터를 구할 때 굉장히 많이 사용하므로 익숙해져야 합니다.
	D3DXVec3Normalize(&vPos, &vPos);
	printf("정규화된 vPos = (%.2f, %.2f, %.2f)\n", vPos.x, vPos.y, vPos.z);
	printf("=========================================\n");
}

void DotProductTest()
{
	// 3차원 벡터의 기본형입니다.
	// 쓰레기값이 있으므로 초기화는 필수입니다.
	D3DXVECTOR3 vPos1(1.0f, 1.0f, 1.0f);
	D3DXVECTOR3 vPos2(2.0f, 0.0f, 3.0f);

	// 두 벡터를 이용해서 내적을 구합니다.
	// 대수학적으로는 각 성분끼리 빼는 것인데 위치벡터 기준입니다.
	// 기하학적으로는 두 벡터가 이루는 각과 정사영을 알 수 있습니다.
	// 둘 다 알아야 하지만 초반에는 대수학적인 의미만 알아도 무난합니다.
	FLOAT rResult = D3DXVec3Dot(&vPos1, &vPos2);
	printf("vPos1 · vPos2 = %.2f\n", rResult);
	
	// 내적은 순서를 바꿔도 결과가 동일합니다.
	rResult = D3DXVec3Dot(&vPos2, &vPos1);
	printf("vPos2 · vPos1 = %.2f\n", rResult);
	printf("=========================================\n");

	// 내적이 양수면 두 벡터가 이루는 각은 예각이고,
	// 내적이 0이면 두 벡터가 이루는 각은 직각이고,
	// 내적이 음수면 두 벡터가 이루는 각은 둔각입니다.
	//
	// 나중에 카메라를 배우게 되는데 카메라 쪽으로 향하는 벡터와
	// 폴리곤의 법선벡터와의 내적에 따라 렌더링 컬링이 결정됩니다.
}

void CrossProductTest()
{
	// 3차원 벡터의 기본형입니다.
	// 쓰레기값이 있으므로 초기화는 필수입니다.
	D3DXVECTOR3 vPos1(1.0f, 1.0f, 1.0f);
	D3DXVECTOR3 vPos2(2.0f, 0.0f, 3.0f);

	// 두 벡터를 이용해서 외적을 구합니다.
	// 대수학적으로는 각 성분끼리 연산하는 것인데 위치벡터 기준입니다.
	// 기하학적으로는 두 벡터에 수직인 벡터인데 왼손좌표계와 오른손좌표계에 따라 방향이 다릅니다.
	// 여기서 좀 헷갈릴 수 있는데 대수학적으로는 왼손좌표계든 오른손좌표계든 값은 동일합니다.
	// 다만 Z축의 방향이 서로 반대이므로 외적 결과 벡터의 Z값을 주목해야 합니다.
	D3DXVECTOR3 vResult(0.0f, 0.0f, 0.0f);
	D3DXVec3Cross(&vResult, &vPos1, &vPos2);
	printf("vPos1 X vPos2 = (%+5.2f, %+5.2f, %+5.2f)\n", vResult.x, vResult.y, vResult.z);

	// 외적은 순서를 바꾸면 결과가 바뀝니다.
	// 왼손좌표계와 오른손좌표계와는 관련이 없습니다.
	D3DXVec3Cross(&vResult, &vPos2, &vPos1);
	printf("vPos2 X vPos1 = (%+5.2f, %+5.2f, %+5.2f)\n", vResult.x, vResult.y, vResult.z);
	printf("=========================================\n");

	// 평면의 법선벡터를 외적으로 구하게 되는데 중요한 건 방향이므로
	// 굳이 정규화는 하지 않아도 되지만 필요할 때는 정규화를 해야 합니다.
	// 왼손좌표계와 오른손좌표계에 따라 Z축의 방향이 바뀌게 되는데
	// 이 방향에 따라 컬링이 결정됩니다. 컬링에 관해서는 내적에서 설명했습니다.
	// 법선벡터를 구하는 방법이 외적이므로 내적과 연결되게 됩니다.
}
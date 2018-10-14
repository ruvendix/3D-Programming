/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-10-14
 *
 * <파일 내용>
 * 수학과 관련된 함수를 다룹니다.
 *
 ====================================================================================*/
#include "PCH.h"
#include "math_function.h"

namespace RX
{

	DLL_DEFINE void ZeroVector(D3DXVECTOR3 * pV)
	{
		SetVector(pV, 0.0f, 0.0f, 0.0f);
	}

	DLL_DEFINE void SetVector(D3DXVECTOR3 * pV, FLOAT rX, FLOAT rY, FLOAT rZ)
	{
		NULLCHK_RETURN_NOCOMENT(pV);

		// 벡터는 이렇게 값을 설정합니다.
		pV->x = rX;
		pV->y = rY;
		pV->z = rZ;
	}

	DLL_DEFINE void ZeroMatrix(D3DXMATRIXA16* pMat)
	{
		for (INT32 i = 0; i < 4; ++i)
		{
			SetMatrixRow(pMat, i, 0.0f, 0.0f, 0.0f, 0.0f);
		}
	}

	DLL_DEFINE void SetMatrixRow(D3DXMATRIXA16* pMat, INT32 r, FLOAT c1, FLOAT c2, FLOAT c3, FLOAT c4)
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

	DLL_DEFINE void SetMatrix1stRow(D3DXMATRIXA16* pMat, FLOAT c11, FLOAT c12, FLOAT c13, FLOAT c14)
	{
		SetMatrixRow(pMat, 0, c11, c12, c13, c14);
	}

	DLL_DEFINE void SetMatrix2ndRow(D3DXMATRIXA16* pMat, FLOAT c21, FLOAT c22, FLOAT c23, FLOAT c24)
	{
		SetMatrixRow(pMat, 1, c21, c22, c23, c24);
	}

	DLL_DEFINE void SetMatrix3rdRow(D3DXMATRIXA16* pMat, FLOAT c31, FLOAT c32, FLOAT c33, FLOAT c34)
	{
		SetMatrixRow(pMat, 2, c31, c32, c33, c34);
	}

	DLL_DEFINE void SetMatrix4thRow(D3DXMATRIXA16* pMat, FLOAT c41, FLOAT c42, FLOAT c43, FLOAT c44)
	{
		SetMatrixRow(pMat, 3, c41, c42, c43, c44);
	}

} // namespace RX end
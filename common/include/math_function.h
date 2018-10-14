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
#ifndef MATH_FUNCTION_H__
#define MATH_FUNCTION_H__

#include "common.h"

namespace RX
{

	// 벡터
	DLL_DEFINE void ZeroVector(D3DXVECTOR3* pV);
	DLL_DEFINE void SetVector(D3DXVECTOR3* pV, FLOAT rX, FLOAT rY, FLOAT rZ);

	// 행렬
	DLL_DEFINE void ZeroMatrix(D3DXMATRIXA16* pMat);
	DLL_DEFINE void SetMatrixRow(D3DXMATRIXA16* pMat, INT32 r, FLOAT c1, FLOAT c2, FLOAT c3, FLOAT c4);
	DLL_DEFINE void SetMatrix1stRow(D3DXMATRIXA16* pMat, FLOAT c11, FLOAT c12, FLOAT c13, FLOAT c14);
	DLL_DEFINE void SetMatrix2ndRow(D3DXMATRIXA16* pMat, FLOAT c21, FLOAT c22, FLOAT c23, FLOAT c24);
	DLL_DEFINE void SetMatrix3rdRow(D3DXMATRIXA16* pMat, FLOAT c31, FLOAT c32, FLOAT c33, FLOAT c34);
	DLL_DEFINE void SetMatrix4thRow(D3DXMATRIXA16* pMat, FLOAT c41, FLOAT c42, FLOAT c43, FLOAT c44);

} // namespace RX end
#endif
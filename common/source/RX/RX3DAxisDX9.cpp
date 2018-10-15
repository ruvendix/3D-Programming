/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-10-12
 *
 * <파일 내용>
 * 3D 기본 축을 다룹니다.
 *
 ====================================================================================*/
#include "PCH.h"
#include "RX3DAxisDX9.h"

namespace RX
{

	HRESULT RX3DAxisDX9::CreateAxis(FLOAT rDist)
	{
		setPrimitiveType(D3DPT_LINELIST);
		AllocVertexBuffer();

		// X축입니다.
		InsertVertex(-rDist, 0.0f, 0.0f, DXCOLOR_RED);
		InsertVertex(rDist, 0.0f, 0.0f, DXCOLOR_RED);

		// Y축입니다.
		InsertVertex(0.0f, -rDist, 0.0f, DXCOLOR_GREEN);
		InsertVertex(0.0f, rDist, 0.0f, DXCOLOR_GREEN);

		// Z축입니다.
		InsertVertex(0.0f, 0.0f, -rDist, DXCOLOR_BLUE);
		InsertVertex(0.0f, 0.0f, rDist, DXCOLOR_BLUE);

		CreateVertexBuffer();
		return S_OK;
	}

	void RX3DAxisDX9::DrawAxis()
	{
		DrawPrimitive();
	}


} // namespace RX end
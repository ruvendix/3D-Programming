/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <�ۼ� ��¥>
 * 2018-10-12
 *
 * <���� ����>
 * 3D �⺻ ���� �ٷ�ϴ�.
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

		// X���Դϴ�.
		InsertVertex(-rDist, 0.0f, 0.0f, DXCOLOR_RED);
		InsertVertex(rDist, 0.0f, 0.0f, DXCOLOR_RED);

		// Y���Դϴ�.
		InsertVertex(0.0f, -rDist, 0.0f, DXCOLOR_GREEN);
		InsertVertex(0.0f, rDist, 0.0f, DXCOLOR_GREEN);

		// Z���Դϴ�.
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
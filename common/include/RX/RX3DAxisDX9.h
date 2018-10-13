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
#ifndef RX3DAXISDX9_H__
#define RX3DAXISDX9_H__

#include "common.h"
#include "RXVertexBufferDX9.h"

namespace RX
{

	class DLL_DEFINE RX3DAxisDX9
	{
	public:
		RX3DAxisDX9() = default;
		virtual ~RX3DAxisDX9();

		HRESULT CreateAxis(FLOAT rDist);
		void DrawAxis();

	private:
		RX::RXVertexBufferDX9* m_pVB;
	};

} // namespace RX end
#endif
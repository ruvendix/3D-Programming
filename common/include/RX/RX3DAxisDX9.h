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
#include "RXRenderObjectDX9.h"

namespace RX
{

	class DLL_DEFINE RX3DAxisDX9 : public RXRenderObjectDX9
	{
	public:
		RX3DAxisDX9() = default;
		virtual ~RX3DAxisDX9() = default;

		HRESULT CreateAxis(FLOAT rDist);
		void    DrawAxis();
	};

} // namespace RX end
#endif
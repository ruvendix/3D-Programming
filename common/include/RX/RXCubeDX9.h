/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-10-14
 *
 * <파일 내용>
 * 테스트용 큐브를 다룹니다.
 *
 ====================================================================================*/
#ifndef RXCUBEDX9_H__
#define RXCUBEDX9_H__

#include "common.h"
#include "RXVertexBufferDX9.h"

enum class CUBE_DRAW_TYPE : INT32
{
	NORMAL,
	INDEX,
};

namespace RX
{

	class DLL_DEFINE RXCubeDX9
	{
	public:
		RXCubeDX9() = default;
		virtual ~RXCubeDX9();

		HRESULT CreateCube(const D3DXVECTOR3& vMin,
			const D3DXVECTOR3& vMax, CUBE_DRAW_TYPE type = CUBE_DRAW_TYPE::NORMAL);
		void    DrawCube(CUBE_DRAW_TYPE type = CUBE_DRAW_TYPE::NORMAL);

	private:
		RX::RXVertexBufferDX9* m_pVB;
	};

} // namespace RX end
#endif
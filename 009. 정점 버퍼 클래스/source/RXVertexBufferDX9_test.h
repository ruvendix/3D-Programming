/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-10-11
 *
 * <파일 내용>
 * DirectX9의 정점 버퍼를 클래스로 래핑했습니다.
 *
 ====================================================================================*/
#ifndef RXVERTEXBUFFERDX9_TEST_H__
#define RXVERTEXBUFFERDX9_TEST_H__

#include "base_project.h"

// 정점의 형식을 설정합니다.
struct CustomVertex
{
	D3DXVECTOR3 vPos;    // 정점의 좌표입니다.
	DWORD       dwColor; // 정점의 색상입니다.
};

namespace RX
{

	class RXVertexBufferDX9Test
	{
	public:
		RXVertexBufferDX9Test();
		virtual ~RXVertexBufferDX9Test();

		void    InsertVertex(FLOAT rX, FLOAT rY, FLOAT rZ, DWORD dwColor);
		void    DrawPrimitive();
		HRESULT CreateVertexBuffer();

		// ====================================================================================
		// Getter

		// ====================================================================================
		// Setter
		void setFVF(DWORD fvf)
		{
			m_dwFVF = fvf;
		}

	private:
		DWORD m_dwFVF;
		std::vector<CustomVertex> m_vecVertex;
		IDirect3DVertexBuffer9*   m_pVertexBuffer;
	};

} // namespace RX end
#endif
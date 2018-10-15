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
#ifndef RXVERTEXBUFFERDX9_H__
#define RXVERTEXBUFFERDX9_H__

#include "common.h"

// 정점의 구조를 설정합니다.
struct VertexInfo
{
	D3DXVECTOR3 vPos;    // 정점의 좌표입니다.
	DWORD       dwColor; // 정점의 색상입니다.
	static const DWORD FORMAT = D3DFVF_XYZ | D3DFVF_DIFFUSE;
};

namespace RX
{

	class DLL_DEFINE RXVertexBufferDX9
	{
	public:
		RXVertexBufferDX9();
		virtual ~RXVertexBufferDX9();

		void    InsertVertex(FLOAT rX, FLOAT rY, FLOAT rZ, DWORD dwColor);
		void    InsertVertex(const D3DXVECTOR3& vPos, DWORD dwColor);
		HRESULT CreateVertexBuffer();

		// ====================================================================================
		// Getter
		INT32 getVertexCount() const noexcept
		{
			return m_vertexCnt;
		}

		IDirect3DVertexBuffer9* getVB() const noexcept
		{
			return m_pVB;
		}

	private:
		INT32                      m_vertexCnt; // 속도를 위해 미리 저장해둡니다.
		std::vector<VertexInfo>    m_vecVertex;
		IDirect3DVertexBuffer9*    m_pVB;
	};

} // namespace RX end
#endif
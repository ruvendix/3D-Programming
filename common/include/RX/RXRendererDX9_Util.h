/*====================================================================================
*
* Copyright (C) Ruvendix. All Rights Reserved.
*
* <작성 날짜>
* 2018-10-16
*
* <파일 내용>
* DirectX9 렌더러의 보조 함수입니다.
*
====================================================================================*/
#ifndef RXRENDERERDX9_UTILITY_H__
#define RXRENDERERDX9_UTILITY_H__

namespace RX
{

	INT32 CalcPrimitiveCount(D3DPRIMITIVETYPE primitiveType, INT32 vertexCnt);
	bool AdjustFullScreenInfo(INT32 adapterIndex, D3DPRESENT_PARAMETERS* pD3DPP,
		const D3DDISPLAYMODE& mainDisplayMode);
	bool AdjustMSAAInfo(INT32 adapterIndex, D3DPRESENT_PARAMETERS* pD3DPP);
	bool AdjustDepthStencilInfo(INT32 adapterIndex, D3DPRESENT_PARAMETERS* pD3DPP,
		INT32 depthBits, INT32 stencilBits);
	bool CheckAvailableAllDXT(INT32 adapterIndex, D3DFORMAT surfaceFormat);

} // namespace RX end

#endif
/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-12-17
 *
 * <파일 내용>
 * 사각형을 다룹니다.
 *
 ====================================================================================*/
#ifndef MATH_RXRECT_H__
#define MATH_RXRECT_H__

#include "common.h"

namespace RX
{

	class DLL_DEFINE RXRect
	{
	public:
		RXRect()
		{
			ZeroRect();
		}

		RXRect(INT32 left, INT32 top, INT32 right, INT32 bottom)
		{
			setRect(left, top, right, bottom);
		}

		INT32 CalcWidth() const noexcept
		{
			return m_right - m_left;
		}

		INT32 CalcHeight() const noexcept
		{
			return m_bottom - m_top;
		}

		const RXRect* CopyRect(const RXRect& src)
		{
			*this = src;
			return this;
		}

		void ZeroRect()
		{
			::ZeroMemory(this, sizeof(RXRect));
		}

		RECT* ConvertToWinAPIRect(RECT* pRt);

		const RXRect* ConvertToRXRect(const RECT& rt)
		{
			setRect(rt.left, rt.top, rt.right, rt.bottom);
			return this;
		}

		// ====================================================================================
		// Getter
		INT32 getLeft() const noexcept
		{
			return m_left;
		}

		INT32 getTop() const noexcept
		{
			return m_top;
		}

		INT32 getRight() const noexcept
		{
			return m_right;
		}

		INT32 getBottom() const noexcept
		{
			return m_bottom;
		}

		const RXRect* getRect()
		{
			return this;
		}

		// ====================================================================================
		// Setter
		void setLeft(INT32 left)
		{
			m_left = left;
		}

		void setTop(INT32 top)
		{
			m_top = top;
		}

		void setRight(INT32 right)
		{
			m_right = right;
		}

		void setBottom(INT32 bottom)
		{
			m_bottom = bottom;
		}

		void setRect(INT32 left, INT32 top, INT32 right, INT32 bottom)
		{
			m_left   = left;
			m_top    = top;
			m_right  = right;
			m_bottom = bottom;
		}

	private:
		INT32 m_left;
		INT32 m_top;
		INT32 m_right;
		INT32 m_bottom;
	};

} // namespace RX end
#endif
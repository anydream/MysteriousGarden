/************************************************************************/
// 创建时间: 080918
// 最后确定: 081028

#ifndef _Quad_h_
#define _Quad_h_

#include "../AnyPlay.h"
#include "Vector4.h"

namespace AnyPlay
{
	class Quad;

	class Rect
	{
	public:
		Rect() { m_Left = m_Right = m_Top = m_Bottom = 0.0f; }
		Rect(float l, float t, float r, float b) { m_Left = l; m_Top = t; m_Right = r; m_Bottom = b; }
		Rect(const Rect& rc) { *this = rc; }
		Rect(const RECT& rc) { *this = rc; }
		
		float GetWidth() const { return m_Right - m_Left; }
		float GetHeight() const { return m_Bottom - m_Top; }
		
		void OffsetRect(const Point& off);
		void SetWidth(float w) { m_Right = m_Left + w; }
		void SetHeight(float h) { m_Bottom = m_Top + h; }
		
		float GetArea() const { return GetWidth() * GetHeight(); }
		
		bool GetRECT(LPRECT rc) const;
		RECT GetRECT() const;
		Quad GetQuad() const;
		void NormalRect();
		bool Hit(const Point& p);
		bool Hit(const Rect& rc, Rect* pOutRC, float* pArea);
		
		Rect& operator = (const Rect& rc);
		Rect& operator = (const RECT& rc);
		
		bool operator == (const Rect& rc) const;
		bool operator != (const Rect& rc) const { return !(*this == rc); }

		Rect operator + (const Rect& rc) const { return Rect(m_Left + rc.m_Left, m_Top + rc.m_Top, m_Right + rc.m_Right, m_Bottom + rc.m_Bottom); }
		Rect operator - (const Rect& rc) const { return Rect(m_Left - rc.m_Left, m_Top - rc.m_Top, m_Right - rc.m_Right, m_Bottom - rc.m_Bottom); }
		Rect operator * (const Rect& rc) const { return Rect(m_Left * rc.m_Left, m_Top * rc.m_Top, m_Right * rc.m_Right, m_Bottom * rc.m_Bottom); }
		Rect operator / (const Rect& rc) const { return Rect(m_Left / rc.m_Left, m_Top / rc.m_Top, m_Right / rc.m_Right, m_Bottom / rc.m_Bottom); }

		Rect operator += (const Rect& rc) { m_Left += rc.m_Left; m_Top += rc.m_Top; m_Right += rc.m_Right; m_Bottom += rc.m_Bottom; }
		Rect operator -= (const Rect& rc) { m_Left -= rc.m_Left; m_Top -= rc.m_Top; m_Right -= rc.m_Right; m_Bottom -= rc.m_Bottom; }
		Rect operator *= (const Rect& rc) { m_Left *= rc.m_Left; m_Top *= rc.m_Top; m_Right *= rc.m_Right; m_Bottom *= rc.m_Bottom; }
		Rect operator /= (const Rect& rc) { m_Left /= rc.m_Left; m_Top /= rc.m_Top; m_Right /= rc.m_Right; m_Bottom /= rc.m_Bottom; }

		Rect operator + (const Point& point) const { return Rect(m_Left + point.x, m_Top + point.y, m_Right + point.x, m_Bottom + point.y); }

	public:
		float	m_Left;
		float	m_Right;
		float	m_Top;
		float	m_Bottom;
	};

	//////////////////////////////////////////////////////////////////////////
	class Quad
	{
	public:
		Quad() {}
		Quad(const Quad& q) { *this = q; }
		Quad(const Rect& r) { *this = r; }
		Quad(const Vector4& lt, const Vector4& rt, const Vector4& rb, const Vector4& lb) { m_lt = lt; m_rt = rt; m_rb = rb; m_lb = lb; }

		void	OffsetQuad(const Point& off);
		Rect	GetRect() const { return Rect(m_lt.x, m_lt.y, m_rb.x, m_rb.y); }
		Quad&	operator = (const Quad& q);
		Quad&	operator = (const Rect& rc) { *this = rc.GetQuad(); return *this; }
		bool	operator == (const Quad& q) const;
		bool	operator != (const Quad& q) const { return !(*this == q); }

		Quad	operator + (const Quad& q) const { return Quad(m_lt + q.m_lt, m_rt + q.m_rt, m_rb + q.m_rb, m_lb + q.m_lb); }
		Quad	operator - (const Quad& q) const { return Quad(m_lt - q.m_lt, m_rt - q.m_rt, m_rb - q.m_rb, m_lb - q.m_lb); }
		Quad	operator * (const Quad& q) const { return Quad(m_lt * q.m_lt, m_rt * q.m_rt, m_rb * q.m_rb, m_lb * q.m_lb); }
		Quad	operator / (const Quad& q) const { return Quad(m_lt / q.m_lt, m_rt / q.m_rt, m_rb / q.m_rb, m_lb / q.m_lb); }

		Quad	operator + (const Point& p) const { return Quad(m_lt + p, m_rt + p, m_rb + p, m_lb + p); }
	public:
		Vector4	m_lt, m_rt, m_rb, m_lb;
	};

	Rect Area(float x, float y, float w, float h);
}

#endif
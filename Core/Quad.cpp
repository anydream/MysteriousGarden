/************************************************************************/
// 创建时间: 080918
// 最后确定: 081028

#include "Quad.h"

#pragma warning(disable : 4244)

namespace AnyPlay
{
	void Quad::OffsetQuad(const Point& off)
	{
		m_lt.x += off.x;
		m_rt.x += off.x;
		m_rb.x += off.x;
		m_lb.x += off.x;
		
		m_lt.y += off.y;
		m_rt.y += off.y;
		m_rb.y += off.y;
		m_lb.y += off.y;
	}

	Quad& Quad::operator = (const Quad& q)
	{
		if(&q == this) return *this;
		m_lt = q.m_lt;
		m_rt = q.m_rt;
		m_rb = q.m_rb;
		m_lb = q.m_lb;
		return *this;
	}

	bool Quad::operator == (const Quad& q) const
	{
		if(&q == this) return true;
		if( (m_lt == q.m_lt) &&
			(m_rt == q.m_rt) &&
			(m_rb == q.m_rb) &&
			(m_lb == q.m_lb) )
			return true;
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	typedef struct tagHitStruct
	{
		float	Value;
		char	Flag;
	}HITSTRUCT;

#define SWAP(X, Y) { t = List[X]; List[X] = List[Y]; List[Y] = t; }
	void Sort4(HITSTRUCT *List)
	{
		HITSTRUCT t;
		if(List[0].Value > List[2].Value) SWAP(0, 2);
		if(List[1].Value > List[3].Value) SWAP(1, 3);
		if(List[0].Value > List[1].Value) SWAP(0, 1);
		if(List[2].Value > List[3].Value) SWAP(2, 3);
		if(List[1].Value > List[2].Value) SWAP(1, 2);         
	} 
#undef SWAP

	bool Rect::GetRECT(LPRECT rc) const
	{
		if(rc)
		{
			rc->left = m_Left;
			rc->right = m_Right;
			rc->top = m_Top;
			rc->bottom = m_Bottom;
			return true;
		}
		return false;
	}

	RECT Rect::GetRECT() const
	{
		RECT rc = { m_Left, m_Top, m_Right, m_Bottom };
		return rc;
	}

	Quad Rect::GetQuad() const
	{
		Quad q;
		q.m_lt = Vector4(m_Left, m_Top);
		q.m_rt = Vector4(m_Right, m_Top);
		q.m_rb = Vector4(m_Right, m_Bottom);
		q.m_lb = Vector4(m_Left, m_Bottom);
		return q;
	}

	void Rect::OffsetRect(const Point& off)
	{
		m_Left += off.x;
		m_Right += off.x;
		m_Top += off.y;
		m_Bottom += off.y;
	}

	void Rect::NormalRect()
	{
		float tmp;
		if(m_Left > m_Right)
		{
			tmp = m_Left;
			m_Left = m_Right;
			m_Right = tmp;
		}
		if(m_Top > m_Bottom)
		{
			tmp = m_Top;
			m_Top = m_Bottom;
			m_Bottom = tmp;
		}
	}

	bool Rect::Hit(const Point& p)
	{
		if(p.x >= m_Left && p.x < m_Right && p.y >= m_Top && p.y < m_Bottom)
			return true;
		return false;
	}

	bool Rect::Hit(const Rect& rc, Rect* pOutRC, float* pArea)
	{
		Rect tmpRc;

		HITSTRUCT hsH[4] = {m_Left, 1, m_Right, 1, rc.m_Left, 2, rc.m_Right, 2};
		Sort4(hsH);
		if(hsH[0].Flag == hsH[1].Flag) return false;

		HITSTRUCT hsV[4] = {m_Top, 1, m_Bottom, 1, rc.m_Top, 2, rc.m_Bottom, 2};
		Sort4(hsV);
		if(hsV[0].Flag == hsV[1].Flag) return false;

		tmpRc.m_Left = hsH[1].Value;
		tmpRc.m_Right = hsH[2].Value;
		tmpRc.m_Top = hsV[1].Value;
		tmpRc.m_Bottom = hsV[2].Value;

		float area = tmpRc.GetArea();
		if(area == 0.0f) return false;

		if(pOutRC) *pOutRC = tmpRc;
		if(pArea) *pArea = area;
		return true;
	}

	Rect& Rect::operator = (const Rect& rc)
	{
		if(&rc == this) return *this;
		m_Left = rc.m_Left;
		m_Right = rc.m_Right;
		m_Top = rc.m_Top;
		m_Bottom = rc.m_Bottom;
		return *this;
	}

	Rect& Rect::operator = (const RECT& rc)
	{
		m_Left = rc.left;
		m_Right = rc.right;
		m_Top = rc.top;
		m_Bottom = rc.bottom;
		return *this;
	}

	bool Rect::operator == (const Rect& rc) const
	{
		if(&rc == this) return true;
		return (m_Left == rc.m_Left &&
			m_Right == rc.m_Right &&
			m_Top == rc.m_Top &&
			m_Bottom == rc.m_Bottom);
	}

	Rect Area(float x, float y, float w, float h)
	{
		return Rect(x, y, x + w, y + h);
	}
}
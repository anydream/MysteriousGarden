/************************************************************************/
// 创建时间: 081029
// 最后确定: 081126

#ifndef _Flexible_h_
#define _Flexible_h_

#include "../AnyPlay.h"
#include "Vector4.h"

namespace AnyPlay
{
	class FlexiblePoint
	{
	public:
		FlexiblePoint() : m_Speed(0.002f) {}

		void Update(float deltaTime)
		{
			Point offset = (m_TargetPoint - m_CurrentPoint) * m_Speed * deltaTime;
			m_CurrentPoint += offset;
		}

		void SetTargetPoint(const Point& p)
		{
			m_TargetPoint = p;

			if(m_TargetPoint.x < m_Range.m_Left) m_TargetPoint.x = m_Range.m_Left;
			else if(m_TargetPoint.x > m_Range.m_Right) m_TargetPoint.x = m_Range.m_Right;
			if(m_TargetPoint.y < m_Range.m_Top) m_TargetPoint.y = m_Range.m_Top;
			else if(m_TargetPoint.y > m_Range.m_Bottom) m_TargetPoint.y = m_Range.m_Bottom;
		}

		const Point&	GetCurrentPoint() const { return m_CurrentPoint; }
		const Point&	GetTargetPoint() const { return m_TargetPoint; }
		float			GetSpeed() const { return m_Speed; }
		void			SetCurrentPoint(const Point& p) { m_CurrentPoint = p; }
		void			SetSpeed(float s) { m_Speed = s; }

	public:
		Point	m_CurrentPoint;
		Point	m_TargetPoint;
		float	m_Speed;
		Rect	m_Range;	// 格外要小心
	};
}
#endif
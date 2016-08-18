/************************************************************************/
// 创建时间: 080920
// 最后确定: 081029

#ifndef _Timer_h_
#define _Timer_h_

#include "../AnyPlay.h"

//////////////////////////////////////////////////////////////////////////
// AnyPlay名称空间
namespace AnyPlay
{
	//////////////////////////////////////////////////////////////////////////
	// 高精度计时器
	class Timer
	{
	public:
		Timer()
		{
			m_bOK = false;
			memset(&m_ticksPerSecond, 0, sizeof(LARGE_INTEGER));
			memset(&m_startCount, 0, sizeof(LARGE_INTEGER));
			m_oldTime = 0.0f;
		}

		bool InitTimer()
		{
			if(!QueryPerformanceFrequency(&m_ticksPerSecond))	// 系统不支持高精度计时器
				m_bOK = false;
			else
			{
				m_bOK = true;
				StartCount();
			}
			return m_bOK;
		}

		// 开始计时
		void StartCount()
		{
			if(m_bOK)
			{
				QueryPerformanceCounter(&m_startCount);
				m_oldTime = GetCurCount();
			}
		}

		// 获得当前时间
		double GetCurCount()
		{
			if(m_bOK)
			{
				LARGE_INTEGER currentTime;
				QueryPerformanceCounter(&currentTime);
				double cnt =  ((double)currentTime.QuadPart - (double)m_startCount.QuadPart) / (double)m_ticksPerSecond.QuadPart;
				return cnt * 1000.f;	// 以毫秒为单位
			}
			return 0.0f;
		}

		// 获得时间差
		double GetElapsedTime()
		{
			if(m_bOK)
			{
				double retTime, tmpTime;
				retTime = tmpTime = GetCurCount();
				retTime -= m_oldTime;
				m_oldTime = tmpTime;
				return retTime;
			}
			return 0.0f;
		}

	private:
		bool			m_bOK;				// 是否就绪
		LARGE_INTEGER	m_ticksPerSecond;	// 每秒频率
		LARGE_INTEGER	m_startCount;		// 开始时间
		double			m_oldTime;			// 上一次记录的时间
	};
}

#endif
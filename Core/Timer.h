/************************************************************************/
// ����ʱ��: 080920
// ���ȷ��: 081029

#ifndef _Timer_h_
#define _Timer_h_

#include "../AnyPlay.h"

//////////////////////////////////////////////////////////////////////////
// AnyPlay���ƿռ�
namespace AnyPlay
{
	//////////////////////////////////////////////////////////////////////////
	// �߾��ȼ�ʱ��
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
			if(!QueryPerformanceFrequency(&m_ticksPerSecond))	// ϵͳ��֧�ָ߾��ȼ�ʱ��
				m_bOK = false;
			else
			{
				m_bOK = true;
				StartCount();
			}
			return m_bOK;
		}

		// ��ʼ��ʱ
		void StartCount()
		{
			if(m_bOK)
			{
				QueryPerformanceCounter(&m_startCount);
				m_oldTime = GetCurCount();
			}
		}

		// ��õ�ǰʱ��
		double GetCurCount()
		{
			if(m_bOK)
			{
				LARGE_INTEGER currentTime;
				QueryPerformanceCounter(&currentTime);
				double cnt =  ((double)currentTime.QuadPart - (double)m_startCount.QuadPart) / (double)m_ticksPerSecond.QuadPart;
				return cnt * 1000.f;	// �Ժ���Ϊ��λ
			}
			return 0.0f;
		}

		// ���ʱ���
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
		bool			m_bOK;				// �Ƿ����
		LARGE_INTEGER	m_ticksPerSecond;	// ÿ��Ƶ��
		LARGE_INTEGER	m_startCount;		// ��ʼʱ��
		double			m_oldTime;			// ��һ�μ�¼��ʱ��
	};
}

#endif
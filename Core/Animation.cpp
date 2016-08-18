/************************************************************************/
// 创建时间: 080914
// 最后确定: 081125

#include "Animation.h"

namespace AnyPlay
{
	Animation::Animation()
	{
		Clear();
	}

	void Animation::Clear()
	{
		RemoveAll();
		RemoveAllFrames();

		m_CountTime = 0.0f;		// 当前累计时间
		m_Speed = 1.0f;			// 播放速度
		m_Activity = true;		// 可活动性
		m_PlayMode = true;		// 播放顺序
	}

	Animation::KeyFrmIter Animation::GetNext(KeyFrmIter f)
	{
		if(m_PlayMode)
		{
			f++;
			if(f >= m_FrameList.end()) f = m_FrameList.begin();
		}
		else
		{
			if(f == m_FrameList.begin()) f = m_FrameList.end() - 1;
			else f--;
		}
		return f;
	}

	bool Animation::DoFrame()
	{
		if((*m_CurrentFrame)->m_Goto != -1) SetNextFrmPos((*m_CurrentFrame)->m_Goto);
		if((*m_CurrentFrame)->m_bStop) m_Activity = false;
		if(!FireEvent(FrmEvent, FrameEventArgs(this))) return false;
		return true;
	}

	dword Animation::AdvanceFrame(float deltaTime)
	{
		dword count = 0;
		if(m_Activity)
		{
			if(IN_RANGE(m_CurrentFrame, m_FrameList))
			{
				m_CountTime += deltaTime * m_Speed;

				while(m_CountTime > (*m_CurrentFrame)->m_WaitTime)
				{
					if(!m_Activity)//....
					{
						m_CountTime = 0.0f;
						break;
					}
					m_CountTime -= (*m_CurrentFrame)->m_WaitTime;
					m_CurrentFrame = m_NextFrame;
					m_NextFrame = GetNext(m_NextFrame);

					count++;

					if(!DoFrame()) return count;
				}
			}
		}
		return count;
	}

	bool Animation::SetCurrentFrmPos(dword frmID)
	{
		KeyFrmIter i = m_FrameList.begin() + frmID;
		if(IN_RANGE(i, m_FrameList))
		{
			m_CurrentFrame = i;
			DoFrame();

			return true;
		}
		return false;
	}

	bool Animation::SetNextFrmPos(dword frmID)
	{
		KeyFrmIter i = m_FrameList.begin() + frmID;
		if(IN_RANGE(i, m_FrameList))
		{
			m_NextFrame = i;
			return true;
		}
		return false;
	}

	dword Animation::GetCurrentFrmPos() const
	{
		if(IN_RANGE(m_CurrentFrame, m_FrameList)) return (dword)(m_CurrentFrame - m_FrameList.begin());
		return -1;
	}

	dword Animation::GetNextFrmPos() const
	{
		if(IN_RANGE(m_NextFrame, m_FrameList)) return (dword)(m_NextFrame - m_FrameList.begin());
		return -1;
	}
//////////////////////////////////////////////////////////////////////////
	bool Animation::Goto(dword fID)
	{
		if(SetCurrentFrmPos(fID))
		{
			m_NextFrame = GetNext(m_CurrentFrame);
			return true;
		}
		return false;
	}

	bool Animation::Goto(const String& frameflag)
	{
		GotoMap::iterator i = m_GotoMap.find(frameflag);
		if(i != m_GotoMap.end()) return Goto(i->second);
		return false;
	}

	/*void Animation::GotoDirect(dword fID)
	{
		SetNextFrmPos(fID);
		dword curfID = GetCurrentFrmPos();
		if(curfID >= fID) SetPlayMode(false);
		else if(curfID < fID) SetPlayMode(true);
		
	}

	void Animation::GotoDirect(const String& frmFlag)
	{
		GotoMap::iterator i = m_GotoMap.find(frmFlag);
		if(i != m_GotoMap.end()) GotoDirect(i->second);
	}*/

	float Animation::GetWeight() const
	{
		float weight = 0.0f;
		if(IN_RANGE(m_CurrentFrame, m_FrameList))
		{
			weight = 1.0f - m_CountTime / (*m_CurrentFrame)->m_WaitTime;
			if(weight > 1.0f) weight = 1.0f;		// 注意
			else if(weight < 0.0f) weight = 0.0f;
		}
		return weight;
	}

	bool Animation::SetWeight(float weight)
	{
		if(weight > 1.0f) weight = 1.0f;
		else if(weight < 0.0f) weight = 0.0f;
		if(IN_RANGE(m_CurrentFrame, m_FrameList))
		{
			m_CountTime = (1.0f - weight) * (*m_CurrentFrame)->m_WaitTime;
			return true;
		}
		return false;
	}

	bool Animation::AddFrame(KeyFrame* frm, dword pos)
	{
		if(frm)
		{
			if(pos == -1) m_FrameList.push_back(frm);
			else
			{
				if(IN_RANGE(m_FrameList.begin() + pos, m_FrameList))
					m_FrameList.insert(m_FrameList.begin() + pos, frm);
				else return false;
			}
			Goto(0);
			return true;
		}
		return false;
	}

	KeyFrame* Animation::GetFrame(dword fID)
	{
		if(fID == -1) return *(m_FrameList.rbegin());
		KeyFrmIter i = m_FrameList.begin() + fID;
		if(IN_RANGE(i, m_FrameList)) return *i;
		return NULL;
	}

	bool Animation::RemoveFrame(dword fID)
	{
		if(fID == -1)
		{
			if(m_FrameList.size())
			{
				SAFE_DELETE(*m_FrameList.rbegin());
				m_FrameList.pop_back();
				return true;
			}
		}
		KeyFrmIter i = m_FrameList.begin() + fID;
		if(IN_RANGE(i, m_FrameList))
		{
			SAFE_DELETE(*i);
			m_FrameList.erase(i);
			return true;
		}
		return false;
	}

	void Animation::RemoveAllFrames()
	{
		for(KeyFrmIter i = m_FrameList.begin(); i != m_FrameList.end(); i++)
			SAFE_DELETE(*i);
		m_FrameList.clear();
		m_CurrentFrame = m_FrameList.end();
		m_NextFrame = m_FrameList.end();
	}

	void Animation::SetPlayMode(bool bMode)
	{
		if(m_Activity && bMode != m_PlayMode)
		{
			float orgWeight = GetWeight();
			KeyFrmIter tmp = m_CurrentFrame;
			m_CurrentFrame = m_NextFrame;
			m_NextFrame = tmp;
			SetWeight(1.0f - orgWeight);
		}
		m_PlayMode = bMode;
	}
}
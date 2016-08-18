/************************************************************************/
// 创建时间: 080914
// 最后确定: 081125

#ifndef _Animation_h_
#define _Animation_h_

#include "../AnyPlay.h"
#include "../FrameWork/Event.h"
#include "Node.h"

//////////////////////////////////////////////////////////////////////////
// AnyPlay名称空间
namespace AnyPlay
{
	//////////////////////////////////////////////////////////////////////////
	// 关键帧
	class KeyFrame
	{
	public:
		float	m_WaitTime;		// 等待的时间
		bool	m_bStop;		// 停止
		dword	m_Goto;			// 跳转

	public:
		KeyFrame() : m_WaitTime(0.0f), m_bStop(false), m_Goto(-1) {}
		KeyFrame(float t) : m_WaitTime(t), m_bStop(false), m_Goto(-1) {}

		virtual ~KeyFrame() {}
	};

	//////////////////////////////////////////////////////////////////////////
	// 动画类
	class Animation : public EventSet, public Node
	{
	public:
		Animation();
		virtual ~Animation() { Clear(); }

		// 清空所有数据
		void		Clear();

		// 向前推进帧
		dword		AdvanceFrame(float deltaTime);

		// 设置帧指针
		bool		SetCurrentFrmPos(dword frmID);
		// 获得帧指针
		dword		GetCurrentFrmPos() const;
		// 设置下一帧
		bool		SetNextFrmPos(dword frmID);
		// 获得下一帧
		dword		GetNextFrmPos() const;
		// 获得当前帧指针
		KeyFrame*	GetCurrentFrame() const { return *m_CurrentFrame; }

		// 跳转
		bool		Goto(dword fID);
		bool		Goto(const String& frameflag);
		// 开始播放
		void		Play() { SetActivity(true); }
		// 停止播放
		void		Stop() { SetActivity(false); }

		//void		GotoDirect(dword fID);
		//void		GotoDirect(const String& frmFlag);
		// 获得当前帧比重
		float		GetWeight() const;
		// 设置当前帧比重
		bool		SetWeight(float weight);

		// 帧列表操作
		bool		AddFrame(KeyFrame* frm, dword pos = -1);
		KeyFrame*	GetFrame(dword fID);
		bool		RemoveFrame(dword fID);
		virtual		void RemoveAllFrames();

		// 获得帧数
		size_t		GetNumFrames() const { return m_FrameList.size(); }

		// 动画可动性
		bool		GetActivity() const { return m_Activity; }
		void		SetActivity(bool bActivity) { m_Activity = bActivity; }

		// 设置速度
		float		GetSpeed() const { return m_Speed; }
		void		SetSpeed(float speed) { if(m_Speed != speed) { m_CountTime = 0.0f; m_Speed = speed; } }

		// 播放顺序操作
		bool		GetPlayMode() const { return m_PlayMode; }
		void		SetPlayMode(bool bMode);

	public:
		typedef std::vector<KeyFrame*> FrmList;
		typedef FrmList::iterator KeyFrmIter;

		typedef std::map<String, dword, String::FastLessCompare> GotoMap;
		GotoMap		m_GotoMap;		// 跳转帧映射

		GotoMap&	GetGotoMap() { return m_GotoMap; }
		FrmList&	GetFrameList() { return m_FrameList; }

	protected:
		FrmList		m_FrameList;	// 帧列表

		float		m_CountTime;	// 当前累计时间
		float		m_Speed;		// 播放速度
		KeyFrmIter	m_CurrentFrame;	// 当前帧指针
		KeyFrmIter	m_NextFrame;	// 下一帧指针

		bool		m_Activity;		// 可活动性
		bool		m_PlayMode;		// 播放顺序

	protected:
		KeyFrmIter	GetNext(KeyFrmIter f);		// 获得下一帧的指针
		bool		DoFrame();					// 响应帧消息
	};

	//////////////////////////////////////////////////////////////////////////
	// 响应动画
	static const String FrmEvent = _T("帧");

	class FrameEventArgs : public EventArgs
	{
	public:
		FrameEventArgs(Animation* pAnm) : m_pAnimation(pAnm) {}

	public:
		Animation*	m_pAnimation;
	};
}

#endif
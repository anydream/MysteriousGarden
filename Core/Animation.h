/************************************************************************/
// ����ʱ��: 080914
// ���ȷ��: 081125

#ifndef _Animation_h_
#define _Animation_h_

#include "../AnyPlay.h"
#include "../FrameWork/Event.h"
#include "Node.h"

//////////////////////////////////////////////////////////////////////////
// AnyPlay���ƿռ�
namespace AnyPlay
{
	//////////////////////////////////////////////////////////////////////////
	// �ؼ�֡
	class KeyFrame
	{
	public:
		float	m_WaitTime;		// �ȴ���ʱ��
		bool	m_bStop;		// ֹͣ
		dword	m_Goto;			// ��ת

	public:
		KeyFrame() : m_WaitTime(0.0f), m_bStop(false), m_Goto(-1) {}
		KeyFrame(float t) : m_WaitTime(t), m_bStop(false), m_Goto(-1) {}

		virtual ~KeyFrame() {}
	};

	//////////////////////////////////////////////////////////////////////////
	// ������
	class Animation : public EventSet, public Node
	{
	public:
		Animation();
		virtual ~Animation() { Clear(); }

		// �����������
		void		Clear();

		// ��ǰ�ƽ�֡
		dword		AdvanceFrame(float deltaTime);

		// ����ָ֡��
		bool		SetCurrentFrmPos(dword frmID);
		// ���ָ֡��
		dword		GetCurrentFrmPos() const;
		// ������һ֡
		bool		SetNextFrmPos(dword frmID);
		// �����һ֡
		dword		GetNextFrmPos() const;
		// ��õ�ǰָ֡��
		KeyFrame*	GetCurrentFrame() const { return *m_CurrentFrame; }

		// ��ת
		bool		Goto(dword fID);
		bool		Goto(const String& frameflag);
		// ��ʼ����
		void		Play() { SetActivity(true); }
		// ֹͣ����
		void		Stop() { SetActivity(false); }

		//void		GotoDirect(dword fID);
		//void		GotoDirect(const String& frmFlag);
		// ��õ�ǰ֡����
		float		GetWeight() const;
		// ���õ�ǰ֡����
		bool		SetWeight(float weight);

		// ֡�б����
		bool		AddFrame(KeyFrame* frm, dword pos = -1);
		KeyFrame*	GetFrame(dword fID);
		bool		RemoveFrame(dword fID);
		virtual		void RemoveAllFrames();

		// ���֡��
		size_t		GetNumFrames() const { return m_FrameList.size(); }

		// �����ɶ���
		bool		GetActivity() const { return m_Activity; }
		void		SetActivity(bool bActivity) { m_Activity = bActivity; }

		// �����ٶ�
		float		GetSpeed() const { return m_Speed; }
		void		SetSpeed(float speed) { if(m_Speed != speed) { m_CountTime = 0.0f; m_Speed = speed; } }

		// ����˳�����
		bool		GetPlayMode() const { return m_PlayMode; }
		void		SetPlayMode(bool bMode);

	public:
		typedef std::vector<KeyFrame*> FrmList;
		typedef FrmList::iterator KeyFrmIter;

		typedef std::map<String, dword, String::FastLessCompare> GotoMap;
		GotoMap		m_GotoMap;		// ��ת֡ӳ��

		GotoMap&	GetGotoMap() { return m_GotoMap; }
		FrmList&	GetFrameList() { return m_FrameList; }

	protected:
		FrmList		m_FrameList;	// ֡�б�

		float		m_CountTime;	// ��ǰ�ۼ�ʱ��
		float		m_Speed;		// �����ٶ�
		KeyFrmIter	m_CurrentFrame;	// ��ǰָ֡��
		KeyFrmIter	m_NextFrame;	// ��һָ֡��

		bool		m_Activity;		// �ɻ��
		bool		m_PlayMode;		// ����˳��

	protected:
		KeyFrmIter	GetNext(KeyFrmIter f);		// �����һ֡��ָ��
		bool		DoFrame();					// ��Ӧ֡��Ϣ
	};

	//////////////////////////////////////////////////////////////////////////
	// ��Ӧ����
	static const String FrmEvent = _T("֡");

	class FrameEventArgs : public EventArgs
	{
	public:
		FrameEventArgs(Animation* pAnm) : m_pAnimation(pAnm) {}

	public:
		Animation*	m_pAnimation;
	};
}

#endif
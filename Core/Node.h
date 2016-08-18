/************************************************************************/
// ����ʱ��: 080914
// ���ȷ��: 081118

#ifndef _Node_h_
#define _Node_h_

#include "../AnyPlay.h"
#include "../FrameWork/Manager.h"
#include "String.h"

#define APNODE_DETACH	1
#define APNODE_REMOVE	2
//////////////////////////////////////////////////////////////////////////
// AnyPlay���ƿռ�
namespace AnyPlay
{
	//////////////////////////////////////////////////////////////////////////
	// �ڵ����
	// ���ڹ����������ݽṹ
	class Node : public Managed<Node>
	{
	public:
		Node() { m_Parent = NULL; m_DetachFlag = NULL; }
		// ����ʱ��������
		Node(const String& name) { m_Parent = NULL; SetName(name); m_DetachFlag = NULL; }
		virtual ~Node() { DetachSelf(); }

		// ���ӵ����ڵ�
		bool	AttachParent(Node* parent);
		// �Ͽ����ڵ�
		bool	DetachParent();

		// ����һ���ӽڵ�
		Node*	AttachChild(Node* child);
		// �Ͽ�һ���ӽڵ�
		bool	DetachChild(Node* child);
		// ��������, �Ͽ�һ���ӽڵ�
		dword	DetachChild(const String& name);

		// ����һ���ڵ��Ƿ�������ӽڵ��б���
		bool	IsChildDefined(const Node* child);
		// ��������, ���ӽڵ��б��в���һ���ӽڵ�
		Node*	FindChild(const String& name);
		// �ݹ����һ���ڵ��Ƿ�������ڵ�
		bool	IsDescendantDefined(const Node* desc);
		// ��������, �ݹ����һ���ڵ��Ƿ�������ڵ�
		Node*	FindDescendant(const String& name);
		// �ݹ����һ���ڵ��Ƿ���ǰ���ڵ�
		bool	IsElderDefined(const Node* elder);
		// ��������, �ݹ����һ���ڵ��Ƿ���ǰ���ڵ�
		Node*	FindElder(const String& name);

		// ���Ҹ��ڵ�
		Node*	GetRoot();
		// �������ڵ����
		dword	GetNumDescendant() const;
		// ��õ�ǰ�ڵ�㼶
		dword	GetLevel() const;

		// ����������ͽڵ��б��жϿ�
		void	DetachSelf();
		// ���ӽڵ��б������нڵ����ӵ�һ���µĸ��ڵ�
		void	DetachChildren(Node* newParent);

		// ��һ���ӽڵ��ƶ���һ����λ��
		bool	MoveTo(Node* child, dword newPos);
		// ��������, ��һ���ӽڵ��ƶ���һ����λ��
		bool	MoveTo(const String& name, dword newPos);

		void	KillDescendants();
		void	KillTree();

		// ��ø��ڵ�ָ��
		Node*	GetParent() const { return m_Parent; }
		// ����ӽڵ��б��нڵ����
		size_t	GetNumChildren() const { return m_ChildrenList.size(); }

		// ��ӡ���нڵ�
		void	PrintRoot(dword count = 0);

		void	SetDetachFlag(ushort df) { m_DetachFlag = df; }
		ushort	GetDetachFlag() const { return m_DetachFlag; }

	public:
		typedef std::vector<Node*> NodeList;
		Node*				m_Parent;		// ���ڵ�ָ��
		NodeList			m_ChildrenList;	// �ӽڵ��б�
		ushort				m_DetachFlag;	// �������
	};
}

#endif

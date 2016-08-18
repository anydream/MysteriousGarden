/************************************************************************/
// ����ʱ��: 080914
// ���ȷ��: 081117

#include "Node.h"

namespace AnyPlay
{
	bool Node::AttachParent(Node* parent)
	{
		bool res = true;
		// ����µ�ָ����ǵ�ǰ���ڵ��򷵻�
		if(m_Parent == parent) return true;
		// �Ͽ���ǰ���ڵ�
		res = DetachParent();
		// ���¸��ڵ�����б�������Լ�
		if(parent) parent->m_ChildrenList.push_back(this);
		// �����¸��ڵ�
		m_Parent = parent;
		return res;
	}

	bool Node::DetachParent()
	{
		if(m_Parent)
		{
			// �������ڵ�����б�
			for(NodeList::iterator i = m_Parent->m_ChildrenList.begin(); i != m_Parent->m_ChildrenList.end(); i++)
			{
				// �ҵ��˵�ǰ�ڵ��λ��
				if(*i == this)
				{
					m_Parent->m_ChildrenList.erase(i);	// ɾ����
					break;
				}
			}
			m_Parent = NULL;	// ������ڵ�ָ��
			return true;
		}
		return false;
	}

	Node* Node::AttachChild(Node* child)
	{
		if(child)
		{
			child->AttachParent(this);
			return child;
		}
		return NULL;
	}

	bool Node::DetachChild(Node* child)
	{
		if(child)
		{
			// Ŀ���ӽڵ�ĸ��ڵ��ǵ�ǰ�ڵ�ſ��Զ���
			if(child->m_Parent == this) return child->DetachParent();
		}
		return false;
	}

	dword Node::DetachChild(const String& name)
	{
		dword count = 0;
		// �����������б�
		for(NodeList::iterator i = m_ChildrenList.begin(); i != m_ChildrenList.end(); i++)
		{
			// ���������Ƿ��Ӧ, ����ҵ���Ŀ��ڵ�
			if((*i)->GetName() == name)
			{
				(*i)->m_Parent = NULL;	// ������ĸ��ڵ�ָ��
				m_ChildrenList.erase(i);// �����б�ɾ��ɾ��
				count++;				// ����
			}
		}
		return count;
	}

	bool Node::IsChildDefined(const Node* child)
	{
		if(child)
		{
			// ����Ŀ��ڵ��Ƿ���������б���
			for(NodeList::iterator i = m_ChildrenList.begin(); i != m_ChildrenList.end(); i++)
				if(*i == child) return true;
		}
		return false;
	}

	Node* Node::FindChild(const String& name)
	{
		// �����б��ڲ��Ҷ�Ӧ���ֵ��ӽڵ�
		for(NodeList::iterator i = m_ChildrenList.begin(); i != m_ChildrenList.end(); i++)
			if((*i)->GetName() == name) return *i;
		return NULL;
	}

	bool Node::IsDescendantDefined(const Node* desc)
	{
		if(desc)
		{
			// �����ǰ�ڵ���Ŀ��ڵ��򷵻�
			if(this == desc) return true;
			// ��������ݹ����б�
			for(NodeList::iterator i = m_ChildrenList.begin(); i != m_ChildrenList.end(); i++)
				if((*i)->IsDescendantDefined(desc)) return true;
		}
		return false;
	}

	Node* Node::FindDescendant(const String& name)
	{
		// �����ǰ�ڵ���Ŀ��ڵ��򷵻�����ָ��
		if(GetName() == name) return this;
		// ��������ݹ����б�
		for(NodeList::iterator i = m_ChildrenList.begin(); i != m_ChildrenList.end(); i++)
			if(Node* tmp = (*i)->FindDescendant(name)) return tmp;
		return NULL;
	}

	bool Node::IsElderDefined(const Node* elder)
	{
		if(m_Parent)
		{
			// �����ǰ�ڵ�ĸ��ڵ���Ŀ��ڵ��򷵻�
			if(m_Parent == elder) return true;
			// ��������ݹ�
			return m_Parent->IsElderDefined(elder);
		}
		return false;
	}

	Node* Node::FindElder(const String& name)
	{
		if(m_Parent)
		{
			// �������ֵݹ����
			if(m_Parent->GetName() == name) return m_Parent;
			return m_Parent->FindElder(name);
		}
		return NULL;
	}

	Node* Node::GetRoot()
	{
		// �ݹ������ո��ڵ�
		if(m_Parent) return m_Parent->GetRoot();
		return this;
	}

	dword Node::GetNumDescendant() const
	{
		dword num = GetNumChildren();
		// �ݹ���ӵõ����պ������
		for(NodeList::const_iterator i = m_ChildrenList.begin(); i != m_ChildrenList.end(); i++)
			num += (*i)->GetNumDescendant();
		return num;
	}

	dword Node::GetLevel() const
	{
		dword level = 1;
		// �ݹ�õ�����㼶
		if(m_Parent) level += m_Parent->GetLevel();
		return level;
	}

	void Node::DetachSelf()
	{
		// �ȱ���ԭ��ָ��
		Node* orgParent = m_Parent;
		// �Ͽ����ڵ�
		DetachParent();
		// �����б�Ľڵ��������ӵ�ԭ�ڵ���, �������
		for(NodeList::iterator i = m_ChildrenList.begin(); i != m_ChildrenList.end(); i++)
			(*i)->AttachParent(orgParent);
	}

	void Node::DetachChildren(Node* newParent)
	{
		// �����б�Ľڵ����ӵ�Ŀ��ڵ���, ���Զ��Ͽ����Լ�������
		for(NodeList::iterator i = m_ChildrenList.begin(); i != m_ChildrenList.end(); i++)
			(*i)->AttachParent(newParent);
	}

	bool Node::MoveTo(Node* child, dword newPos)
	{
		if(child)
		{
			// �����б��в���
			for(NodeList::iterator i = m_ChildrenList.begin(); i != m_ChildrenList.end(); i++)
			{
				// �ҵ�Ŀ��ڵ��λ�ú�
				if(*i == child)
				{
					m_ChildrenList.erase(i);	// ��ɾ��
					// �ٴ���λ�ô�����
					m_ChildrenList.insert(m_ChildrenList.begin() + newPos, child);
					return true;
				}
			}
		}
		return false;
	}

	bool Node::MoveTo(const String& name, dword newPos)
	{
		for(NodeList::iterator i = m_ChildrenList.begin(); i != m_ChildrenList.end(); i++)
		{
			if((*i)->GetName() == name)
			{
				Node* tmp = *i;			// �ȱ���ԭ�ڵ�
				m_ChildrenList.erase(i);// ɾ��ԭ�ڵ�
				// �����뵽��λ��
				m_ChildrenList.insert(m_ChildrenList.begin() + newPos, tmp);
				return true;
			}
		}
		return false;
	}

	void Node::KillDescendants()
	{
		NodeList tmpList = m_ChildrenList;
		for(NodeList::iterator i = tmpList.begin(); i != tmpList.end(); i++)
			(*i)->KillDescendants();
		delete this;
	}

	void Node::KillTree()
	{
		NodeList tmpList = m_ChildrenList;
		for(NodeList::iterator i = tmpList.begin(); i != tmpList.end(); i++)
			(*i)->KillDescendants();
	}

#define PRT_SPACE for(i = 0; i < count; i++) _tprintf(_T("    "));
	void Node::PrintRoot(dword count)
	{
		dword i = 0;

		if(!count) _tprintf(_T("�ܽڵ����: %d\n"), GetNumDescendant());

		PRT_SPACE;
		_tprintf(_T("%s"), GetName()());

		dword numChildrens = GetNumChildren();
		if(numChildrens) _tprintf(_T("(%d)"), numChildrens);
		_tprintf(_T("\n"));

		if(numChildrens)
		{
			PRT_SPACE;
			_tprintf(_T("{\n"));
		}

		count++;
		for(Node::NodeList::iterator j = m_ChildrenList.begin(); j != m_ChildrenList.end(); j++)
			(*j)->PrintRoot(count);
		count--;

		if(numChildrens)
		{
			PRT_SPACE;
			_tprintf(_T("}\n"));
		}
	}
}
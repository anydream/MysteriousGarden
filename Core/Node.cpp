/************************************************************************/
// 创建时间: 080914
// 最后确定: 081117

#include "Node.h"

namespace AnyPlay
{
	bool Node::AttachParent(Node* parent)
	{
		bool res = true;
		// 如果新的指针就是当前父节点则返回
		if(m_Parent == parent) return true;
		// 断开当前父节点
		res = DetachParent();
		// 向新父节点的子列表中添加自己
		if(parent) parent->m_ChildrenList.push_back(this);
		// 设置新父节点
		m_Parent = parent;
		return res;
	}

	bool Node::DetachParent()
	{
		if(m_Parent)
		{
			// 遍历父节点的子列表
			for(NodeList::iterator i = m_Parent->m_ChildrenList.begin(); i != m_Parent->m_ChildrenList.end(); i++)
			{
				// 找到了当前节点的位置
				if(*i == this)
				{
					m_Parent->m_ChildrenList.erase(i);	// 删除它
					break;
				}
			}
			m_Parent = NULL;	// 清除父节点指针
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
			// 目标子节点的父节点是当前节点才可以断连
			if(child->m_Parent == this) return child->DetachParent();
		}
		return false;
	}

	dword Node::DetachChild(const String& name)
	{
		dword count = 0;
		// 遍历自身子列表
		for(NodeList::iterator i = m_ChildrenList.begin(); i != m_ChildrenList.end(); i++)
		{
			// 查找名字是否对应, 如果找到了目标节点
			if((*i)->GetName() == name)
			{
				(*i)->m_Parent = NULL;	// 清除它的父节点指针
				m_ChildrenList.erase(i);// 从子列表删除删除
				count++;				// 计数
			}
		}
		return count;
	}

	bool Node::IsChildDefined(const Node* child)
	{
		if(child)
		{
			// 查找目标节点是否存在于子列表内
			for(NodeList::iterator i = m_ChildrenList.begin(); i != m_ChildrenList.end(); i++)
				if(*i == child) return true;
		}
		return false;
	}

	Node* Node::FindChild(const String& name)
	{
		// 在子列表内查找对应名字的子节点
		for(NodeList::iterator i = m_ChildrenList.begin(); i != m_ChildrenList.end(); i++)
			if((*i)->GetName() == name) return *i;
		return NULL;
	}

	bool Node::IsDescendantDefined(const Node* desc)
	{
		if(desc)
		{
			// 如果当前节点是目标节点则返回
			if(this == desc) return true;
			// 否则继续递归子列表
			for(NodeList::iterator i = m_ChildrenList.begin(); i != m_ChildrenList.end(); i++)
				if((*i)->IsDescendantDefined(desc)) return true;
		}
		return false;
	}

	Node* Node::FindDescendant(const String& name)
	{
		// 如果当前节点是目标节点则返回自身指针
		if(GetName() == name) return this;
		// 否则继续递归子列表
		for(NodeList::iterator i = m_ChildrenList.begin(); i != m_ChildrenList.end(); i++)
			if(Node* tmp = (*i)->FindDescendant(name)) return tmp;
		return NULL;
	}

	bool Node::IsElderDefined(const Node* elder)
	{
		if(m_Parent)
		{
			// 如果当前节点的父节点是目标节点则返回
			if(m_Parent == elder) return true;
			// 否则继续递归
			return m_Parent->IsElderDefined(elder);
		}
		return false;
	}

	Node* Node::FindElder(const String& name)
	{
		if(m_Parent)
		{
			// 根据名字递归查找
			if(m_Parent->GetName() == name) return m_Parent;
			return m_Parent->FindElder(name);
		}
		return NULL;
	}

	Node* Node::GetRoot()
	{
		// 递归获得最终根节点
		if(m_Parent) return m_Parent->GetRoot();
		return this;
	}

	dword Node::GetNumDescendant() const
	{
		dword num = GetNumChildren();
		// 递归相加得到最终后代个数
		for(NodeList::const_iterator i = m_ChildrenList.begin(); i != m_ChildrenList.end(); i++)
			num += (*i)->GetNumDescendant();
		return num;
	}

	dword Node::GetLevel() const
	{
		dword level = 1;
		// 递归得到自身层级
		if(m_Parent) level += m_Parent->GetLevel();
		return level;
	}

	void Node::DetachSelf()
	{
		// 先保存原父指针
		Node* orgParent = m_Parent;
		// 断开父节点
		DetachParent();
		// 把子列表的节点重新连接到原节点上, 隐身而退
		for(NodeList::iterator i = m_ChildrenList.begin(); i != m_ChildrenList.end(); i++)
			(*i)->AttachParent(orgParent);
	}

	void Node::DetachChildren(Node* newParent)
	{
		// 把子列表的节点连接到目标节点上, 并自动断开与自己的连接
		for(NodeList::iterator i = m_ChildrenList.begin(); i != m_ChildrenList.end(); i++)
			(*i)->AttachParent(newParent);
	}

	bool Node::MoveTo(Node* child, dword newPos)
	{
		if(child)
		{
			// 在子列表中查找
			for(NodeList::iterator i = m_ChildrenList.begin(); i != m_ChildrenList.end(); i++)
			{
				// 找到目标节点的位置后
				if(*i == child)
				{
					m_ChildrenList.erase(i);	// 先删除
					// 再从新位置处插入
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
				Node* tmp = *i;			// 先保存原节点
				m_ChildrenList.erase(i);// 删除原节点
				// 最后插入到新位置
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

		if(!count) _tprintf(_T("总节点个数: %d\n"), GetNumDescendant());

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
/************************************************************************/
// 创建时间: 080914
// 最后确定: 081118

#ifndef _Node_h_
#define _Node_h_

#include "../AnyPlay.h"
#include "../FrameWork/Manager.h"
#include "String.h"

#define APNODE_DETACH	1
#define APNODE_REMOVE	2
//////////////////////////////////////////////////////////////////////////
// AnyPlay名称空间
namespace AnyPlay
{
	//////////////////////////////////////////////////////////////////////////
	// 节点基类
	// 用于构建树型数据结构
	class Node : public Managed<Node>
	{
	public:
		Node() { m_Parent = NULL; m_DetachFlag = NULL; }
		// 构造时设置名字
		Node(const String& name) { m_Parent = NULL; SetName(name); m_DetachFlag = NULL; }
		virtual ~Node() { DetachSelf(); }

		// 连接到父节点
		bool	AttachParent(Node* parent);
		// 断开父节点
		bool	DetachParent();

		// 连接一个子节点
		Node*	AttachChild(Node* child);
		// 断开一个子节点
		bool	DetachChild(Node* child);
		// 根据名字, 断开一个子节点
		dword	DetachChild(const String& name);

		// 查找一个节点是否存在于子节点列表中
		bool	IsChildDefined(const Node* child);
		// 根据名字, 在子节点列表中查找一个子节点
		Node*	FindChild(const String& name);
		// 递归查找一个节点是否是子孙节点
		bool	IsDescendantDefined(const Node* desc);
		// 根据名字, 递归查找一个节点是否是子孙节点
		Node*	FindDescendant(const String& name);
		// 递归查找一个节点是否是前辈节点
		bool	IsElderDefined(const Node* elder);
		// 根据名字, 递归查找一个节点是否是前辈节点
		Node*	FindElder(const String& name);

		// 查找根节点
		Node*	GetRoot();
		// 获得子孙节点个数
		dword	GetNumDescendant() const;
		// 获得当前节点层级
		dword	GetLevel() const;

		// 把自身从树型节点列表中断开
		void	DetachSelf();
		// 把子节点列表中所有节点连接到一个新的父节点
		void	DetachChildren(Node* newParent);

		// 把一个子节点移动到一个新位置
		bool	MoveTo(Node* child, dword newPos);
		// 根据名字, 把一个子节点移动到一个新位置
		bool	MoveTo(const String& name, dword newPos);

		void	KillDescendants();
		void	KillTree();

		// 获得父节点指针
		Node*	GetParent() const { return m_Parent; }
		// 获得子节点列表中节点个数
		size_t	GetNumChildren() const { return m_ChildrenList.size(); }

		// 打印所有节点
		void	PrintRoot(dword count = 0);

		void	SetDetachFlag(ushort df) { m_DetachFlag = df; }
		ushort	GetDetachFlag() const { return m_DetachFlag; }

	public:
		typedef std::vector<Node*> NodeList;
		Node*				m_Parent;		// 父节点指针
		NodeList			m_ChildrenList;	// 子节点列表
		ushort				m_DetachFlag;	// 断连标记
	};
}

#endif

/************************************************************************/
// 创建时间: 081116
// 最后确定: 081117

#ifndef _Event_h_
#define _Event_h_

#include "../AnyPlay.h"
#include "../Core/String.h"
#include "Manager.h"

namespace AnyPlay
{
	//////////////////////////////////////////////////////////////////////////
	// 事件参数基类
	class EventArgs
	{
	public:
		EventArgs() {}
		virtual ~EventArgs() {}
	};

	//////////////////////////////////////////////////////////////////////////
	// 回调函子基类
	class SlotFunctorBase
	{
	public:
		virtual ~SlotFunctorBase() {};
		// 调用
		virtual bool Fire(const EventArgs& args) = 0;
		// 判断相等
		virtual bool operator == (const SlotFunctorBase& s) const = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	// 成员回调函子
	template<typename T> class MemberFunctionSlot : public SlotFunctorBase
	{
	public:
		// 回调函子模版
		typedef bool (T::*MemberFunctionType)(const EventArgs&);

		// 设置成员回调函子
		MemberFunctionSlot(MemberFunctionType func, T* obj) : m_Function(func), m_pObj(obj) {}

		// 调用函子
		bool Fire(const EventArgs& args)
		{
			return (m_pObj->*m_Function)(args);
		}

		// 判断是否相等
		bool operator == (const SlotFunctorBase& s) const
		{
			return (m_Function == ((MemberFunctionSlot&)s).m_Function) &&
				(m_pObj == ((MemberFunctionSlot&)s).m_pObj);
		}

	private:
		MemberFunctionType	m_Function;	// 回调函子指针
		T*					m_pObj;		// 回调类指针
	};

	//////////////////////////////////////////////////////////////////////////
	// 标准函数回调函子
	class FreeFunctionSlot : public SlotFunctorBase
	{
	public:
		// 回调函子格式定义
		typedef bool (SlotFunction)(const EventArgs&);

		FreeFunctionSlot(SlotFunction* func) : m_Function(func) {}

		// 调用函子
		bool Fire(const EventArgs& args)
		{
			return m_Function(args);
		}

		// 判断是否相等
		bool operator == (const SlotFunctorBase& s) const
		{
			return (m_Function == ((FreeFunctionSlot&)s).m_Function);
		}

	private:
		SlotFunction*		m_Function;	// 回调函子指针
	};

	//////////////////////////////////////////////////////////////////////////
	// 回调函子序列
	class SubscriberSlot
	{
	public:
		SubscriberSlot() : m_FunctorImpl(0), m_bCanDelete(true) {}
		SubscriberSlot(const SubscriberSlot& subscriber) : m_FunctorImpl(0) { *this = subscriber; }

		// 构造为成员回调函子
		template<typename T>
		SubscriberSlot(bool (T::*func)(const EventArgs&), T* obj) :
		m_FunctorImpl(new MemberFunctionSlot<T>(func, obj)), m_bCanDelete(true) {}

		// 构造为标准函数回调函子
		SubscriberSlot(FreeFunctionSlot::SlotFunction* func) :
		m_FunctorImpl(new FreeFunctionSlot(func)), m_bCanDelete(true) {}

		virtual ~SubscriberSlot() { CleanUp(); }

		// 释放
		void CleanUp()
		{
			if(m_bCanDelete)
				SAFE_DELETE(m_FunctorImpl);
		}
		// 调用回调函子
		bool Fire(const EventArgs& args) const
		{
			return m_FunctorImpl->Fire(args);
		}
		// 判断相等
		bool operator == (const SubscriberSlot& s) const
		{
			return *m_FunctorImpl == *s.m_FunctorImpl;
		}
		bool operator != (const SubscriberSlot& s) const { return !(*this == s); }

	private:
		// 复制
		SubscriberSlot& operator = (const SubscriberSlot& subscriber)
		{
			CleanUp();
			m_FunctorImpl = subscriber.m_FunctorImpl;
			m_bCanDelete = subscriber.m_bCanDelete;
			const_cast<SubscriberSlot&>(subscriber).m_bCanDelete = false;
			return *this;
		}

	private:
		SlotFunctorBase*	m_FunctorImpl;	// 函子
		bool				m_bCanDelete;	// 释放权
	};

	//////////////////////////////////////////////////////////////////////////
	// 事件
	// 一个事件包含多个回调函子序列
	class Event : public Managed<Event>
	{
	public:
		Event() {}
		// 在初始化时添加一个序列
		Event(const String& name, SubscriberSlot* subscriber)
		{
			SetName(name);
			if(subscriber) m_SlotPtrArray.push_back(subscriber);
		}
		// 析构
		virtual ~Event() { RemoveAllSlot(); }

		// 是否存在于列表中
		bool IsDefined(SubscriberSlot* subscriber)
		{
			for(SlotPtrArray::iterator i = m_SlotPtrArray.begin(); i != m_SlotPtrArray.end(); i++)
				if(*i == subscriber) return true;
			return false;
		}

		// 获得一个序列
		SubscriberSlot* GetSlot(int pos = -1)
		{
			if(pos == -1)
			{
				if(m_SlotPtrArray.rbegin() != m_SlotPtrArray.rend())
					return *(m_SlotPtrArray.rbegin());
			}
			else
			{
				if(IN_RANGE(m_SlotPtrArray.begin() + pos, m_SlotPtrArray)) return m_SlotPtrArray[pos];
			}
			return NULL;
		}

		// 添加序列
		bool AddSlot(SubscriberSlot* subscriber, int pos = -1)
		{
			if(subscriber)
			{
				if(!IsDefined(subscriber))	// 该序列不存在
				{
					if(pos == -1)
					{
						m_SlotPtrArray.push_back(subscriber);
						return true;
					}
					else
					{
						if(IN_RANGE(m_SlotPtrArray.begin() + pos, m_SlotPtrArray))
						{
							m_SlotPtrArray.insert(m_SlotPtrArray.begin() + pos, subscriber);
							return true;
						}
					}
				}
			}
			return false;
		}

		// 删除一个序列
		bool RemoveSlot(int pos = -1)
		{
			if(pos == -1)
			{
				if(m_SlotPtrArray.rbegin() != m_SlotPtrArray.rend())
				{
					SAFE_DELETE(*(m_SlotPtrArray.rbegin()));
					m_SlotPtrArray.pop_back();
					return true;
				}
			}
			else
			{
				if(IN_RANGE(m_SlotPtrArray.begin() + pos, m_SlotPtrArray))
				{
					SAFE_DELETE(*(m_SlotPtrArray.begin() + pos));
					m_SlotPtrArray.erase(m_SlotPtrArray.begin() + pos);
					return true;
				}
			}
			return false;
		}

		// 根据指针删除序列
		bool RemoveSlot(SubscriberSlot* subscriber)
		{
			if(subscriber)
			{
				for(SlotPtrArray::iterator i = m_SlotPtrArray.begin(); i != m_SlotPtrArray.end(); i++)
				{
					if(**i == *subscriber)
					{
						SAFE_DELETE(*i);
						m_SlotPtrArray.erase(i);
						return true;
					}
				}
			}
			return false;
		}

		// 删除所有序列
		void RemoveAllSlot()
		{
			for(SlotPtrArray::iterator i = m_SlotPtrArray.begin(); i != m_SlotPtrArray.end(); i++)
				SAFE_DELETE(*i);
			m_SlotPtrArray.clear();
		}

		// 调用所有序列
		bool Fire(const EventArgs& args)
		{
			for(SlotPtrArray::iterator i = m_SlotPtrArray.begin(); i != m_SlotPtrArray.end(); i++)
			{
				if(!(*i)->Fire(args)) return false;
			}
			return true;
		}

		bool operator () (const EventArgs& args)
		{
			return Fire(args);
		}

	private:
		typedef std::vector<SubscriberSlot*> SlotPtrArray;
		SlotPtrArray	m_SlotPtrArray;		// 回调序列
	};

	//////////////////////////////////////////////////////////////////////////
	// 事件集
	class EventSet : public Manager<Event>
	{
	public:
		EventSet() {}
		virtual ~EventSet() {}

		// 记录一个事件
		void SubscribeEvent(const String& name, const SubscriberSlot& subscriber)
		{
			Event* eve = Get(name);
			if(eve) eve->AddSlot(new SubscriberSlot(subscriber));
			else Add(new Event(name, new SubscriberSlot(subscriber)));
		}
		// 删除一个事件
		bool RemoveEvent(const String& name, const SubscriberSlot& subscriber)
		{
			if(Event* eve = Get(name))
			{
				if(!eve->RemoveSlot((SubscriberSlot*)&subscriber))
					Remove(eve);
				return true;
			}
			return false;
		}

		// 调用一个事件
		bool FireEvent(const String& name, const EventArgs& args)
		{
			if(Event* eve = Get(name)) return eve->Fire(args);
			return false;
		}

	};
}

#endif
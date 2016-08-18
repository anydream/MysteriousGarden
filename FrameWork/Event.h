/************************************************************************/
// ����ʱ��: 081116
// ���ȷ��: 081117

#ifndef _Event_h_
#define _Event_h_

#include "../AnyPlay.h"
#include "../Core/String.h"
#include "Manager.h"

namespace AnyPlay
{
	//////////////////////////////////////////////////////////////////////////
	// �¼���������
	class EventArgs
	{
	public:
		EventArgs() {}
		virtual ~EventArgs() {}
	};

	//////////////////////////////////////////////////////////////////////////
	// �ص����ӻ���
	class SlotFunctorBase
	{
	public:
		virtual ~SlotFunctorBase() {};
		// ����
		virtual bool Fire(const EventArgs& args) = 0;
		// �ж����
		virtual bool operator == (const SlotFunctorBase& s) const = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	// ��Ա�ص�����
	template<typename T> class MemberFunctionSlot : public SlotFunctorBase
	{
	public:
		// �ص�����ģ��
		typedef bool (T::*MemberFunctionType)(const EventArgs&);

		// ���ó�Ա�ص�����
		MemberFunctionSlot(MemberFunctionType func, T* obj) : m_Function(func), m_pObj(obj) {}

		// ���ú���
		bool Fire(const EventArgs& args)
		{
			return (m_pObj->*m_Function)(args);
		}

		// �ж��Ƿ����
		bool operator == (const SlotFunctorBase& s) const
		{
			return (m_Function == ((MemberFunctionSlot&)s).m_Function) &&
				(m_pObj == ((MemberFunctionSlot&)s).m_pObj);
		}

	private:
		MemberFunctionType	m_Function;	// �ص�����ָ��
		T*					m_pObj;		// �ص���ָ��
	};

	//////////////////////////////////////////////////////////////////////////
	// ��׼�����ص�����
	class FreeFunctionSlot : public SlotFunctorBase
	{
	public:
		// �ص����Ӹ�ʽ����
		typedef bool (SlotFunction)(const EventArgs&);

		FreeFunctionSlot(SlotFunction* func) : m_Function(func) {}

		// ���ú���
		bool Fire(const EventArgs& args)
		{
			return m_Function(args);
		}

		// �ж��Ƿ����
		bool operator == (const SlotFunctorBase& s) const
		{
			return (m_Function == ((FreeFunctionSlot&)s).m_Function);
		}

	private:
		SlotFunction*		m_Function;	// �ص�����ָ��
	};

	//////////////////////////////////////////////////////////////////////////
	// �ص���������
	class SubscriberSlot
	{
	public:
		SubscriberSlot() : m_FunctorImpl(0), m_bCanDelete(true) {}
		SubscriberSlot(const SubscriberSlot& subscriber) : m_FunctorImpl(0) { *this = subscriber; }

		// ����Ϊ��Ա�ص�����
		template<typename T>
		SubscriberSlot(bool (T::*func)(const EventArgs&), T* obj) :
		m_FunctorImpl(new MemberFunctionSlot<T>(func, obj)), m_bCanDelete(true) {}

		// ����Ϊ��׼�����ص�����
		SubscriberSlot(FreeFunctionSlot::SlotFunction* func) :
		m_FunctorImpl(new FreeFunctionSlot(func)), m_bCanDelete(true) {}

		virtual ~SubscriberSlot() { CleanUp(); }

		// �ͷ�
		void CleanUp()
		{
			if(m_bCanDelete)
				SAFE_DELETE(m_FunctorImpl);
		}
		// ���ûص�����
		bool Fire(const EventArgs& args) const
		{
			return m_FunctorImpl->Fire(args);
		}
		// �ж����
		bool operator == (const SubscriberSlot& s) const
		{
			return *m_FunctorImpl == *s.m_FunctorImpl;
		}
		bool operator != (const SubscriberSlot& s) const { return !(*this == s); }

	private:
		// ����
		SubscriberSlot& operator = (const SubscriberSlot& subscriber)
		{
			CleanUp();
			m_FunctorImpl = subscriber.m_FunctorImpl;
			m_bCanDelete = subscriber.m_bCanDelete;
			const_cast<SubscriberSlot&>(subscriber).m_bCanDelete = false;
			return *this;
		}

	private:
		SlotFunctorBase*	m_FunctorImpl;	// ����
		bool				m_bCanDelete;	// �ͷ�Ȩ
	};

	//////////////////////////////////////////////////////////////////////////
	// �¼�
	// һ���¼���������ص���������
	class Event : public Managed<Event>
	{
	public:
		Event() {}
		// �ڳ�ʼ��ʱ���һ������
		Event(const String& name, SubscriberSlot* subscriber)
		{
			SetName(name);
			if(subscriber) m_SlotPtrArray.push_back(subscriber);
		}
		// ����
		virtual ~Event() { RemoveAllSlot(); }

		// �Ƿ�������б���
		bool IsDefined(SubscriberSlot* subscriber)
		{
			for(SlotPtrArray::iterator i = m_SlotPtrArray.begin(); i != m_SlotPtrArray.end(); i++)
				if(*i == subscriber) return true;
			return false;
		}

		// ���һ������
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

		// �������
		bool AddSlot(SubscriberSlot* subscriber, int pos = -1)
		{
			if(subscriber)
			{
				if(!IsDefined(subscriber))	// �����в�����
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

		// ɾ��һ������
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

		// ����ָ��ɾ������
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

		// ɾ����������
		void RemoveAllSlot()
		{
			for(SlotPtrArray::iterator i = m_SlotPtrArray.begin(); i != m_SlotPtrArray.end(); i++)
				SAFE_DELETE(*i);
			m_SlotPtrArray.clear();
		}

		// ������������
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
		SlotPtrArray	m_SlotPtrArray;		// �ص�����
	};

	//////////////////////////////////////////////////////////////////////////
	// �¼���
	class EventSet : public Manager<Event>
	{
	public:
		EventSet() {}
		virtual ~EventSet() {}

		// ��¼һ���¼�
		void SubscribeEvent(const String& name, const SubscriberSlot& subscriber)
		{
			Event* eve = Get(name);
			if(eve) eve->AddSlot(new SubscriberSlot(subscriber));
			else Add(new Event(name, new SubscriberSlot(subscriber)));
		}
		// ɾ��һ���¼�
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

		// ����һ���¼�
		bool FireEvent(const String& name, const EventArgs& args)
		{
			if(Event* eve = Get(name)) return eve->Fire(args);
			return false;
		}

	};
}

#endif
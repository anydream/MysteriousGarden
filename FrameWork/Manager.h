/************************************************************************/
// ����ʱ��: 081112
// ���ȷ��: 081119

#ifndef _Manager_h_
#define _Manager_h_

#include "../AnyPlay.h"
#include "Singleton.h"
#include "../Core/String.h"

namespace AnyPlay
{
	template<typename T> class Managed;

	//////////////////////////////////////////////////////////////////////////
	// ������ T = �����������
	template<typename T> class Manager
	{
	public:
		Manager() : m_bFullRight(true) {}
		virtual ~Manager() { RemoveAll(); }

		//////////////////////////////////////////////////////////////////////////
		// ���
		bool Add(T* mgd)
		{
			if(!mgd) return false;
			if(!dynamic_cast<Managed<T>*>(mgd)) return false;

			mgd->Detach();
			mgd->m_pManager = this;

			ManagedMap::iterator i = m_ManagedMap.find(mgd->GetName());
			if(i != m_ManagedMap.end())
			{
				if(i->second == mgd) return false;	// �Ѿ����ڵ�ָ��
				mgd->m_strName = GenName(mgd->GetName());
			}

			m_ManagedMap[mgd->GetName()] = mgd;

			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		// ��������
		String GenName(const String& name)
		{
			dword count = 2;
			String tmpName = name;
			while(m_ManagedMap.find(tmpName) != m_ManagedMap.end())
			{
				tmpName.Format(_T("%s#%X"), name(), count);
				count++;
			}
			return tmpName;
		}

		//////////////////////////////////////////////////////////////////////////
		// ���������Ƴ�
		bool Remove(const String& name)
		{
			ManagedMap::iterator i = m_ManagedMap.find(name);
			if(i == m_ManagedMap.end()) return false;

			if(m_bFullRight) { SAFE_DELETE(i->second); }
			else i->second->m_pManager = NULL;
			m_ManagedMap.erase(i);

			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		// �Ƴ�
		bool Remove(T* mgd)
		{
			if(mgd)
			{
				for(ManagedMap::iterator i = m_ManagedMap.begin(); i != m_ManagedMap.end(); i++)
				{
					if(i->second == mgd)
					{
						if(m_bFullRight) { SAFE_DELETE(i->second); }
						else i->second->m_pManager = NULL;
						m_ManagedMap.erase(i);
						return true;
					}
				}
			}
			return false;
		}

		//////////////////////////////////////////////////////////////////////////
		// ������
		bool Rename(const String& name, const String& newname)
		{
			ManagedMap::iterator i = m_ManagedMap.find(name);
			if(i != m_ManagedMap.end())
			{
				T* mgd = i->second;
				m_ManagedMap.erase(i);
				mgd->m_strName = newname;
				return Add(mgd);
			}
			return false;
		}

		//////////////////////////////////////////////////////////////////////////
		// ���
		T* Get(const String& name) const
		{
			ManagedMap::const_iterator i = m_ManagedMap.find(name);
			if(i != m_ManagedMap.end()) return i->second;
			return NULL;
		}

		//////////////////////////////////////////////////////////////////////////
		// �Ƴ�ȫ��
		void RemoveAll()
		{
			for(ManagedMap::iterator i = m_ManagedMap.begin(); i != m_ManagedMap.end(); i++)
			{
				if(m_bFullRight) { SAFE_DELETE(i->second); }
				else i->second->m_pManager = NULL; 
			}
			m_ManagedMap.clear();
		}

		//////////////////////////////////////////////////////////////////////////
		// ����ָ���ж��Ƿ���
		bool IsDefined(T* mgd) const
		{
			if(mgd)
			{
				for(ManagedMap::const_iterator i = m_ManagedMap.begin(); i != m_ManagedMap.end(); i++)
					if(i->second == mgd) return true;
			}
			return false;
		}

		// �Ƿ���
		bool				IsDefined(const String& name) const { return m_ManagedMap.find(name) != m_ManagedMap.end(); }
		// ��ñ�����������
		size_t				GetNumManaged() const { return m_ManagedMap.size(); }
		// ȫȨ���
		void				SetFullRight(bool bFR) { m_bFullRight = bFR; }
		bool				GetFullRight() const { return m_bFullRight; }

	public:
		typedef std::map<String, T*, String::FastLessCompare> ManagedMap;
		ManagedMap	m_ManagedMap;

	private:
		bool		m_bFullRight;	// ȫȨ���
	};

	//////////////////////////////////////////////////////////////////////////
	// ��������
	template<typename T> class Managed
	{
	public:
		friend class Manager<T>;

		Managed() : m_pManager(NULL) {}
		virtual ~Managed() {}

		// ��ù�����ָ��
		Manager<T>*	GetMgrPtr() { return m_pManager; }
		// �������
		const String&	GetName() const
		{
			return m_strName;
		}
		// ��������
		bool SetName(const String& name)
		{
			if(m_pManager)
			{
				if(!m_pManager->Rename(m_strName, name)) return false;
			}
			m_strName = name;
			return true;
		}

		bool Detach()
		{
			if(m_pManager)
			{
				Manager<T>::ManagedMap::iterator i = m_pManager->m_ManagedMap.find(m_strName);
				if(i != m_pManager->m_ManagedMap.end())
				{
					m_pManager->m_ManagedMap.erase(i);
					m_pManager = NULL;
					return true;
				}
			}
			return false;
		}

	private:
		String		m_strName;	// ����
		Manager<T>*	m_pManager;	// ������ָ��
	};


	//////////////////////////////////////////////////////////////////////////
	// ȫ�ֹ�����
	/*class GlobalManager : public Singleton<GlobalManager> {};
	template<> GlobalManager* Singleton<GlobalManager>::m_Singleton = 0;*/
}

#endif
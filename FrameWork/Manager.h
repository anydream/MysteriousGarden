/************************************************************************/
// 创建时间: 081112
// 最后确定: 081119

#ifndef _Manager_h_
#define _Manager_h_

#include "../AnyPlay.h"
#include "Singleton.h"
#include "../Core/String.h"

namespace AnyPlay
{
	template<typename T> class Managed;

	//////////////////////////////////////////////////////////////////////////
	// 管理器 T = 被管理的类型
	template<typename T> class Manager
	{
	public:
		Manager() : m_bFullRight(true) {}
		virtual ~Manager() { RemoveAll(); }

		//////////////////////////////////////////////////////////////////////////
		// 添加
		bool Add(T* mgd)
		{
			if(!mgd) return false;
			if(!dynamic_cast<Managed<T>*>(mgd)) return false;

			mgd->Detach();
			mgd->m_pManager = this;

			ManagedMap::iterator i = m_ManagedMap.find(mgd->GetName());
			if(i != m_ManagedMap.end())
			{
				if(i->second == mgd) return false;	// 已经存在的指针
				mgd->m_strName = GenName(mgd->GetName());
			}

			m_ManagedMap[mgd->GetName()] = mgd;

			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		// 生成名字
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
		// 根据名字移除
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
		// 移除
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
		// 重命名
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
		// 获得
		T* Get(const String& name) const
		{
			ManagedMap::const_iterator i = m_ManagedMap.find(name);
			if(i != m_ManagedMap.end()) return i->second;
			return NULL;
		}

		//////////////////////////////////////////////////////////////////////////
		// 移除全部
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
		// 根据指针判断是否定义
		bool IsDefined(T* mgd) const
		{
			if(mgd)
			{
				for(ManagedMap::const_iterator i = m_ManagedMap.begin(); i != m_ManagedMap.end(); i++)
					if(i->second == mgd) return true;
			}
			return false;
		}

		// 是否定义
		bool				IsDefined(const String& name) const { return m_ManagedMap.find(name) != m_ManagedMap.end(); }
		// 获得被管理对象个数
		size_t				GetNumManaged() const { return m_ManagedMap.size(); }
		// 全权标记
		void				SetFullRight(bool bFR) { m_bFullRight = bFR; }
		bool				GetFullRight() const { return m_bFullRight; }

	public:
		typedef std::map<String, T*, String::FastLessCompare> ManagedMap;
		ManagedMap	m_ManagedMap;

	private:
		bool		m_bFullRight;	// 全权标记
	};

	//////////////////////////////////////////////////////////////////////////
	// 被管理者
	template<typename T> class Managed
	{
	public:
		friend class Manager<T>;

		Managed() : m_pManager(NULL) {}
		virtual ~Managed() {}

		// 获得管理器指针
		Manager<T>*	GetMgrPtr() { return m_pManager; }
		// 获得名字
		const String&	GetName() const
		{
			return m_strName;
		}
		// 设置名字
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
		String		m_strName;	// 名字
		Manager<T>*	m_pManager;	// 管理器指针
	};


	//////////////////////////////////////////////////////////////////////////
	// 全局管理器
	/*class GlobalManager : public Singleton<GlobalManager> {};
	template<> GlobalManager* Singleton<GlobalManager>::m_Singleton = 0;*/
}

#endif
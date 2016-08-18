/************************************************************************/
// 创建时间: 081001
// 最后确定: 081017
// "头脑风暴..."

#ifndef _Singleton_h_
#define _Singleton_h_

#include "../AnyPlay.h"

//////////////////////////////////////////////////////////////////////////
// AnyPlay名称空间
namespace AnyPlay
{
	//////////////////////////////////////////////////////////////////////////
	// 单件模式基类
	// 让派生类可在全局任何地方被访问
	template<typename T> class Singleton
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		// 构造时获得目标单件类指针
		Singleton() { m_Singleton = static_cast<T*>(this); }
		virtual ~Singleton() { m_Singleton = NULL; }

		//////////////////////////////////////////////////////////////////////////
		// 获得单件引用对象
		static T& GetSingleton() { return *m_Singleton; }

		//////////////////////////////////////////////////////////////////////////
		// 获得单件指针
		static T* GetSingletonPtr() { return m_Singleton; }

	protected:
		static T* m_Singleton;	// 目标单件类指针
	};
}

#endif
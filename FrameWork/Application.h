/************************************************************************/
// 创建时间: 081028
// 最后确定: 081119

#ifndef _Application_h_
#define _Application_h_

#include "../AnyPlay.h"
#include "../Core/String.h"
#include "Singleton.h"
#include "Interface.h"
#include "Logger.h"

//////////////////////////////////////////////////////////////////////////
// AnyPlay名称空间
namespace AnyPlay
{
	//////////////////////////////////////////////////////////////////////////
	// 窗口消息回调函数
	LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//////////////////////////////////////////////////////////////////////////
	// 应用程序类
	// 用于组建程序框架, 只能创建一个对象, 可在全局任何地方被访问
	class Application : public Singleton<Application>
	{
	public:
		Application(Interface* firstInf);
		~Application() {}

		// 开始运行
		dword			Run();

		// 设置更新接口标记
		virtual void	SetUpdateFlag(bool flag) { m_bUpdateInterface = flag; }
		// 获得更新接口标记
		virtual bool	GetUpdateFlag() const { return m_bUpdateInterface; }
		// 设置下一个更新接口指针
		virtual void	SetNextInterface(Interface* inf);
		// 销毁当前更新接口
		virtual bool	DestroyCurInterface();
		// 更新当前接口
		virtual void	UpdateInterface();

		//////////////////////////////////////////////////////////////////////////
		// 初始化应用程序
		virtual bool	Initialise() = 0;
		// 释放应用程序
		virtual void	Cleanup() = 0;
		// 主循环
		virtual void	MainLoop();

		// 响应窗口消息
		virtual LRESULT	OnWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		virtual ushort	GetWindowWidth() const { return 0; }
		virtual ushort	GetWindowHeight() const { return 0; }

	protected:
		bool			m_bActivity;		// 程序可活动性

		bool			m_bUpdateInterface;		// 是否要更新接口
		Interface*		m_pCurrentInterface;	// 当前接口
		Interface*		m_pNextInterface;		// 下一接口
	};
}

#endif
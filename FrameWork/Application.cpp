/************************************************************************/
// 创建时间: 080918
// 最后确定: 081118

#include "Application.h"
#pragma warning(disable: 4244)

namespace AnyPlay
{
	template<> Application* Singleton<Application>::m_Singleton = 0;

	Application::Application(Interface* firstInf)
	{
		m_bActivity = false;

		m_pCurrentInterface = NULL;
		m_pNextInterface = firstInf;
		m_bUpdateInterface = firstInf ? true : false;
	}

	LRESULT	Application::OnWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		bool bFlag = true;
		if(m_pCurrentInterface) bFlag = m_pCurrentInterface->OnWindowMessage(hWnd, message, wParam, lParam);
		// 调用缺省消息处理过程
		if(bFlag) return DefWindowProc(hWnd, message, wParam, lParam);
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	// 运行程序, 程序循环
	dword Application::Run()
	{
		if(!Initialise())	// 如果程序初始化失败
		{
			// 释放并返回错误
			Cleanup();
			return -1;
		}
		// 设置程序可活动
		m_bActivity = true;

		MSG msg;
		// 开始进入主消息循环
		while(1)
		{
			if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
			{
				if(!GetMessage(&msg, NULL, 0, 0 )) break;	// 程序正常退出, 跳出消息循环
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else if(m_bActivity) MainLoop();	// 进入主循环
			else WaitMessage();					// 等待消息
		}
		// 释放程序
		Cleanup();
		// 返回退出码
		return msg.wParam;
	}

	void Application::MainLoop()
	{
		if(m_bUpdateInterface)
			UpdateInterface();
	}
	
	//////////////////////////////////////////////////////////////////////////
	// 设置接口
	void Application::SetNextInterface(Interface* inf)
	{
		if(inf) m_pNextInterface = inf;
	}

	//////////////////////////////////////////////////////////////////////////
	// 销毁接口
	bool Application::DestroyCurInterface()
	{
		bool bRet = false;
		if(m_pCurrentInterface)
		{
			bRet = m_pCurrentInterface->Cleanup();
			SAFE_DELETE(m_pCurrentInterface);
		}
		return bRet;
	}

	//////////////////////////////////////////////////////////////////////////
	// 更新接口
	void Application::UpdateInterface()
	{
		m_bUpdateInterface = false;
		DestroyCurInterface();
		if(m_pNextInterface)
		{
			m_pNextInterface->Initialise();
			m_pCurrentInterface = m_pNextInterface;
			m_pNextInterface = NULL;
		}
	}

	LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		return Application::GetSingleton().OnWindowMessage(hWnd, message, wParam, lParam);
	}
}
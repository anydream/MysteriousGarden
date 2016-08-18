

#include "MyApp.h"
#include "../../Utils.h"
#include "../../RenderSystem/RenderSystem.h"
#include "../../InputSystem/InputSystem.h"
#include "../../FileSystem/FileSystem.h"
#include "../../FrameWork/AnmManager.h"

namespace AnyPlay
{
	//////////////////////////////////////////////////////////////////////////
	// 初始化程序
	bool MyApp::Initialise()
	{
		// 初始化
		bool bResult = true;

		m_Timer.InitTimer();
		// 日志记录器
		Logger* logger = new Logger;

		// 窗口
		HICON hIcon = 0;//LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MAINICON));
		m_hWnd = InitWindow(m_strClassName(), m_strWindowName(), m_wndWidth, m_wndHeight, WinProc, hIcon, hIcon, 0);
		if(!m_hWnd) bResult = false;

		// 文件系统
		FileSystem* fs = new FileSystem;

		// 输入系统
		InputSystem* is = new InputSystem;
		is->BeginHideCursor(LoadCursor(NULL, IDC_ARROW));

		// 渲染系统
		RenderSystem* rs = new RenderSystem;
		if(!rs->CreateRenderSystem(m_wndWidth, m_wndHeight, m_hWnd, true, true, false, false)) bResult = false;

		// 动画管理
		AnmManager* am = new AnmManager;

		// 场景管理

		return bResult;
	}

	void MyApp::Cleanup()
	{
		SAFE_DELETE(m_pNextInterface);
		// 释放游戏接口
		DestroyCurInterface();

		// 释放场景管理

		// 释放动画管理
		AnmManager* am = AnmManager::GetSingletonPtr();
		SAFE_DELETE(am);

		// 释放渲染系统
		RenderSystem* rs = RenderSystem::GetSingletonPtr();
		SAFE_DELETE(rs);

		// 释放输入系统
		InputSystem* is = InputSystem::GetSingletonPtr();
		SAFE_DELETE(is);

		// 释放文件系统
		FileSystem* fs = FileSystem::GetSingletonPtr();
		SAFE_DELETE(fs);

		// 释放日志记录器
		Logger* logger = Logger::GetSingletonPtr();
		SAFE_DELETE(logger);
	}

	//////////////////////////////////////////////////////////////////////////
	// 主循环
	void MyApp::MainLoop()
	{
		// 如果需要更新接口
		Application::MainLoop();
		// 扫描输入
		InputSystem* is = InputSystem::GetSingletonPtr();
		is->Scan();

		// 计算延迟
		float deltaTime = m_Timer.GetElapsedTime();

		// 接口循环
		if(m_pCurrentInterface) m_pCurrentInterface->MainLoop(deltaTime);

		// 推进帧
		AnmManager::GetSingleton().Advance(deltaTime);

		// 绘制场景
		RenderSystem* rs = RenderSystem::GetSingletonPtr();
		rs->BeginScene(true, false, 0xFF000000);
		{
			//Texture* tex = (Texture*)rs->Get(_T("未命名.tex"));
			//tex->DrawTexture(0, 0, 0);
		}
		rs->EndScene(NULL);
	}

	LRESULT MyApp::OnWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch(message)
		{
		case WM_SETFOCUS:
			if(m_bActivity) InputSystem::GetSingleton().SetActivity(true);
			break;
		case WM_KILLFOCUS:
			if(m_bActivity) InputSystem::GetSingleton().SetActivity(false);
			break;

		case WM_SYSCOMMAND:		// 当程序最小化时设置为非活动状态
			if(wParam == SC_MINIMIZE) m_bActivity = false;
			else if(wParam == SC_RESTORE)
			{
				m_bActivity = true;
				m_Timer.StartCount();
			}
			break;
		case WM_DESTROY:		// 退出程序消息
			m_bActivity = false;
			PostQuitMessage(0);	// 退出
			break;
		}
		return Application::OnWindowMessage(hWnd, message, wParam, lParam);
	}

	void MyApp::UpdateInterface()
	{
		Application::UpdateInterface();
		// 开始计时
		m_Timer.StartCount();
	}
}
/************************************************************************/
// 创建时间: 081118
// 最后确定: 081125

#include "SampleApp.h"
#include "../Utils.h"
#include "../RenderSystem/RenderSystem.h"
#include "../AudioSystem/AudioSystem.h"
#include "../InputSystem/InputSystem.h"
#include "../FileSystem/FileSystem.h"
#include "../FrameWork/NodeManager.h"
#include "../Sprites/Sprite.h"
#include "../FrameWork/SceneManager.h"
#pragma warning(disable: 4244)

namespace AnyPlay
{
	SampleApp::SampleApp(Interface* firstInf) : Application(firstInf)
	{
		m_hWnd = NULL;
		m_wndWidth = 640;
		m_wndHeight = 480;
		m_strClassName = _T("AnyPlay Class");
		m_strWindowName = _T("AnyPlay Application");
	}
	//////////////////////////////////////////////////////////////////////////
	// 初始化程序
	bool SampleApp::Initialise()
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

		// 音频系统
		AudioSystem* as = new AudioSystem;

		// 渲染系统
		RenderSystem* rs = new RenderSystem;
		if(!rs->CreateRenderSystem(m_wndWidth, m_wndHeight, m_hWnd, true, true, false, false)) bResult = false;
		Font* fontSong = new Font;
		if(!fontSong->CreateFont(_T("宋体"))) bResult = false;
		rs->FontMgr.Add(fontSong);

		// 节点管理
		NodeManager* nm = new NodeManager;

		// 场景管理
		SceneManager* sm = new SceneManager;

		return bResult;
	}

	void SampleApp::Cleanup()
	{
		SAFE_DELETE(m_pNextInterface);
		// 释放游戏接口
		DestroyCurInterface();

		// 释放场景管理
		SceneManager* sm = SceneManager::GetSingletonPtr();
		SAFE_DELETE(sm);

		// 释放节点管理
		NodeManager* nm = NodeManager::GetSingletonPtr();
		SAFE_DELETE(nm);

		// 释放渲染系统
		RenderSystem* rs = RenderSystem::GetSingletonPtr();
		SAFE_DELETE(rs);

		// 释放音频系统
		AudioSystem* as = AudioSystem::GetSingletonPtr();
		SAFE_DELETE(as);

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
	void SampleApp::MainLoop()
	{
		// 如果需要更新接口
		Application::MainLoop();
		// 扫描输入
		InputSystem* is = InputSystem::GetSingletonPtr();
		is->Scan();

		// 计算延迟
		float deltaTime = m_Timer.GetElapsedTime() * 1.5f;// 被修改过
		String strFPS;
		strFPS.Format(_T("帧率: %g FPS"), 1000.f / deltaTime);

		// 接口循环
		if(m_pCurrentInterface) m_pCurrentInterface->MainLoop(deltaTime);

		SceneManager::GetSingleton().AdvanceScene(deltaTime);
		// 绘制场景
		RenderSystem* rs = RenderSystem::GetSingletonPtr();
		rs->BeginScene(true, false, 0xFF000000);
		{
			SceneManager::GetSingleton().DrawScene();

			Font* fontSong = rs->FontMgr.Get(_T("宋体"));
			fontSong->DrawFont(strFPS, -1, &Rect(0, 0, 200, 20), DT_LEFT, 0xFFFF0000);
		}
		rs->EndScene(NULL);
	}

	LRESULT SampleApp::OnWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch(message)
		{
		case WM_SETFOCUS:
			if(InputSystem* is = InputSystem::GetSingletonPtr()) is->SetActivity(true);
			break;
		case WM_KILLFOCUS:
			if(InputSystem* is = InputSystem::GetSingletonPtr()) is->SetActivity(false);
			break;

		case WM_SYSCOMMAND:		// 当程序最小化时设置为非活动状态
			if(wParam == SC_MINIMIZE)
			{
				if(AudioSystem* as = AudioSystem::GetSingletonPtr()) as->PauseAll();
				m_bActivity = false;
			}
			else if(wParam == SC_RESTORE)
			{
				m_bActivity = true;
				m_Timer.StartCount();
				if(AudioSystem* as = AudioSystem::GetSingletonPtr()) as->ResumeAll();
			}
			break;
		case WM_DESTROY:		// 退出程序消息
			m_bActivity = false;
			PostQuitMessage(0);	// 退出
			break;
		}
		return Application::OnWindowMessage(hWnd, message, wParam, lParam);
	}

	void SampleApp::UpdateInterface()
	{
		Application::UpdateInterface();
		// 开始计时
		m_Timer.StartCount();
	}
}
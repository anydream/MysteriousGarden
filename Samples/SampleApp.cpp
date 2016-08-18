/************************************************************************/
// ����ʱ��: 081118
// ���ȷ��: 081125

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
	// ��ʼ������
	bool SampleApp::Initialise()
	{
		// ��ʼ��
		bool bResult = true;

		m_Timer.InitTimer();
		// ��־��¼��
		Logger* logger = new Logger;

		// ����
		HICON hIcon = 0;//LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MAINICON));
		m_hWnd = InitWindow(m_strClassName(), m_strWindowName(), m_wndWidth, m_wndHeight, WinProc, hIcon, hIcon, 0);
		if(!m_hWnd) bResult = false;

		// �ļ�ϵͳ
		FileSystem* fs = new FileSystem;

		// ����ϵͳ
		InputSystem* is = new InputSystem;
		is->BeginHideCursor(LoadCursor(NULL, IDC_ARROW));

		// ��Ƶϵͳ
		AudioSystem* as = new AudioSystem;

		// ��Ⱦϵͳ
		RenderSystem* rs = new RenderSystem;
		if(!rs->CreateRenderSystem(m_wndWidth, m_wndHeight, m_hWnd, true, true, false, false)) bResult = false;
		Font* fontSong = new Font;
		if(!fontSong->CreateFont(_T("����"))) bResult = false;
		rs->FontMgr.Add(fontSong);

		// �ڵ����
		NodeManager* nm = new NodeManager;

		// ��������
		SceneManager* sm = new SceneManager;

		return bResult;
	}

	void SampleApp::Cleanup()
	{
		SAFE_DELETE(m_pNextInterface);
		// �ͷ���Ϸ�ӿ�
		DestroyCurInterface();

		// �ͷų�������
		SceneManager* sm = SceneManager::GetSingletonPtr();
		SAFE_DELETE(sm);

		// �ͷŽڵ����
		NodeManager* nm = NodeManager::GetSingletonPtr();
		SAFE_DELETE(nm);

		// �ͷ���Ⱦϵͳ
		RenderSystem* rs = RenderSystem::GetSingletonPtr();
		SAFE_DELETE(rs);

		// �ͷ���Ƶϵͳ
		AudioSystem* as = AudioSystem::GetSingletonPtr();
		SAFE_DELETE(as);

		// �ͷ�����ϵͳ
		InputSystem* is = InputSystem::GetSingletonPtr();
		SAFE_DELETE(is);

		// �ͷ��ļ�ϵͳ
		FileSystem* fs = FileSystem::GetSingletonPtr();
		SAFE_DELETE(fs);

		// �ͷ���־��¼��
		Logger* logger = Logger::GetSingletonPtr();
		SAFE_DELETE(logger);
	}

	//////////////////////////////////////////////////////////////////////////
	// ��ѭ��
	void SampleApp::MainLoop()
	{
		// �����Ҫ���½ӿ�
		Application::MainLoop();
		// ɨ������
		InputSystem* is = InputSystem::GetSingletonPtr();
		is->Scan();

		// �����ӳ�
		float deltaTime = m_Timer.GetElapsedTime() * 1.5f;// ���޸Ĺ�
		String strFPS;
		strFPS.Format(_T("֡��: %g FPS"), 1000.f / deltaTime);

		// �ӿ�ѭ��
		if(m_pCurrentInterface) m_pCurrentInterface->MainLoop(deltaTime);

		SceneManager::GetSingleton().AdvanceScene(deltaTime);
		// ���Ƴ���
		RenderSystem* rs = RenderSystem::GetSingletonPtr();
		rs->BeginScene(true, false, 0xFF000000);
		{
			SceneManager::GetSingleton().DrawScene();

			Font* fontSong = rs->FontMgr.Get(_T("����"));
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

		case WM_SYSCOMMAND:		// ��������С��ʱ����Ϊ�ǻ״̬
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
		case WM_DESTROY:		// �˳�������Ϣ
			m_bActivity = false;
			PostQuitMessage(0);	// �˳�
			break;
		}
		return Application::OnWindowMessage(hWnd, message, wParam, lParam);
	}

	void SampleApp::UpdateInterface()
	{
		Application::UpdateInterface();
		// ��ʼ��ʱ
		m_Timer.StartCount();
	}
}
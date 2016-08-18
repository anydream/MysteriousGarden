

#include "MyApp.h"
#include "../../Utils.h"
#include "../../RenderSystem/RenderSystem.h"
#include "../../InputSystem/InputSystem.h"
#include "../../FileSystem/FileSystem.h"
#include "../../FrameWork/AnmManager.h"

namespace AnyPlay
{
	//////////////////////////////////////////////////////////////////////////
	// ��ʼ������
	bool MyApp::Initialise()
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

		// ��Ⱦϵͳ
		RenderSystem* rs = new RenderSystem;
		if(!rs->CreateRenderSystem(m_wndWidth, m_wndHeight, m_hWnd, true, true, false, false)) bResult = false;

		// ��������
		AnmManager* am = new AnmManager;

		// ��������

		return bResult;
	}

	void MyApp::Cleanup()
	{
		SAFE_DELETE(m_pNextInterface);
		// �ͷ���Ϸ�ӿ�
		DestroyCurInterface();

		// �ͷų�������

		// �ͷŶ�������
		AnmManager* am = AnmManager::GetSingletonPtr();
		SAFE_DELETE(am);

		// �ͷ���Ⱦϵͳ
		RenderSystem* rs = RenderSystem::GetSingletonPtr();
		SAFE_DELETE(rs);

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
	void MyApp::MainLoop()
	{
		// �����Ҫ���½ӿ�
		Application::MainLoop();
		// ɨ������
		InputSystem* is = InputSystem::GetSingletonPtr();
		is->Scan();

		// �����ӳ�
		float deltaTime = m_Timer.GetElapsedTime();

		// �ӿ�ѭ��
		if(m_pCurrentInterface) m_pCurrentInterface->MainLoop(deltaTime);

		// �ƽ�֡
		AnmManager::GetSingleton().Advance(deltaTime);

		// ���Ƴ���
		RenderSystem* rs = RenderSystem::GetSingletonPtr();
		rs->BeginScene(true, false, 0xFF000000);
		{
			//Texture* tex = (Texture*)rs->Get(_T("δ����.tex"));
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

		case WM_SYSCOMMAND:		// ��������С��ʱ����Ϊ�ǻ״̬
			if(wParam == SC_MINIMIZE) m_bActivity = false;
			else if(wParam == SC_RESTORE)
			{
				m_bActivity = true;
				m_Timer.StartCount();
			}
			break;
		case WM_DESTROY:		// �˳�������Ϣ
			m_bActivity = false;
			PostQuitMessage(0);	// �˳�
			break;
		}
		return Application::OnWindowMessage(hWnd, message, wParam, lParam);
	}

	void MyApp::UpdateInterface()
	{
		Application::UpdateInterface();
		// ��ʼ��ʱ
		m_Timer.StartCount();
	}
}
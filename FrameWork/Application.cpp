/************************************************************************/
// ����ʱ��: 080918
// ���ȷ��: 081118

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
		// ����ȱʡ��Ϣ�������
		if(bFlag) return DefWindowProc(hWnd, message, wParam, lParam);
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	// ���г���, ����ѭ��
	dword Application::Run()
	{
		if(!Initialise())	// ��������ʼ��ʧ��
		{
			// �ͷŲ����ش���
			Cleanup();
			return -1;
		}
		// ���ó���ɻ
		m_bActivity = true;

		MSG msg;
		// ��ʼ��������Ϣѭ��
		while(1)
		{
			if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
			{
				if(!GetMessage(&msg, NULL, 0, 0 )) break;	// ���������˳�, ������Ϣѭ��
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else if(m_bActivity) MainLoop();	// ������ѭ��
			else WaitMessage();					// �ȴ���Ϣ
		}
		// �ͷų���
		Cleanup();
		// �����˳���
		return msg.wParam;
	}

	void Application::MainLoop()
	{
		if(m_bUpdateInterface)
			UpdateInterface();
	}
	
	//////////////////////////////////////////////////////////////////////////
	// ���ýӿ�
	void Application::SetNextInterface(Interface* inf)
	{
		if(inf) m_pNextInterface = inf;
	}

	//////////////////////////////////////////////////////////////////////////
	// ���ٽӿ�
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
	// ���½ӿ�
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
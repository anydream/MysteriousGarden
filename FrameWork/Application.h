/************************************************************************/
// ����ʱ��: 081028
// ���ȷ��: 081119

#ifndef _Application_h_
#define _Application_h_

#include "../AnyPlay.h"
#include "../Core/String.h"
#include "Singleton.h"
#include "Interface.h"
#include "Logger.h"

//////////////////////////////////////////////////////////////////////////
// AnyPlay���ƿռ�
namespace AnyPlay
{
	//////////////////////////////////////////////////////////////////////////
	// ������Ϣ�ص�����
	LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//////////////////////////////////////////////////////////////////////////
	// Ӧ�ó�����
	// �����齨������, ֻ�ܴ���һ������, ����ȫ���κεط�������
	class Application : public Singleton<Application>
	{
	public:
		Application(Interface* firstInf);
		~Application() {}

		// ��ʼ����
		dword			Run();

		// ���ø��½ӿڱ��
		virtual void	SetUpdateFlag(bool flag) { m_bUpdateInterface = flag; }
		// ��ø��½ӿڱ��
		virtual bool	GetUpdateFlag() const { return m_bUpdateInterface; }
		// ������һ�����½ӿ�ָ��
		virtual void	SetNextInterface(Interface* inf);
		// ���ٵ�ǰ���½ӿ�
		virtual bool	DestroyCurInterface();
		// ���µ�ǰ�ӿ�
		virtual void	UpdateInterface();

		//////////////////////////////////////////////////////////////////////////
		// ��ʼ��Ӧ�ó���
		virtual bool	Initialise() = 0;
		// �ͷ�Ӧ�ó���
		virtual void	Cleanup() = 0;
		// ��ѭ��
		virtual void	MainLoop();

		// ��Ӧ������Ϣ
		virtual LRESULT	OnWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		virtual ushort	GetWindowWidth() const { return 0; }
		virtual ushort	GetWindowHeight() const { return 0; }

	protected:
		bool			m_bActivity;		// ����ɻ��

		bool			m_bUpdateInterface;		// �Ƿ�Ҫ���½ӿ�
		Interface*		m_pCurrentInterface;	// ��ǰ�ӿ�
		Interface*		m_pNextInterface;		// ��һ�ӿ�
	};
}

#endif
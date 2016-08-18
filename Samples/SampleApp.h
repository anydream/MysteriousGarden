/************************************************************************/
// ����ʱ��: 081118
// ���ȷ��: 081125

#ifndef _SampleApp_h_
#define _SampleApp_h_

#include "../AnyPlay.h"
#include "../FrameWork/Application.h"
#include "../Core/Timer.h"
namespace AnyPlay
{
	class SampleApp : public Application
	{
	public:
		SampleApp(Interface* firstInf);

		bool		Initialise();
		void		Cleanup();
		void		MainLoop();
		LRESULT		OnWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		void		UpdateInterface();

		ushort		GetWindowWidth() const { return m_wndWidth; }
		ushort		GetWindowHeight() const { return m_wndHeight; }

	private:
		HWND		m_hWnd;				// ���ھ��
		ushort		m_wndWidth;			// ���ڿ��
		ushort		m_wndHeight;		// ���ڸ߶�
		String		m_strClassName;		// ��������
		String		m_strWindowName;	// ������

		Timer		m_Timer;			// ��ʱ��
	};
}
#endif
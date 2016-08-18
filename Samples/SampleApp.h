/************************************************************************/
// 创建时间: 081118
// 最后确定: 081125

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
		HWND		m_hWnd;				// 窗口句柄
		ushort		m_wndWidth;			// 窗口宽度
		ushort		m_wndHeight;		// 窗口高度
		String		m_strClassName;		// 窗口类名
		String		m_strWindowName;	// 窗口名

		Timer		m_Timer;			// 定时器
	};
}
#endif
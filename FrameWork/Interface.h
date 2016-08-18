/************************************************************************/
// 创建时间: 081028
// 最后确定: 081028

#ifndef _Interface_h_
#define _Interface_h_

#include "../AnyPlay.h"

namespace AnyPlay
{
	class Interface
	{
	public:
		Interface() {}
		virtual ~Interface() {}
		virtual bool Initialise() = 0;
		virtual bool Cleanup() = 0;
		virtual void MainLoop(float deltaTime) = 0;
		virtual bool OnWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
	};
}
#endif
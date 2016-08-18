/************************************************************************/
// 创建时间: 080929
// 最后确定: 081119

#ifndef _InputSystem_h_
#define _InputSystem_h_

#include "../AnyPlay.h"
#include "../FrameWork/Singleton.h"
#include "../FrameWork/Event.h"
#include "../Core/Vector4.h"

#define KEY_NUM 256		// 键个数
// 获得某键状态
#define KEY_STATE(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? true : false)
// 获得鼠标窗口坐标
#define MOUSE_POS(_pos) { POINT _point; GetCursorPos(&_point); ScreenToClient(GetActiveWindow(), &_point); (_pos).x = _point.x; (_pos).y = _point.y; }

//////////////////////////////////////////////////////////////////////////
// AnyPlay名称空间
namespace AnyPlay
{
	static const String KeyEvent = _T("键");
	static const String MotionEvent = _T("鼠");
	//////////////////////////////////////////////////////////////////////////
	// API输入系统
	// 使用WinAPI来实现输入检测
	class InputSystem : public Singleton<InputSystem>, public EventSet
	{
	public:
		InputSystem();
		~InputSystem() {}

		void	HideCursor();
		void	Scan();

		// 获得鼠标坐标
		Point	GetMousePos() const { return m_MousePos; }
		// 获得虚拟键状态
		bool	GetVKState(byte vk) const { return m_KeyStates[vk]; }

		// 设置可活动性
		void	SetActivity(bool ac) { m_Activity = ac; }
		void	SetActivity2(bool ac) { m_Activity2 = ac; }
		// 获得可活动性
		bool	GetActivity() const { return m_Activity && m_Activity2; }

		void	BeginHideCursor(HCURSOR hDefCur);
		void	EndHideCursor();

	private:
		bool			m_Activity;				// 可活动性
		bool			m_Activity2;			// 可活动性2
		bool			m_KeyStates[KEY_NUM];	// 虚拟键状态
		Point			m_MousePos;				// 鼠标坐标

		bool			m_HideCursor;			// 隐藏光标
		HCURSOR			m_DefaultCur;			// 默认光标
	};

	//////////////////////////////////////////////////////////////////////////
	// 输入事件
	class KeyEventArgs : public EventArgs
	{
	public:
		KeyEventArgs(int vk, bool state) : m_VirtualKey(vk), m_bState(state) {}

	public:
		int		m_VirtualKey;
		bool	m_bState;
	};

	//////////////////////////////////////////////////////////////////////////
	// 鼠标移动事件
	class MotionEventArgs : public EventArgs
	{
	public:
		MotionEventArgs(const Point& offset) : m_Offset(offset) {}

	public:
		Point	m_Offset;
	};
}
#endif
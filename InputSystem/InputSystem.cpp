/************************************************************************/
// 创建时间: 080929
// 最后确定: 081119

#include "InputSystem.h"

#pragma warning(disable: 4244)

namespace AnyPlay
{
	template<> InputSystem* Singleton<InputSystem>::m_Singleton = 0;

	InputSystem::InputSystem()
	{
		m_Activity = true;
		m_Activity2 = true;
		// 初始化位移/按键状态
		for(int i = 0; i < KEY_NUM; i++)
			m_KeyStates[i] = KEY_STATE(i);
		MOUSE_POS(m_MousePos);

		m_HideCursor = false;
		m_DefaultCur = NULL;
	}

	void InputSystem::HideCursor()
	{
		if(m_HideCursor)
		{
			if(m_MousePos.y >= 0)
			{
				if(GetCursor()) SetCursor(0);
			}
			else
			{
				if(GetCursor() != m_DefaultCur) SetCursor(m_DefaultCur);
			}
		}
	}
	
	void InputSystem::Scan()
	{
		if(m_Activity && m_Activity2)
		{
			// 扫描按键
			for(int i = 0; i < KEY_NUM; i++)
			{
				bool bKey = KEY_STATE(i);
				if(m_KeyStates[i] != bKey)
				{
					m_KeyStates[i] = bKey;
					FireEvent(KeyEvent, KeyEventArgs(i, bKey));
				}
			}
			
			// 扫描位移
			Point mousePoint;
			MOUSE_POS(mousePoint);
			if(m_MousePos != mousePoint)
			{
				Point offset = mousePoint - m_MousePos;
				m_MousePos = mousePoint;

				HideCursor();

				FireEvent(MotionEvent, MotionEventArgs(offset));
			}
		}
	}

	void InputSystem::BeginHideCursor(HCURSOR hDefCur)
	{
		m_HideCursor = true;
		m_DefaultCur = hDefCur;

		HideCursor();
	}

	void InputSystem::EndHideCursor()
	{
		m_HideCursor = false;
		SetCursor(m_DefaultCur);
	}
}
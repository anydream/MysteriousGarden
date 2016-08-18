/************************************************************************/
// ����ʱ��: 080929
// ���ȷ��: 081119

#ifndef _InputSystem_h_
#define _InputSystem_h_

#include "../AnyPlay.h"
#include "../FrameWork/Singleton.h"
#include "../FrameWork/Event.h"
#include "../Core/Vector4.h"

#define KEY_NUM 256		// ������
// ���ĳ��״̬
#define KEY_STATE(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? true : false)
// �����괰������
#define MOUSE_POS(_pos) { POINT _point; GetCursorPos(&_point); ScreenToClient(GetActiveWindow(), &_point); (_pos).x = _point.x; (_pos).y = _point.y; }

//////////////////////////////////////////////////////////////////////////
// AnyPlay���ƿռ�
namespace AnyPlay
{
	static const String KeyEvent = _T("��");
	static const String MotionEvent = _T("��");
	//////////////////////////////////////////////////////////////////////////
	// API����ϵͳ
	// ʹ��WinAPI��ʵ��������
	class InputSystem : public Singleton<InputSystem>, public EventSet
	{
	public:
		InputSystem();
		~InputSystem() {}

		void	HideCursor();
		void	Scan();

		// ����������
		Point	GetMousePos() const { return m_MousePos; }
		// ��������״̬
		bool	GetVKState(byte vk) const { return m_KeyStates[vk]; }

		// ���ÿɻ��
		void	SetActivity(bool ac) { m_Activity = ac; }
		void	SetActivity2(bool ac) { m_Activity2 = ac; }
		// ��ÿɻ��
		bool	GetActivity() const { return m_Activity && m_Activity2; }

		void	BeginHideCursor(HCURSOR hDefCur);
		void	EndHideCursor();

	private:
		bool			m_Activity;				// �ɻ��
		bool			m_Activity2;			// �ɻ��2
		bool			m_KeyStates[KEY_NUM];	// �����״̬
		Point			m_MousePos;				// �������

		bool			m_HideCursor;			// ���ع��
		HCURSOR			m_DefaultCur;			// Ĭ�Ϲ��
	};

	//////////////////////////////////////////////////////////////////////////
	// �����¼�
	class KeyEventArgs : public EventArgs
	{
	public:
		KeyEventArgs(int vk, bool state) : m_VirtualKey(vk), m_bState(state) {}

	public:
		int		m_VirtualKey;
		bool	m_bState;
	};

	//////////////////////////////////////////////////////////////////////////
	// ����ƶ��¼�
	class MotionEventArgs : public EventArgs
	{
	public:
		MotionEventArgs(const Point& offset) : m_Offset(offset) {}

	public:
		Point	m_Offset;
	};
}
#endif
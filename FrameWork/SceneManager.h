/************************************************************************/
// ����ʱ��: 081030
// ���ȷ��: 081119

#ifndef _SceneManager_h_
#define _SceneManager_h_

#include "../AnyPlay.h"
#include "../FrameWork/Singleton.h"
#include "../Sprites/Sprite.h"
#include "../InputSystem/InputSystem.h"

namespace AnyPlay
{
	static const String LostMouseFocusEvent = _T("ʧ��");
	static const String GetMouseFocusEvent = _T("����");

	static const String LostFocusEvent = _T("ʧ��");
	static const String GetFocusEvent = _T("�ý�");

	static const String MouseMoveEvent = _T("����");
	static const String DraggingEvent = _T("����");
	static const String MouseDownEvent = _T("����");
	static const String MouseUpEvent = _T("����");
	static const String MouseClickEvent = _T("���");

	//////////////////////////////////////////////////////////////////////////
	// ��������
	class SceneManager : public Singleton<SceneManager>
	{
	public:
		SceneManager();
		virtual ~SceneManager() { if(m_pRoot) m_pRoot->KillDescendants(); }

		void		Clear();

		void		SetRoot(Sprite* pSpr) { m_pRoot = pSpr; }
		Sprite*		GetRoot() const { return m_pRoot; }

		Sprite*		GetFocus() const { return m_pFocus; }
		Sprite*		GetMouseFocus() const { return m_pMouseFocus; }

		void		BeginDragging() { m_bDragging = true; }
		void		EndDragging() { m_bDragging = false; }

		void		AdvanceScene(float deltaTime);
		void		DrawScene();

	public:
		bool		OnKey(const EventArgs& args);
		bool		OnMotion(const EventArgs& args);

	private:
		Sprite*		m_pRoot;
		Sprite*		m_pFocus;
		Sprite*		m_pMouseFocus;
		bool		m_bDragging;
	};
}

#endif
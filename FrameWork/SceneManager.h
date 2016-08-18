/************************************************************************/
// 创建时间: 081030
// 最后确定: 081119

#ifndef _SceneManager_h_
#define _SceneManager_h_

#include "../AnyPlay.h"
#include "../FrameWork/Singleton.h"
#include "../Sprites/Sprite.h"
#include "../InputSystem/InputSystem.h"

namespace AnyPlay
{
	static const String LostMouseFocusEvent = _T("失鼠焦");
	static const String GetMouseFocusEvent = _T("得鼠焦");

	static const String LostFocusEvent = _T("失焦");
	static const String GetFocusEvent = _T("得焦");

	static const String MouseMoveEvent = _T("鼠移");
	static const String DraggingEvent = _T("鼠拖");
	static const String MouseDownEvent = _T("鼠下");
	static const String MouseUpEvent = _T("鼠上");
	static const String MouseClickEvent = _T("鼠击");

	//////////////////////////////////////////////////////////////////////////
	// 场景管理
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
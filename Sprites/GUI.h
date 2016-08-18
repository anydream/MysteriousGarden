/************************************************************************/
// 创建时间: 081125
// 最后确定: 081130

#ifndef _GUI_h_
#define _GUI_h_

#include "../AnyPlay.h"
#include "../FrameWork/SceneManager.h"
#include "../RenderSystem/Font.h"
#include "Sprite.h"

namespace AnyPlay
{
	//////////////////////////////////////////////////////////////////////////
	// 鼠标光标
	class GUICursor : public Sprite
	{
	public:
		GUICursor() {}

		void Update()
		{
			m_Position = InputSystem::GetSingleton().GetMousePos();
		}

	};

	class GUIButton : public Sprite
	{
	public:
		GUIButton();
		bool OnLostMouseFocus(const EventArgs& args);
		bool OnGetMouseFocus(const EventArgs& args);
		bool OnMouseDown(const EventArgs& args);
		bool OnMouseUp(const EventArgs& args);
	};

	//////////////////////////////////////////////////////////////////////////
	// 窗口
	class GUIWindow : public Sprite
	{
	public:
		GUIWindow();
		bool OnMouseDown(const EventArgs& args);
		bool OnMouseUp(const EventArgs& args);
		bool OnDragging(const EventArgs& args);

	public:
		Rect	m_MoveBox;
		Point	m_HitPoint;
	};

	//////////////////////////////////////////////////////////////////////////
	// 标签
	class GUILabel : public Sprite
	{
	public:
		GUILabel(Font* pFont) : m_pFont(pFont), m_strText(_T("?")), m_dwFormat(DT_RIGHT | DT_BOTTOM), m_Color(0xFF053805), m_rcDest(0, 0, 55, 34) {}
		void DrawSprite(const Point& dstPos, const Point& srcPos, const Color& color)
		{
			Sprite::DrawSprite(dstPos, srcPos, color);
			if(m_pFont)
			{
				Rect rcDst = m_rcDest + m_Position + dstPos;
				m_pFont->DrawFont(m_strText, -1, &rcDst, m_dwFormat, m_Color);
			}
		}

		/*Sprite* GetMouseFocus(const Point& point)
		{
			if(m_rcDest.Hit(point - m_Position)) return this;
			return NULL;
		}*/

	public:
		Font*	m_pFont;
		String	m_strText;
		dword	m_dwFormat;
		Color	m_Color;
		Rect	m_rcDest;
	};

	//////////////////////////////////////////////////////////////////////////
	// 提示
	class GUITips : public Sprite
	{
	public:
		GUITips(Font *pFont);
		void DrawSprite(const Point& pos, const Point& srcPos, const Color& color);
		void Show();
		void Hide();
		bool OnClick(const EventArgs& args);
		void Update(float deltaTime);

		Font*	m_pFont;
		String	m_strTips;
		float	m_DeltaTime;
		float	m_Count;
	};
#define SHOW_TIPS(_s)\
{\
	GameInf::GetSingleton().m_pTips->m_strTips = _s;\
	GameInf::GetSingleton().m_pTips->Show();\
}\

	GUIWindow* MakeWindow(const String& name, const String& caption, ushort width, ushort height, const SubscriberSlot& closesub);
	void MakeButton(Sprite& spr, const String& name, const Point& pos, const Rect& hitBox,
		const Rect& s1, const Rect& d1,
		const Rect& s2, const Rect& d2,
		const Rect& s3, const Rect& d3);
}

#endif
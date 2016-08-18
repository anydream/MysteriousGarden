/************************************************************************/
// 创建时间: 081130
// 最后确定: 081130

#include "GUI.h"
#include "../RenderSystem/RenderSystem.h"

namespace AnyPlay
{
	GUIButton::GUIButton()
	{
		SubscribeEvent(LostMouseFocusEvent, SubscriberSlot(&GUIButton::OnLostMouseFocus, this));
		SubscribeEvent(GetMouseFocusEvent, SubscriberSlot(&GUIButton::OnGetMouseFocus, this));

		SubscribeEvent(MouseDownEvent, SubscriberSlot(&GUIButton::OnMouseDown, this));
		SubscribeEvent(MouseUpEvent, SubscriberSlot(&GUIButton::OnMouseUp, this));
	}

	bool GUIButton::OnLostMouseFocus(const EventArgs& args)
	{
		Play();
		SetNextFrmPos(0);
		return true;
	}

	bool GUIButton::OnGetMouseFocus(const EventArgs& args)
	{
		InputSystem* is = InputSystem::GetSingletonPtr();
		SceneManager* sm = SceneManager::GetSingletonPtr();

		Play();
		if(is->GetVKState(0x1) && (sm->GetFocus() == this)) SetNextFrmPos(2);
		else SetNextFrmPos(1);

		return true;
	}

	bool GUIButton::OnMouseDown(const EventArgs& args)
	{
		Play();
		SetNextFrmPos(2);
		return true;
	}

	bool GUIButton::OnMouseUp(const EventArgs& args)
	{
		Play();
		SetNextFrmPos(1);
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	GUIWindow::GUIWindow()
	{
		m_MoveBox = Rect(-1000, -1000, 1000, 1000);

		SubscribeEvent(MouseDownEvent, SubscriberSlot(&GUIWindow::OnMouseDown, this));
		SubscribeEvent(MouseUpEvent, SubscriberSlot(&GUIWindow::OnMouseUp, this));

		SubscribeEvent(DraggingEvent, SubscriberSlot(&GUIWindow::OnDragging, this));
	}

	bool GUIWindow::OnMouseDown(const EventArgs& args)
	{
		m_HitPoint = InputSystem::GetSingleton().GetMousePos() - m_Position;
		SceneManager::GetSingleton().BeginDragging();
		return true;
	}

	bool GUIWindow::OnMouseUp(const EventArgs& args)
	{
		SceneManager::GetSingleton().EndDragging();
		SceneManager::GetSingleton().OnMotion(MotionEventArgs(Point()));
		return true;
	}

	bool GUIWindow::OnDragging(const EventArgs& args)
	{
		m_Position = InputSystem::GetSingleton().GetMousePos() - m_HitPoint;

		if(m_Position.x < m_MoveBox.m_Left) m_Position.x = m_MoveBox.m_Left;
		else if(m_Position.x > m_MoveBox.m_Right) m_Position.x = m_MoveBox.m_Right;

		if(m_Position.y < m_MoveBox.m_Top) m_Position.y = m_MoveBox.m_Top;
		else if(m_Position.y > m_MoveBox.m_Bottom) m_Position.y = m_MoveBox.m_Bottom;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	GUITips::GUITips(Font *pFont) : m_pFont(pFont), m_DeltaTime(1500.f), m_Count(0.0f), m_strTips(_T("无提示"))
	{
		Stop();
		SubscribeEvent(MouseClickEvent, SubscriberSlot(&GUITips::OnClick, this));
	}

	void GUITips::DrawSprite(const Point& pos, const Point& srcPos, const Color& color)
	{
		Sprite::DrawSprite(pos, srcPos, color);
		if(m_pFont)
		{
			SprFrame* pSF = (SprFrame*)GetCurrentFrame();
			float alp = pSF->m_LayerList.begin()->m_Color[0].m_Alpha;
			m_pFont->DrawFont(m_strTips, -1, &(Area(13, 14, 204, 68) + m_Position), DT_CENTER | DT_VCENTER, Color(alp, 0.5f, 0.f, 1.f)/*0xFF8000FF*/);
		}
	}

	void GUITips::Show()
	{
		m_Count = 0.0f;
		if(GetCurrentFrmPos() == 0)
		{
			Goto(0);
			Play();
		}
	}

	void GUITips::Hide()
	{
		if(GetCurrentFrmPos() == 51)
		{
			Goto(52);
			Play();
		}
	}

	bool GUITips::OnClick(const EventArgs& args)
	{
		Hide();
		return true;
	}

	void GUITips::Update(float deltaTime)
	{
		if(GetCurrentFrmPos() == 51)
		{
			m_Count += deltaTime;
			if(m_Count > m_DeltaTime) Play();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	GUIWindow* MakeWindow(const String& name, const String& caption, ushort width, ushort height, const SubscriberSlot& closesub)
	{
		RenderSystem* rs = RenderSystem::GetSingletonPtr();
		Texture* pTexSkin = rs->TextureMgr.Get(_T("皮肤.tex"));

		GUIWindow* pWnd = new GUIWindow;
		pWnd->SetName(name);
		SprFrame* pSF = new SprFrame(0.f);
		pWnd->AddFrame(pSF);

		// 上左
		pSF->AddLayer(Layer(pTexSkin, Area(1, 64, 11, 35), Area(0, 0, 11, 35), Color(), Area(0, 0, 11, 35)));
		// 上右
		pSF->AddLayer(Layer(pTexSkin, Area(80, 64, 14, 35), Area(width - 14, 0, 14, 35), Color(), Area(width - 14, 0, 14, 35)));
		// 上中
		pSF->AddLayer(Layer(pTexSkin, Area(12, 64, 67, 35), Area(11, 0, width - 14 - 11, 35), Color(), Area(11, 0, width - 14 - 11, 35)));

		// 中
		pSF->AddLayer(Layer(pTexSkin, Area(12, 99, 67, 25), Area(11, 35, width - 14 - 11, height - 35 - 14), Color()/*, Area(11, 35, width - 14 - 11, height - 35 - 14)*/));

		// 中左
		pSF->AddLayer(Layer(pTexSkin, Area(1, 99, 11, 25), Area(0, 35, 11, height - 35 - 14), Color()/*, Area(0, 35, 11, height - 35 - 14)*/));
		// 中右
		pSF->AddLayer(Layer(pTexSkin, Area(80, 99, 14, 25), Area(width - 14, 35, 14, height - 35 - 14), Color()/*, Area(width - 14, 35, 14, height - 35 - 14)*/));
		
		// 下左
		pSF->AddLayer(Layer(pTexSkin, Area(1, 125, 11, 14), Area(0, height - 14, 11, 14), Color()/*, Area(0, height - 14, 11, 14)*/));
		// 下右
		pSF->AddLayer(Layer(pTexSkin, Area(80, 125, 14, 14), Area(width - 14, height - 14, 14, 14), Color()/*, Area(width - 14, height - 14, 14, 14)*/));
		// 下中
		pSF->AddLayer(Layer(pTexSkin, Area(12, 125, 67, 14), Area(11, height - 14, width - 14 - 11, 14), Color()/*, Area(11, height - 14, width - 14 - 11, 14)*/));

		GUILabel* pCaption = new GUILabel(RenderSystem::GetSingleton().FontMgr.Get(_T("宋体")));
		pCaption->m_Color = 0xFFFFFFFF;
		pCaption->m_dwFormat = DT_CENTER | DT_VCENTER;
		pCaption->m_rcDest = Area(11, 5, width - 14 - 11, 20);
		pCaption->m_strText = caption;
		pWnd->AttachChild(pCaption);

		GUIButton* pClose = new GUIButton;
		MakeButton(*pClose, _T("关闭按钮"), Point(width - 25, 7),
			Area(0, 0, 16, 16),
			Area(123, 84, 16, 16), Area(0, 0, 16, 16),
			Area(123, 65, 16, 16), Area(0, 0, 16, 16),
			Area(101, 84, 16, 16), Area(0, 0, 16, 16));
		pWnd->AttachChild(pClose);

		pClose->SubscribeEvent(MouseClickEvent, closesub);

		return pWnd;
	}


	void MakeButton(Sprite& spr, const String& name, const Point& pos, const Rect& hitBox,
		const Rect& s1, const Rect& d1,
		const Rect& s2, const Rect& d2,
		const Rect& s3, const Rect& d3)
	{
		RenderSystem* rs = RenderSystem::GetSingletonPtr();
		Texture* tex = rs->TextureMgr.Get(_T("皮肤.tex"));
		spr.SetName(name);
		spr.SetPosition(pos);
		spr.SetActivity(false);
		spr.SetSmoothMode(true);
		spr.AddFrame(new SprFrame(Layer(tex, s1, d1, Color(), hitBox), 200.f));
		spr.GetFrame(0)->m_bStop = true;
		spr.AddFrame(new SprFrame(Layer(tex, s2, d2, Color(), hitBox), 200.f));
		spr.GetFrame(1)->m_bStop = true;
		spr.AddFrame(new SprFrame(Layer(tex, s3, d3, Color(), hitBox), 200.f));
		spr.GetFrame(2)->m_bStop = true;
		spr.m_GotoMap[_T("常")] = 0;
		spr.m_GotoMap[_T("上")] = 1;
		spr.m_GotoMap[_T("下")] = 2;
	}
}
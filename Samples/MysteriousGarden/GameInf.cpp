/************************************************************************/
// ����ʱ��: 081126
// ���ȷ��: 081126

#include "GameInf.h"
#include "MainInf.h"

namespace AnyPlay
{
	template<> GameInf* Singleton<GameInf>::m_Singleton = 0;

	GameInf::GameInf()
	{
		m_pRoot = NULL;
		m_pScene = NULL;
		m_pGUI = NULL;
		m_pTop = NULL;
		m_pMask = NULL;
		m_pCtrlRole = NULL;
		m_pSprRoleWnd = NULL;
		m_pSprPass = NULL;

		m_pBlueTip = NULL;
		m_pRedTip = NULL;
		m_pBlackTip = NULL;
		m_pYellowTip = NULL;
		m_pPinkTip = NULL;
		m_pMailTip = NULL;
		m_pPassTip = NULL;

		m_pCursor = NULL;
		m_pTips = NULL;
		m_Passed = false;
	}

	GameInf::~GameInf()
	{

	}
	bool GameInf::Initialise()
	{
		NodeManager* nm = NodeManager::GetSingletonPtr();
		SceneManager* sm = SceneManager::GetSingletonPtr();
		RenderSystem* rs = RenderSystem::GetSingletonPtr();
		AudioSystem* as = AudioSystem::GetSingletonPtr();
		InputSystem* is = InputSystem::GetSingletonPtr();

		//////////////////////////////////////////////////////////////////////////
		m_pRoot = new Sprite;
		m_pRoot->SetName(_T("��"));
		m_pRoot->SetActivity(false);
		sm->SetRoot(m_pRoot);

		//////////////////////////////////////////////////////////////////////////
		// ����
		m_pScene = new Sprite;
		m_pScene->SetName(_T("����"));
		m_pScene->SetActivity(false);
		m_pRoot->AttachChild(m_pScene);
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// ����
		m_pGUI = new Sprite;
		m_pGUI->SetName(_T("����"));
		m_pGUI->SetActivity(false);
		m_pRoot->AttachChild(m_pGUI);

		m_pSprRoleWnd = new Sprite;
		m_pSprRoleWnd->LoadSprite(_T("Data\\���ﴰ��.spr"));
		m_pSprRoleWnd->SetPosition(Point(0, 326));
		m_pSprRoleWnd->SubscribeEvent(MouseClickEvent, SubscriberSlot(&GameInf::OnRoleWndClick, this));
		m_pGUI->AttachChild(m_pSprRoleWnd);

		m_pTips = new GUITips(rs->FontMgr.Get(_T("����")));
		m_pTips->LoadSprite(_T("Data\\��ʾ.spr"));
		m_pTips->SetPosition(Point(205, 60));
		m_pGUI->AttachChild(m_pTips);

		m_pSprPass = new Sprite;
		m_pSprPass->LoadSprite(_T("Data\\ͨ��.spr"));
		m_pSprPass->SubscribeEvent(FrmEvent, SubscriberSlot(&GameInf::OnPassFrame, this));
		m_pGUI->AttachChild(m_pSprPass);

		Sprite* pSprBar = new Sprite;
		pSprBar->LoadSprite(_T("Data\\��Ʒ��.spr"));
		m_pGUI->AttachChild(pSprBar);

		m_pBlueTip = new Sprite;
		m_pRedTip = new Sprite;
		m_pBlackTip = new Sprite;
		m_pYellowTip = new Sprite;
		m_pPinkTip = new Sprite;
		m_pMailTip = new Sprite;
		m_pPassTip = new Sprite;

		Font* pFont = new Font;
		pFont->CreateFont(_T("Arial"), -18, 0, FW_NORMAL);
		rs->FontMgr.Add(pFont);
		// ��ˮ����ť
		GUILabel* pSprBlueKey = new GUILabel(pFont);
		pSprBlueKey->SetName(_T("����ǩ"));
		pSprBlueKey->SetActivity(false);
		pSprBlueKey->AddFrame(new SprFrame(Layer(NULL, Quad(), Quad(), 0x0, Area(0, 0, 55, 32)), 0.f));
		pSprBlueKey->SetPosition(Point(48, 7));
		pSprBlueKey->SubscribeEvent(MouseClickEvent, SubscriberSlot(&GameInf::OnBlueKey, this));
		pSprBlueKey->SubscribeEvent(GetMouseFocusEvent, SubscriberSlot(&GameInf::OnBlueKeyGet, this));
		pSprBlueKey->SubscribeEvent(LostMouseFocusEvent, SubscriberSlot(&GameInf::OnBlueKeyLost, this));
		pSprBar->AttachChild(pSprBlueKey);
		m_pBlueTip->LoadSprite(_T("Data\\����ʾ.spr"));
		pSprBlueKey->AttachChild(m_pBlueTip);

		// ��ˮ����ť
		GUILabel* pSprRedKey = new GUILabel(pFont);
		pSprRedKey->SetName(_T("���ǩ"));
		pSprRedKey->SetActivity(false);
		pSprRedKey->AddFrame(new SprFrame(Layer(NULL, Quad(), Quad(), 0x0, Area(0, 0, 55, 32)), 0.f));
		pSprRedKey->SetPosition(Point(104, 7));
		pSprRedKey->SubscribeEvent(MouseClickEvent, SubscriberSlot(&GameInf::OnRedKey, this));
		pSprRedKey->SubscribeEvent(GetMouseFocusEvent, SubscriberSlot(&GameInf::OnRedKeyGet, this));
		pSprRedKey->SubscribeEvent(LostMouseFocusEvent, SubscriberSlot(&GameInf::OnRedKeyLost, this));
		pSprBar->AttachChild(pSprRedKey);
		m_pRedTip->LoadSprite(_T("Data\\����ʾ.spr"));
		pSprRedKey->AttachChild(m_pRedTip);

		// ��ˮ����ť
		GUILabel* pSprBlackKey = new GUILabel(pFont);
		pSprBlackKey->SetName(_T("�ڱ�ǩ"));
		pSprBlackKey->SetActivity(false);
		pSprBlackKey->AddFrame(new SprFrame(Layer(NULL, Quad(), Quad(), 0x0, Area(0, 0, 55, 32)), 0.f));
		pSprBlackKey->SetPosition(Point(160, 7));
		pSprBlackKey->SubscribeEvent(MouseClickEvent, SubscriberSlot(&GameInf::OnBlackKey, this));
		pSprBlackKey->SubscribeEvent(GetMouseFocusEvent, SubscriberSlot(&GameInf::OnBlackKeyGet, this));
		pSprBlackKey->SubscribeEvent(LostMouseFocusEvent, SubscriberSlot(&GameInf::OnBlackKeyLost, this));
		pSprBar->AttachChild(pSprBlackKey);
		m_pBlackTip->LoadSprite(_T("Data\\����ʾ.spr"));
		pSprBlackKey->AttachChild(m_pBlackTip);

		// ��ˮ����ť
		GUILabel* pSprYellowKey = new GUILabel(pFont);
		pSprYellowKey->SetName(_T("�Ʊ�ǩ"));
		pSprYellowKey->SetActivity(false);
		pSprYellowKey->AddFrame(new SprFrame(Layer(NULL, Quad(), Quad(), 0x0, Area(0, 0, 55, 32)), 0.f));
		pSprYellowKey->SetPosition(Point(216, 7));
		pSprYellowKey->SubscribeEvent(MouseClickEvent, SubscriberSlot(&GameInf::OnYellowKey, this));
		pSprYellowKey->SubscribeEvent(GetMouseFocusEvent, SubscriberSlot(&GameInf::OnYellowKeyGet, this));
		pSprYellowKey->SubscribeEvent(LostMouseFocusEvent, SubscriberSlot(&GameInf::OnYellowKeyLost, this));
		pSprBar->AttachChild(pSprYellowKey);
		m_pYellowTip->LoadSprite(_T("Data\\����ʾ.spr"));
		pSprYellowKey->AttachChild(m_pYellowTip);

		// ��ˮ����ť
		GUILabel* pSprPinkKey = new GUILabel(pFont);
		pSprPinkKey->SetName(_T("�۱�ǩ"));
		pSprPinkKey->SetActivity(false);
		pSprPinkKey->AddFrame(new SprFrame(Layer(NULL, Quad(), Quad(), 0x0, Area(0, 0, 55, 32)), 0.f));
		pSprPinkKey->SetPosition(Point(272, 7));
		pSprPinkKey->SubscribeEvent(MouseClickEvent, SubscriberSlot(&GameInf::OnPinkKey, this));
		pSprPinkKey->SubscribeEvent(GetMouseFocusEvent, SubscriberSlot(&GameInf::OnPinkKeyGet, this));
		pSprPinkKey->SubscribeEvent(LostMouseFocusEvent, SubscriberSlot(&GameInf::OnPinkKeyLost, this));
		pSprBar->AttachChild(pSprPinkKey);
		m_pPinkTip->LoadSprite(_T("Data\\����ʾ.spr"));
		pSprPinkKey->AttachChild(m_pPinkTip);

		// ��������ť
		GUILabel* pSprMail = new GUILabel(pFont);
		pSprMail->SetName(_T("���ͱ�ǩ"));
		pSprMail->SetActivity(false);
		pSprMail->AddFrame(new SprFrame(Layer(NULL, Quad(), Quad(), 0x0, Area(0, 0, 55, 32)), 0.f));
		pSprMail->SetPosition(Point(328, 7));
		pSprMail->SubscribeEvent(MouseClickEvent, SubscriberSlot(&GameInf::OnMail, this));
		pSprMail->SubscribeEvent(GetMouseFocusEvent, SubscriberSlot(&GameInf::OnMailGet, this));
		pSprMail->SubscribeEvent(LostMouseFocusEvent, SubscriberSlot(&GameInf::OnMailLost, this));
		pSprBar->AttachChild(pSprMail);
		m_pMailTip->LoadSprite(_T("Data\\��������ʾ.spr"));
		pSprMail->AttachChild(m_pMailTip);

		// ͨ��֤��ť
		GUILabel* pSprPass = new GUILabel(pFont);
		pSprPass->SetName(_T("ͨ�б�ǩ"));
		pSprPass->SetActivity(false);
		pSprPass->AddFrame(new SprFrame(Layer(NULL, Quad(), Quad(), 0x0, Area(0, 0, 55, 32)), 0.f));
		pSprPass->SetPosition(Point(384, 7));
		pSprPass->SubscribeEvent(MouseClickEvent, SubscriberSlot(&GameInf::OnPass, this));
		pSprPass->SubscribeEvent(GetMouseFocusEvent, SubscriberSlot(&GameInf::OnPassGet, this));
		pSprPass->SubscribeEvent(LostMouseFocusEvent, SubscriberSlot(&GameInf::OnPassLost, this));
		pSprBar->AttachChild(pSprPass);
		m_pPassTip->LoadSprite(_T("Data\\ͨ��֤��ʾ.spr"));
		pSprPass->AttachChild(m_pPassTip);

		GUILabel* pLBTotal = new GUILabel(pFont);
		pLBTotal->m_dwFormat = DT_LEFT;
		pLBTotal->m_Color = 0xFF8000FF;
		pLBTotal->m_rcDest = Area(0, 0, 50, 20);
		pLBTotal->SetName(_T("����"));
		pLBTotal->SetActivity(false);
		pLBTotal->SetPosition(Point(81, -50));
		pSprBar->AttachChild(pLBTotal);

		GUILabel* pLBCount = new GUILabel(pFont);
		pLBCount->m_dwFormat = DT_LEFT;
		pLBCount->m_Color = 0xFF8000FF;
		pLBCount->m_rcDest = Area(0, 0, 50, 20);
		pLBCount->SetName(_T("����"));
		pLBCount->SetActivity(false);
		pLBCount->SetPosition(Point(81, -28));
		pSprBar->AttachChild(pLBCount);
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// �ö�
		m_pTop = new Sprite;
		m_pTop->SetName(_T("�ö�"));
		m_pTop->SetActivity(false);
		m_pRoot->AttachChild(m_pTop);

		m_pCursor = new GUICursor;
		m_pCursor->LoadSprite(_T("Data\\���.spr"));
		m_pTop->AttachChild(m_pCursor);

		m_pMask = new Sprite;
		m_pMask->LoadSprite(_T("Data\\����.spr"));
		m_pMask->SubscribeEvent(FrmEvent, SubscriberSlot(&GameInf::OnMask, this));
		m_pTop->AttachChild(m_pMask);

		//////////////////////////////////////////////////////////////////////////
		is->SubscribeEvent(KeyEvent, SubscriberSlot(&GameInf::OnChangeCtrlRole, this));

		MapBase* mbEmpty = new MapBase;
		mbEmpty->SetName(_T("��"));
		nm->Add(mbEmpty);

		MapBase* mbWall = new MapBase;
		mbWall->m_Type = MB_WALL;
		mbWall->SetName(_T("ǽ"));
		nm->Add(mbWall);

		MapBase* mbWater = new MapBase;
		mbWater->m_Type = MB_WATER;
		mbWater->SetName(_T("ˮ"));
		nm->Add(mbWater);


		MakeTestMap(this);

		ChangeCtrlRole();



		return true;
	}

	bool GameInf::Cleanup()
	{
		NodeManager* nm = NodeManager::GetSingletonPtr();
		SceneManager* sm = SceneManager::GetSingletonPtr();
		RenderSystem* rs = RenderSystem::GetSingletonPtr();
		AudioSystem* as = AudioSystem::GetSingletonPtr();
		InputSystem* is = InputSystem::GetSingletonPtr();

		//
		is->RemoveEvent(KeyEvent, SubscriberSlot(&GameInf::OnChangeCtrlRole, this));

		sm->GetRoot()->KillDescendants();
		sm->Clear();
		nm->RemoveAll();
		as->RenderingList.RemoveAll();
		as->MP3Buffers.RemoveAll();
		return true;
	}

	void GameInf::MainLoop(float deltaTime)
	{
		AudioSystem* as = AudioSystem::GetSingletonPtr();
		m_pCursor->Update();
		m_pTips->Update(deltaTime);

		if(!as->RenderingList.Get(_T("��ԭ"))->IsPlaying())
			as->RenderingList.Get(_T("��ԭ"))->Play();

		if(m_pCtrlRole)
		{
			Point tmpPoint = m_pCtrlRole->GetPosition();
			//tmpPoint = Point(320, 240) - tmpPoint;
			tmpPoint.x -= 320;
			tmpPoint.y -= 240;
			m_FPoint.SetTargetPoint(tmpPoint);
			m_FPoint.Update(deltaTime);

			tmpPoint = m_FPoint.GetCurrentPoint();
			tmpPoint.x = (int)-tmpPoint.x;
			tmpPoint.y = (int)-tmpPoint.y;
			m_pScene->SetPosition(tmpPoint);

			if(!m_Passed)
			{
				InputSystem* is = InputSystem::GetSingletonPtr();
				if(is->GetVKState(VK_UP))
				{
					m_pCtrlRole->RoleMove(DIR_UP);
				}
				else if(is->GetVKState(VK_DOWN))
				{
					m_pCtrlRole->RoleMove(DIR_DOWN);
				}
				else if(is->GetVKState(VK_LEFT))
				{
					m_pCtrlRole->RoleMove(DIR_LEFT);
				}
				else if(is->GetVKState(VK_RIGHT))
				{
					m_pCtrlRole->RoleMove(DIR_RIGHT);
				}
			}
		}
	}

	void GameInf::ChangeCtrlRole()
	{
		if(!m_Passed)
		{
			if(m_Map.m_Hero->m_Passed && m_Map.m_Heroine->m_Passed)
			{
				//MessageBox(0, _T("�ɹ�ͨ��"), 0, 0);
				m_pSprPass->Play();
				AudioSystem* as = AudioSystem::GetSingletonPtr();
				as->RenderingList.Get(_T("��ԭ"))->FadeOut(2);
				as->Play(_T("ͨ��"));
				m_Passed = true;
			}
			if(!m_pCtrlRole) m_pCtrlRole = m_Map.m_Hero;
			else
			{
				if(m_pCtrlRole == m_Map.m_Hero)
				{
					if(!m_Map.m_Heroine->m_Passed)
					{
						m_pCtrlRole = m_Map.m_Heroine;
						m_pSprRoleWnd->Goto(1);
					}
				}
				else
				{
					if(!m_Map.m_Hero->m_Passed)
					{
						m_pCtrlRole = m_Map.m_Hero;
						m_pSprRoleWnd->Goto(0);
					}
				}
			}
			UpdateThings();
		}
	}

	void GameInf::UpdateThings()
	{
		GUILabel* plb;
		Role* role = m_pCtrlRole;
		if(!role) return;

		plb = (GUILabel*)m_pGUI->FindDescendant(_T("����ǩ"));
		plb->m_strText.Format(_T("x %d"), role->m_BlueKey);

		plb = (GUILabel*)m_pGUI->FindDescendant(_T("���ǩ"));
		plb->m_strText.Format(_T("x %d"), role->m_RedKey);

		plb = (GUILabel*)m_pGUI->FindDescendant(_T("�ڱ�ǩ"));
		plb->m_strText.Format(_T("x %d"), role->m_BlackKey);

		plb = (GUILabel*)m_pGUI->FindDescendant(_T("�Ʊ�ǩ"));
		plb->m_strText.Format(_T("x %d"), role->m_YellowKey);

		plb = (GUILabel*)m_pGUI->FindDescendant(_T("�۱�ǩ"));
		plb->m_strText.Format(_T("x %d"), role->m_PinkKey);

		plb = (GUILabel*)m_pGUI->FindDescendant(_T("���ͱ�ǩ"));
		plb->m_strText.Format(_T("x %d"), role->m_Mail);

		plb = (GUILabel*)m_pGUI->FindDescendant(_T("ͨ�б�ǩ"));
		plb->m_strText.Format(_T("%s"), role->m_bPass ? _T("��") : _T("�w"));

		plb = (GUILabel*)m_pGUI->FindDescendant(_T("����"));
		plb->m_strText.Format(_T("%d"), m_Map.m_TotalGems);

		plb = (GUILabel*)m_pGUI->FindDescendant(_T("����"));
		plb->m_strText.Format(_T("%d"), m_Map.m_Hero->m_Gem + m_Map.m_Heroine->m_Gem);
	}

	//////////////////////////////////////////////////////////////////////////

	bool GameInf::OnChangeCtrlRole(const EventArgs& args)
	{
		KeyEventArgs& ka = (KeyEventArgs&)args;
		if(ka.m_bState && ka.m_VirtualKey == 'H') ChangeCtrlRole();
		return true;
	}

	bool GameInf::OnRoleWndClick(const EventArgs& args)
	{
		ChangeCtrlRole();
		return true;
	}

	bool GameInf::OnPassFrame(const EventArgs& args)
	{
		FrameEventArgs& fa = (FrameEventArgs&)args;
		if(fa.m_pAnimation->GetCurrentFrmPos() == 5)
		{
			m_pMask->Play();
		}
		return true;
	}


	bool GameInf::OnWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	bool GameInf::OnMask(const EventArgs& args)
	{
		FrameEventArgs& fa = (FrameEventArgs&)args;
		dword cfp = fa.m_pAnimation->GetCurrentFrmPos();

		if(cfp == 102)
		{
			SampleApp* pApp = (SampleApp*)SampleApp::GetSingletonPtr();
			pApp->SetNextInterface(new MainInf);
			pApp->SetUpdateFlag(true);
		}
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	bool GameInf::OnBlueKey(const EventArgs& args)
	{
		m_pCtrlRole->Discard(MB_BLUE_KEY);
		return true;
	}

	bool GameInf::OnBlueKeyGet(const EventArgs& args)
	{
		m_pBlueTip->SetNextFrmPos(1);
		m_pBlueTip->Play();
		return true;
	}

	bool GameInf::OnBlueKeyLost(const EventArgs& args)
	{
		m_pBlueTip->SetNextFrmPos(7);
		m_pBlueTip->Play();
		return true;
	}

	bool GameInf::OnRedKey(const EventArgs& args)
	{
		m_pCtrlRole->Discard(MB_RED_KEY);
		return true;
	}

	bool GameInf::OnRedKeyGet(const EventArgs& args)
	{
		m_pRedTip->SetNextFrmPos(1);
		m_pRedTip->Play();
		return true;
	}

	bool GameInf::OnRedKeyLost(const EventArgs& args)
	{
		m_pRedTip->SetNextFrmPos(7);
		m_pRedTip->Play();
		return true;
	}

	bool GameInf::OnBlackKey(const EventArgs& args)
	{
		m_pCtrlRole->Discard(MB_BLACK_KEY);
		return true;
	}

	bool GameInf::OnBlackKeyGet(const EventArgs& args)
	{
		m_pBlackTip->SetNextFrmPos(1);
		m_pBlackTip->Play();
		return true;
	}

	bool GameInf::OnBlackKeyLost(const EventArgs& args)
	{
		m_pBlackTip->SetNextFrmPos(7);
		m_pBlackTip->Play();
		return true;
	}

	bool GameInf::OnYellowKey(const EventArgs& args)
	{
		m_pCtrlRole->Discard(MB_YELLOW_KEY);
		return true;
	}

	bool GameInf::OnYellowKeyGet(const EventArgs& args)
	{
		m_pYellowTip->SetNextFrmPos(1);
		m_pYellowTip->Play();
		return true;
	}

	bool GameInf::OnYellowKeyLost(const EventArgs& args)
	{
		m_pYellowTip->SetNextFrmPos(7);
		m_pYellowTip->Play();
		return true;
	}

	bool GameInf::OnPinkKey(const EventArgs& args)
	{
		m_pCtrlRole->Discard(MB_PINK_KEY);
		return true;
	}

	bool GameInf::OnPinkKeyGet(const EventArgs& args)
	{
		m_pPinkTip->SetNextFrmPos(1);
		m_pPinkTip->Play();
		return true;
	}

	bool GameInf::OnPinkKeyLost(const EventArgs& args)
	{
		m_pPinkTip->SetNextFrmPos(7);
		m_pPinkTip->Play();
		return true;
	}

	bool GameInf::OnMail(const EventArgs& args)
	{
		m_pCtrlRole->Discard(MB_MAIL);
		return true;
	}

	bool GameInf::OnMailGet(const EventArgs& args)
	{
		m_pMailTip->SetNextFrmPos(1);
		m_pMailTip->Play();
		return true;
	}

	bool GameInf::OnMailLost(const EventArgs& args)
	{
		m_pMailTip->SetNextFrmPos(7);
		m_pMailTip->Play();
		return true;
	}

	bool GameInf::OnPass(const EventArgs& args)
	{
		m_pCtrlRole->Discard(MB_PASS);
		return true;
	}

	bool GameInf::OnPassGet(const EventArgs& args)
	{
		m_pPassTip->SetNextFrmPos(1);
		m_pPassTip->Play();
		return true;
	}

	bool GameInf::OnPassLost(const EventArgs& args)
	{
		m_pPassTip->SetNextFrmPos(7);
		m_pPassTip->Play();
		return true;
	}
}
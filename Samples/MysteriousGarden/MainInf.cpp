/************************************************************************/
// 创建时间: 081124
// 最后确定: 081125

#include "MainInf.h"

namespace AnyPlay
{
	MainInf::MainInf()
	{
		m_pRoot = NULL;
		m_pScene = NULL;
		m_pGUI = NULL;
		m_pTop = NULL;
		m_pMask = NULL;
		m_pSelWnd = NULL;

		m_pCursor = NULL;
	}

	MainInf::~MainInf()
	{
	}

	//////////////////////////////////////////////////////////////////////////
	bool MainInf::Initialise()
	{
		NodeManager* nm = NodeManager::GetSingletonPtr();
		SceneManager* sm = SceneManager::GetSingletonPtr();
		RenderSystem* rs = RenderSystem::GetSingletonPtr();
		AudioSystem* as = AudioSystem::GetSingletonPtr();

		//////////////////////////////////////////////////////////////////////////

		as->MP3Buffers.Add(new Buffer(_T("主题"), _T("Data\\主题.MP3")));
		as->Play(_T("主题"));
		//as->SetVolume(0, 0);

		//////////////////////////////////////////////////////////////////////////
		m_pRoot = new Sprite;
		m_pRoot->SetName(_T("根"));
		m_pRoot->SetActivity(false);
		sm->SetRoot(m_pRoot);

		//////////////////////////////////////////////////////////////////////////
		// 场景
		m_pScene = new Sprite;
		m_pScene->SetName(_T("场景"));
		m_pScene->SetActivity(false);
		m_pRoot->AttachChild(m_pScene);

		Sprite* pSprBack = new Sprite;
		pSprBack->LoadSprite(_T("Data\\登陆背景.spr"));
		m_pScene->AttachChild(pSprBack);

		Sprite* pSprFloat = new Sprite;
		pSprFloat->LoadSprite(_T("Data\\浮动精灵.spr"));
		pSprFloat->SetSmoothMode(false);
		pSprBack->AttachChild(pSprFloat);

		Sprite* pSprLogo = new Sprite;
		pSprLogo->LoadSprite(_T("Data\\游戏Logo.spr"));
		pSprBack->AttachChild(pSprLogo);
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// 界面
		m_pGUI = new Sprite;
		m_pGUI->SetName(_T("界面"));
		m_pGUI->SetActivity(false);
		m_pRoot->AttachChild(m_pGUI);

		GUIButton* pSprStart = new GUIButton;
		pSprStart->LoadSprite(_T("Data\\开始游戏.spr"));
		pSprStart->SubscribeEvent(MouseClickEvent, SubscriberSlot(&MainInf::OnStartClick, this));
		m_pGUI->AttachChild(pSprStart);

		GUIButton* pSprAbout = new GUIButton;
		pSprAbout->LoadSprite(_T("Data\\游戏说明.spr"));
		pSprAbout->SubscribeEvent(MouseClickEvent, SubscriberSlot(&MainInf::OnAboutClick, this));
		m_pGUI->AttachChild(pSprAbout);

		GUIButton* pSprReadme = new GUIButton;
		pSprReadme->LoadSprite(_T("Data\\关于作者.spr"));
		pSprReadme->SubscribeEvent(MouseClickEvent, SubscriberSlot(&MainInf::OnReadmeClick, this));
		m_pGUI->AttachChild(pSprReadme);

		GUIButton* pSprExit = new GUIButton;
		pSprExit->LoadSprite(_T("Data\\结束游戏.spr"));
		pSprExit->SubscribeEvent(MouseClickEvent, SubscriberSlot(&MainInf::OnExitClick, this));
		m_pGUI->AttachChild(pSprExit);

		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// 置顶
		m_pTop = new Sprite;
		m_pTop->SetName(_T("置顶"));
		m_pTop->SetActivity(false);
		m_pRoot->AttachChild(m_pTop);

		m_pCursor = new GUICursor;
		m_pCursor->LoadSprite(_T("Data\\光标.spr"));
		m_pTop->AttachChild(m_pCursor);

		m_pMask = new Sprite;
		m_pMask->LoadSprite(_T("Data\\遮罩.spr"));
		m_pMask->SubscribeEvent(FrmEvent, SubscriberSlot(&MainInf::OnMask, this));
		m_pTop->AttachChild(m_pMask);


		m_pSelWnd = MakeWindow(_T("选择关卡"), _T("神秘乐章"), 200, 300, SubscriberSlot(&MainInf::OnSelCloseClick, this));
		m_pSelWnd->SetPosition(Point(220, 90));
		m_pSelWnd->SetVisible(false);
		m_pGUI->AttachChild(m_pSelWnd);

		GUIButton* pBtnDongyuan = new GUIButton;
		pBtnDongyuan->LoadSprite(_T("Data\\冻原按钮.spr"));
		pBtnDongyuan->SetSmoothMode(false);
		pBtnDongyuan->SubscribeEvent(MouseClickEvent, SubscriberSlot(&MainInf::OnDongyuanClick, this));
		m_pSelWnd->AttachChild(pBtnDongyuan);

		return true;
	}

	bool MainInf::Cleanup()
	{
		NodeManager* nm = NodeManager::GetSingletonPtr();
		SceneManager* sm = SceneManager::GetSingletonPtr();
		RenderSystem* rs = RenderSystem::GetSingletonPtr();
		AudioSystem* as = AudioSystem::GetSingletonPtr();

		sm->GetRoot()->KillDescendants();
		sm->Clear();
		as->RenderingList.RemoveAll();
		as->MP3Buffers.RemoveAll();
		return true;
	}

	void MainInf::MainLoop(float deltaTime)
	{
		AudioSystem* as = AudioSystem::GetSingletonPtr();
		m_pCursor->Update();

		if(!as->RenderingList.Get(_T("主题"))->IsPlaying())
			as->RenderingList.Get(_T("主题"))->Play();

	}

	bool MainInf::OnWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	bool MainInf::OnMask(const EventArgs& args)
	{
		FrameEventArgs& fa = (FrameEventArgs&)args;
		dword cfp = fa.m_pAnimation->GetCurrentFrmPos();

		if(cfp == 102)
		{
			SampleApp* pApp = (SampleApp*)SampleApp::GetSingletonPtr();
			pApp->SetNextInterface(new GameInf());
			pApp->SetUpdateFlag(true);
		}
		return true;
	}

	bool MainInf::OnStartClick(const EventArgs& args)
	{
		//
		m_pSelWnd->SetVisible(true);

		return true;
	}

	bool MainInf::OnSelCloseClick(const EventArgs& args)
	{
		m_pSelWnd->SetVisible(false);
		return true;
	}

	bool MainInf::OnAboutClick(const EventArgs& args)
	{
		return true;
	}

	bool MainInf::OnReadmeClick(const EventArgs& args)
	{
		return true;
	}

	bool MainInf::OnExitClick(const EventArgs& args)
	{
		QUIT_PROGRAM;
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	bool MainInf::OnDongyuanClick(const EventArgs& args)
	{
		m_pMask->Play();
		return true;
	}
}
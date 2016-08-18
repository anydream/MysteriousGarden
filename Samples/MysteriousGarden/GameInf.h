/************************************************************************/
// 创建时间: 081126
// 最后确定: 081126

#ifndef _GameInf_h_
#define _GameInf_h_

#include "../../AnyPlay.h"
#include "../../FrameWork/Interface.h"
#include "../../Utils.h"
#include "../../RenderSystem/RenderSystem.h"
#include "../../AudioSystem/AudioSystem.h"
#include "../../InputSystem/InputSystem.h"
#include "../../FileSystem/FileSystem.h"
#include "../../FrameWork/NodeManager.h"
#include "../../FrameWork/Singleton.h"
#include "../../Sprites/Sprite.h"
#include "../../Sprites/GUI.h"
#include "../../FrameWork/SceneManager.h"
#include "../../Core/Flexible.h"
#include "Game.h"

namespace AnyPlay
{
	class GameInf : public Interface, public Singleton<GameInf>
	{
	public:
		GameInf();
		~GameInf();
		bool		Initialise();
		bool		Cleanup();
		void		MainLoop(float deltaTime);
		bool		OnWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		//////////////////////////////////////////////////////////////////////////
		void		ChangeCtrlRole();
		void		UpdateThings();

		bool		OnChangeCtrlRole(const EventArgs& args);
		bool		OnRoleWndClick(const EventArgs& args);
		bool		OnMask(const EventArgs& args);
		bool		OnPassFrame(const EventArgs& args);

		//////////////////////////////////////////////////////////////////////////
		bool		OnBlueKey(const EventArgs& args);
		bool		OnBlueKeyGet(const EventArgs& args);
		bool		OnBlueKeyLost(const EventArgs& args);

		bool		OnRedKey(const EventArgs& args);
		bool		OnRedKeyGet(const EventArgs& args);
		bool		OnRedKeyLost(const EventArgs& args);

		bool		OnBlackKey(const EventArgs& args);
		bool		OnBlackKeyGet(const EventArgs& args);
		bool		OnBlackKeyLost(const EventArgs& args);

		bool		OnYellowKey(const EventArgs& args);
		bool		OnYellowKeyGet(const EventArgs& args);
		bool		OnYellowKeyLost(const EventArgs& args);

		bool		OnPinkKey(const EventArgs& args);
		bool		OnPinkKeyGet(const EventArgs& args);
		bool		OnPinkKeyLost(const EventArgs& args);

		bool		OnMail(const EventArgs& args);
		bool		OnMailGet(const EventArgs& args);
		bool		OnMailLost(const EventArgs& args);

		bool		OnPass(const EventArgs& args);
		bool		OnPassGet(const EventArgs& args);
		bool		OnPassLost(const EventArgs& args);

		Sprite*		m_pBlueTip;
		Sprite*		m_pRedTip;
		Sprite*		m_pBlackTip;
		Sprite*		m_pYellowTip;
		Sprite*		m_pPinkTip;
		Sprite*		m_pMailTip;
		Sprite*		m_pPassTip;
	public:
		Sprite*		m_pRoot;
		Sprite*		m_pScene;
		Sprite*		m_pGUI;
		Sprite*		m_pTop;
		Sprite*		m_pMask;
		Sprite*		m_pSprRoleWnd;
		Sprite*		m_pSprPass;

		Role*		m_pCtrlRole;

		GUICursor*	m_pCursor;
		GUITips*	m_pTips;

		GameMap		m_Map;
		FlexiblePoint	m_FPoint;

		bool		m_Passed;

	};
	void MakeTestMap(GameInf* gi);
}

#endif
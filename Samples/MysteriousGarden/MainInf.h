/************************************************************************/
// 创建时间: 081124
// 最后确定: 081125

#ifndef _MainInf_h_
#define _MainInf_h_

#include "../../AnyPlay.h"
#include "../../FrameWork/Interface.h"
#include "../../Utils.h"
#include "../../RenderSystem/RenderSystem.h"
#include "../../AudioSystem/AudioSystem.h"
#include "../../InputSystem/InputSystem.h"
#include "../../FileSystem/FileSystem.h"
#include "../../FrameWork/NodeManager.h"
#include "../../Sprites/Sprite.h"
#include "../../Sprites/GUI.h"
#include "../../FrameWork/SceneManager.h"
#include "../SampleApp.h"
#include "GameInf.h"

namespace AnyPlay
{
	class MainInf : public Interface
	{
	public:
		MainInf();
		~MainInf();
		bool		Initialise();
		bool		Cleanup();
		void		MainLoop(float deltaTime);
		bool		OnWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		bool		OnMask(const EventArgs& args);

		bool		OnStartClick(const EventArgs& args);
		bool		OnSelCloseClick(const EventArgs& args);

		bool		OnAboutClick(const EventArgs& args);
		bool		OnReadmeClick(const EventArgs& args);
		bool		OnExitClick(const EventArgs& args);

		bool		OnDongyuanClick(const EventArgs& args);

	public:
		Sprite*		m_pRoot;
		Sprite*		m_pScene;
		Sprite*		m_pGUI;
		Sprite*		m_pTop;
		Sprite*		m_pMask;
		GUIWindow*	m_pSelWnd;

		GUICursor*	m_pCursor;
	};
}
#endif
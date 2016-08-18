/************************************************************************/
// 创建时间: 081118
// 最后确定: 081123

#ifndef _MyInf_h_
#define _MyInf_h_

#include "../../AnyPlay.h"
#include "../../FrameWork/Interface.h"
#include "../../Utils.h"
#include "../../RenderSystem/RenderSystem.h"
#include "../../AudioSystem/AudioSystem.h"
#include "../../InputSystem/InputSystem.h"
#include "../../FileSystem/FileSystem.h"
#include "../../FrameWork/NodeManager.h"
#include "../../Sprites/Sprite.h"
#include "../../FrameWork/SceneManager.h"

namespace AnyPlay
{
	class MyInf : public Interface
	{
	public:
		virtual bool Initialise()
		{
			NodeManager* nm = NodeManager::GetSingletonPtr();
			SceneManager* sm = SceneManager::GetSingletonPtr();
			RenderSystem* rs = RenderSystem::GetSingletonPtr();
			AudioSystem* as = AudioSystem::GetSingletonPtr();
			/*Texture* tex = new Texture;
			tex->SetName(_T("未命名.tex"));
			tex->LoadFromFile(_T("未命名.bmp"), NULL, APTEX_HIRES);
			rs->TextureMgr.Add(tex);*/

			//////////////////////////////////////////////////////////////////////////
			
			Sprite* spr = new Sprite;
			/*spr->SetName(_T("Data\\登陆背景.spr"));
			spr->AddFrame(new SprFrame(Layer(tex, Area(533, 32, 50, 39), Area(0, 0, 50, 39)), 1000.f));
			spr->AddFrame(new SprFrame(Layer(tex, Area(605, 32, 50, 39), Area(0, 0, 50, 39)), 1000.f));
			spr->AddFrame(new SprFrame(Layer(tex, Area(670, 32, 50, 39), Area(0, 0, 50, 39)), 1000.f));
			spr->SaveSprite(_T("Data\\登陆背景.spr"));*/
			spr->LoadSprite(_T("Data\\登陆背景.spr"));
			//spr->SetPosition(Point(200, 200));
			nm->Add(spr);
			sm->SetRoot(new Sprite(*spr));

// 			as->MP3Buffers.Add(new Buffer(_T("音乐1"), _T("E:\\游戏现成资源\\神奇的西游世界.mp3")));
// 			as->MP3Buffers.Add(new Buffer(_T("音乐2"), _T("升级.mp3")));
// 
// 			as->Play(_T("音乐2"));
// 			Sleep(5000);
// 			as->Play(_T("音乐1"));

			return true;
		}
		virtual bool Cleanup()
		{
			return true;
		}
		virtual void MainLoop(float deltaTime)
		{
		}
		virtual bool OnWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			return true;
		}
	};
}
#endif
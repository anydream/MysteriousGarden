
#include "Game.h"
#include "GameInf.h"

namespace AnyPlay
{
	MapBase* AddWall(GameInf* gi, const String& name, dword X, dword Y, LPCTSTR gotFrm)
	{
		MapBase* mb = new MapBase;
		mb->m_Type = MB_WALL;

		mb->LoadSprite(name);
		if(gotFrm) mb->Goto(gotFrm);

		gi->m_pScene->AttachChild(mb);
		gi->m_Map.SetMapData(X, Y, mb);
		mb->UpdatePos();
		return mb;
	}

	MapBase* AddPortal(GameInf* gi, LPCTSTR name, dword X, dword Y, dword tarX, dword tarY, ushort idx)
	{
		MapBase* mb = new MapBase;
		mb->m_Type = MB_PORTAL;
		if(name) mb->SetName(name);

		if(idx == 1) mb->LoadSprite(_T("Data\\������1.spr"));
		else if(idx == 2) mb->LoadSprite(_T("Data\\������2.spr"));
		else if(idx == 3) mb->LoadSprite(_T("Data\\������3.spr"));
		else mb->LoadSprite(_T("Data\\������4.spr"));

		mb->m_TarX = tarX;
		mb->m_TarY = tarY;

		gi->m_pScene->AttachChild(mb);
		gi->m_Map.SetMapData(X, Y, mb);
		mb->UpdatePos();
		return mb;
	}

	MapBase* AddTrap(GameInf* gi, LPCTSTR name, dword X, dword Y, ushort idx)
	{
		MapBase* mb = new MapBase;
		mb->m_Type = MB_TRAP;
		if(name) mb->SetName(name);

		if(idx == 1) mb->LoadSprite(_T("Data\\����1.spr"));
		else if(idx == 2) mb->LoadSprite(_T("Data\\����2.spr"));
		else if(idx == 3) mb->LoadSprite(_T("Data\\����3.spr"));
		else mb->LoadSprite(_T("Data\\����4.spr"));
		mb->Goto(3);

		gi->m_pScene->AttachChild(mb);
		gi->m_Map.SetMapData(X, Y, mb);
		mb->UpdatePos();
		return mb;
	}

	MapBase* AddWater(GameInf* gi, LPCTSTR name, dword X, dword Y)
	{
		MapBase* mb = new MapBase;
		mb->m_Type = MB_WATER;
		if(name) mb->SetName(name);
		gi->m_pScene->AttachChild(mb);
		gi->m_Map.SetMapData(X, Y, mb);
		return mb;
	}

	MapBase* AddRock(GameInf* gi, LPCTSTR name, dword X, dword Y)
	{
		MapBase* mb = new MapBase;
		mb->m_Type = MB_ROCK;
		if(name) mb->SetName(name);

		mb->LoadSprite(_T("Data\\��ʯ.spr"));
		gi->m_pScene->AttachChild(mb);
		gi->m_Map.SetMapData(X, Y, mb);
		mb->UpdatePos();
		return mb;
	}


	MapBase* AddRockTar(GameInf* gi, LPCTSTR name, dword X, dword Y)
	{
		MapBase* mb = new MapBase;
		mb->m_Type = MB_ROCK_TAR;
		if(name) mb->SetName(name);

		mb->LoadSprite(_T("Data\\����.spr"));
		gi->m_pScene->AttachChild(mb);
		gi->m_Map.SetMapData(X, Y, mb);
		mb->UpdatePos();
		return mb;
	}

	MapBase* AddOW(GameInf* gi, LPCTSTR name, dword X, dword Y, DIRECT dir, bool ch)
	{
		MapBase* mb = new MapBase;
		if(name) mb->SetName(name);
		mb->LoadSprite(_T("Data\\������.spr"));
		if(ch)
		{
			if(dir == DIR_UP)
			{
				mb->m_Type = MB_UP_CH;
				mb->Goto(_T("����"));
			}
			else if(dir == DIR_DOWN)
			{
				mb->m_Type = MB_DOWN_CH;
				mb->Goto(_T("����"));
			}
			else if(dir == DIR_LEFT)
			{
				mb->m_Type = MB_LEFT_CH;
				mb->Goto(_T("����"));
			}
			else if(dir == DIR_RIGHT)
			{
				mb->m_Type = MB_RIGHT_CH;
				mb->Goto(_T("����"));
			}
		}
		else
		{
			if(dir == DIR_UP)
			{
				mb->m_Type = MB_UP_OW;
				mb->Goto(_T("��"));
			}
			else if(dir == DIR_DOWN)
			{
				mb->m_Type = MB_DOWN_OW;
				mb->Goto(_T("��"));
			}
			else if(dir == DIR_LEFT)
			{
				mb->m_Type = MB_LEFT_OW;
				mb->Goto(_T("��"));
			}
			else if(dir == DIR_RIGHT)
			{
				mb->m_Type = MB_RIGHT_OW;
				mb->Goto(_T("��"));
			}
		}
		gi->m_pScene->AttachChild(mb);
		gi->m_Map.SetMapData(X, Y, mb);
		mb->UpdatePos();

		return mb;
	}

	MapBase* AddThing(GameInf* gi, LPCTSTR name, dword X, dword Y, ushort type, LPCTSTR filename)
	{
		MapBase* mb = new MapBase;
		mb->m_Type = type;
		if(name) mb->SetName(name);

		if(filename) mb->LoadSprite(filename);
		else
		{
			if(type == MB_RED_KEY) mb->LoadSprite(_T("Data\\��ˮ��.spr"));
			else if(type == MB_BLUE_KEY) mb->LoadSprite(_T("Data\\��ˮ��.spr"));
			else if(type == MB_BLACK_KEY) mb->LoadSprite(_T("Data\\��ˮ��.spr"));
			else if(type == MB_YELLOW_KEY) mb->LoadSprite(_T("Data\\��ˮ��.spr"));
			else if(type == MB_PINK_KEY) mb->LoadSprite(_T("Data\\��ˮ��.spr"));
			else if(type == MB_MAIL) mb->LoadSprite(_T("Data\\������.spr"));
			else if(type == MB_GEM) mb->LoadSprite(_T("Data\\��ʯ.spr"));
			else if(type == MB_PASS) mb->LoadSprite(_T("Data\\ͨ��֤.spr"));
		}

		if(type == MB_GEM) gi->m_Map.m_TotalGems++;

		gi->m_pScene->AttachChild(mb);
		gi->m_Map.SetMapData(X, Y, mb);
		mb->UpdatePos();
		return mb;
	}

	MapBase* AddSwitch(GameInf* gi, LPCTSTR name, dword X, dword Y, ushort type, LPCTSTR filename, dword tarX, dword tarY)
	{
		MapBase* mb = new MapBase;
		mb->m_Type = type;
		if(name) mb->SetName(name);

		if(filename) mb->LoadSprite(filename);
		else
		{
			if(type == MB_RED_SWITCH) mb->LoadSprite(_T("Data\\��ˮ����.spr"));
			else if(type == MB_BLUE_SWITCH) mb->LoadSprite(_T("Data\\��ˮ����.spr"));
			else if(type == MB_BLACK_SWITCH) mb->LoadSprite(_T("Data\\��ˮ����.spr"));
			else if(type == MB_YELLOW_SWITCH) mb->LoadSprite(_T("Data\\��ˮ����.spr"));
			else if(type == MB_PINK_SWITCH) mb->LoadSprite(_T("Data\\��ˮ����.spr"));
			else if(type == MB_PASS_SWITCH) mb->LoadSprite(_T("Data\\ͨ��֤��.spr"));
			else if(type == MB_STEPON_SWITCH) mb->LoadSprite(_T("Data\\ľ����.spr"));
			else if(type == MB_STEPON_TAR) mb->LoadSprite(_T("Data\\��ʯ��.spr"));
			else if(type == MB_CH_SWITCH) mb->LoadSprite(_T("Data\\�л���.spr"));
			else if(type == MB_END) mb->LoadSprite(_T("Data\\�յ�.spr"));
			else if(type == MB_MALE_SWITCH) mb->LoadSprite(_T("Data\\����.spr"));
			else if(type == MB_FEMALE_SWITCH) mb->LoadSprite(_T("Data\\Ů��.spr"));
			else if(type == MB_WATER_SWITCH) mb->LoadSprite(_T("Data\\ˮ����.spr"));
		}

		if(type == MB_STEPON_TAR) mb->m_Lock = 1; 

		mb->m_TarX = tarX;
		mb->m_TarY = tarY;
		gi->m_pScene->AttachChild(mb);
		gi->m_Map.SetMapData(X, Y, mb);
		mb->UpdatePos();
		return mb;
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	void MakeTestMap(GameInf* gi)
	{
		NodeManager* nm = NodeManager::GetSingletonPtr();
		AudioSystem* as = AudioSystem::GetSingletonPtr();


		Role* pHero = new Role;
		pHero->m_X = 8;
		pHero->m_Y = 10;
		pHero->LoadSprite(_T("Data\\����.spr"));
		gi->m_pScene->AttachChild(pHero);

		Role* pHeroine = new Role;
		pHeroine->m_X = 10;
		pHeroine->m_Y = 10;
		pHeroine->LoadSprite(_T("Data\\ɺ��.spr"));
		gi->m_pScene->AttachChild(pHeroine);
		//////////////////////////////////////////////////////////////////////////
		gi->m_Map.CreateMap(60, 60);
		gi->m_Map.SetAll((MapBase*)nm->Get(_T("��")));
		gi->m_Map.SetHero(pHero);
		gi->m_Map.SetHeroine(pHeroine);

		gi->m_pScene->LoadSprite(_T("Data\\��ԭ��ͼ.spr"));

		gi->m_FPoint.m_Range = Area(-16, -12, 1280, 960);
		//////////////////////////////////////////////////////////////////////////

		gi->m_Map.SetWall(0, 0, 7, 16);
		gi->m_Map.SetWall(7, 0, 8, 8);
		gi->m_Map.SetWall(15, 0, 4, 6);
		gi->m_Map.SetWall(19, 0, 6, 4);
		gi->m_Map.SetWall(25, 0, 13, 3);
		gi->m_Map.SetWall(38, 0, 22, 4);
		gi->m_Map.SetWall(47, 4, 13, 8);
		gi->m_Map.SetWall(49, 12, 11, 11);
		gi->m_Map.SetWall(45, 23, 15, 4);
		gi->m_Map.SetWall(47, 27, 13, 10);
		gi->m_Map.SetWall(44, 34, 3, 3);
		gi->m_Map.SetWall(37, 34, 5, 3);
		gi->m_Map.SetWall(36, 31, 1, 6);
		gi->m_Map.SetWall(35, 27, 1, 4);
		gi->m_Map.SetWall(31, 26, 4, 3);
		gi->m_Map.SetWall(30, 15, 1, 14);
		gi->m_Map.SetWall(25, 14, 5, 3);
		gi->m_Map.SetWall(15, 13, 10, 3);
		gi->m_Map.SetWall(13, 8, 2, 10);
		gi->m_Map.SetWall(11, 18, 3, 2);
		gi->m_Map.SetWall(11, 20, 1, 11);
		gi->m_Map.SetWall(9, 29, 2, 2);
		gi->m_Map.SetWall(4, 29, 2, 2);
		gi->m_Map.SetWall(0, 16, 4, 15);
		/*gi->m_Map.SetWall(6, 28);
		gi->m_Map.SetWall(8, 28);*/

		gi->m_Map.SetWall(10, 31, 2, 9);
		gi->m_Map.SetWall(11, 35, 2, 7);
		gi->m_Map.SetWall(12, 37, 2, 11);
		gi->m_Map.SetWall(14, 40, 1, 2);
		gi->m_Map.SetWall(13, 44, 2, 9);
		gi->m_Map.SetWall(14, 45, 2, 8);
		gi->m_Map.SetWall(16, 49, 1, 4);
		gi->m_Map.SetWall(13, 55, 3, 3);
		gi->m_Map.SetWall(16, 55, 1, 1);
		gi->m_Map.SetWall(12, 57, 3, 2);
		gi->m_Map.SetWall(11, 59, 4, 1);
		//////////////////////////////////////////////////////////////////////////
		gi->m_Map.SetWater(12, 20, 8, 4);
		gi->m_Map.SetWater(13, 24, 2, 1);
		gi->m_Map.SetWater(14, 17, 10, 3);
		gi->m_Map.SetWater(14, 25, 1, 1);
		gi->m_Map.SetWater(15, 16, 4, 10);
		gi->m_Map.SetWater(19, 16, 2, 6);
		gi->m_Map.SetWater(21, 19, 6, 2);
		gi->m_Map.SetWater(22, 21, 1, 1);
		gi->m_Map.SetWater(23, 19, 4, 6);
		gi->m_Map.SetWater(27, 20, 2, 4);
		gi->m_Map.SetWater(24, 24, 4, 2);
		gi->m_Map.SetWater(25, 25, 4, 11);
		gi->m_Map.SetWater(29, 27, 1, 9);
		gi->m_Map.SetWater(24, 28, 1, 8);
		gi->m_Map.SetWater(23, 29, 8, 5);
		gi->m_Map.SetWater(31, 31, 1, 3);
		gi->m_Map.SetWater(26, 36, 3, 1);
		gi->m_Map.SetWater(30, 34, 1, 2);
		//////////////////////////////////////////////////////////////////////////
		AddOW(gi, 0, 7, 15, DIR_DOWN, false);
		AddOW(gi, 0, 9, 15, DIR_DOWN, false);

		//AddThing(gi, 0, 6, 18, MB_MAIL);
		//AddThing(gi, 0, 6, 19, MB_MAIL);

		gi->m_Map.SetWall(31, 34, 2, 2);
		gi->m_Map.SetWall(34, 34, 2, 2);
		//
		AddWall(gi, _T("Data\\��.spr"), 10, 9, _T("ѩ��"));
		AddWall(gi, _T("Data\\��.spr"), 26, 5, _T("ѩ��"));
		AddWall(gi, _T("Data\\��.spr"), 2, 44, _T("ѩ��"));
		AddWall(gi, _T("Data\\��.spr"), 44, 31, _T("ѩ��"));
		AddWall(gi, _T("Data\\��.spr"), 8, 54, _T("ѩ��"));
		AddWall(gi, _T("Data\\��.spr"), 29, 54, _T("ѩ��"));
		AddWall(gi, _T("Data\\��.spr"), 55, 40, _T("ѩ��"));

		AddWall(gi, _T("Data\\��.spr"), 44, 7, _T("��ѩ��"));
		AddWall(gi, _T("Data\\��.spr"), 29, 12, _T("��ѩ��"));
		AddWall(gi, _T("Data\\��.spr"), 4, 25, _T("��ѩ��"));
		AddWall(gi, _T("Data\\��.spr"), 21, 26, _T("��ѩ��"));
		AddWall(gi, _T("Data\\��.spr"), 25, 39, _T("��ѩ��"));

		AddWall(gi, _T("Data\\��.spr"), 13, 27, _T("��ѩ��2"));
		AddWall(gi, _T("Data\\��.spr"), 32, 35, _T("��ѩ��2"));//
		AddWall(gi, _T("Data\\��.spr"), 34, 35, _T("��ѩ��2"));
		AddWall(gi, _T("Data\\��.spr"), 14, 43, _T("��ѩ��2"));
		AddWall(gi, _T("Data\\��.spr"), 40, 50, _T("��ѩ��2"));

		AddWall(gi, _T("Data\\��.spr"), 3, 48, _T("����"));
		gi->m_Map.SetWall(0, 47, 5, 1);

		gi->m_Map.SetWall(0, 38, 5, 1);
		gi->m_Map.SetWall(6, 38, 4, 1);
		gi->m_Map.SetWall(10, 15, 3, 1);
		AddWall(gi, _T("Data\\ͼԪ.spr"), 3, 38, _T("ѩ��ľ"));
		AddWall(gi, _T("Data\\ͼԪ.spr"), 8, 38, _T("ѩ��ľ"));
		AddWall(gi, _T("Data\\ͼԪ.spr"), 6, 38, _T("ѩľ׮"));
		AddWall(gi, _T("Data\\ͼԪ.spr"), 11, 15, _T("դ��"));
		AddWall(gi, _T("Data\\ͼԪ.spr"), 8, 15, _T("��ѩ��׮"));

		gi->m_Map.SetWall(6, 47, 3, 1);
		gi->m_Map.SetWall(2, 47, 2, 1);
		gi->m_Map.SetWall(10, 47, 2, 1);
		AddWall(gi, _T("Data\\ͼԪ.spr"), 7, 47, _T("ľդ��"));
		AddWall(gi, _T("Data\\ͼԪ.spr"), 13, 55, _T("��դ��"));
		AddWall(gi, _T("Data\\��.spr"), 10, 47, _T("��ѩ��"));
		//
		gi->m_Map.SetWall(16, 46, 3, 1);
		gi->m_Map.SetWall(20, 46, 4, 1);
		gi->m_Map.SetWall(24, 47, 3, 1);
		gi->m_Map.SetWall(27, 48, 1, 5);
		gi->m_Map.SetWall(28, 53, 1, 3);
		gi->m_Map.SetWall(27, 56, 1, 4);
		
		AddWall(gi, _T("Data\\ͼԪ.spr"), 17, 46, _T("դ��"));
		AddWall(gi, _T("Data\\ͼԪ.spr"), 21, 46, _T("դ��"));
		AddWall(gi, _T("Data\\ͼԪ.spr"), 25, 47, _T("դ��"));
		AddWall(gi, _T("Data\\ͼԪ.spr"), 23, 46, _T("ѩ��׮"));

		AddWall(gi, _T("Data\\ͼԪ.spr"), 27, 49, _T("ѩ��ľ"));
		AddWall(gi, _T("Data\\ͼԪ.spr"), 27, 52, _T("ѩ��ľ"));
		AddWall(gi, _T("Data\\ͼԪ.spr"), 19, 36, _T("ѩդ��"));
		AddWall(gi, _T("Data\\ͼԪ.spr"), 17, 36, _T("ѩ��"));
		AddWall(gi, _T("Data\\��.spr"), 27, 57, _T("ѩ��"));
		AddWall(gi, _T("Data\\��.spr"), 14, 35, _T("ѩ��"));
		AddWall(gi, _T("Data\\��.spr"), 21, 34, _T("����"));
		AddWall(gi, _T("Data\\ͼԪ.spr"), 27, 41, _T("ѩ��ľ"));
		AddWall(gi, _T("Data\\��.spr"), 26, 44, _T("ѩ��"));
		AddWall(gi, _T("Data\\��.spr"), 27, 49, _T("��ѩ��2"));
		//AddWall(gi, _T("Data\\��.spr"), , _T(""));

		gi->m_Map.SetWall(27, 37, 1, 11);
		gi->m_Map.SetWall(13, 35, 2, 1);
		gi->m_Map.SetWall(15, 36, 1, 1);
		gi->m_Map.SetWall(17, 36, 4, 1);
		gi->m_Map.SetWall(21, 33, 1, 3);
		gi->m_Map.SetWall(22, 33, 1, 1);



		AddTrap(gi, NULL, 33, 35, 1);


		AddThing(gi, 0, 5, 17, MB_RED_KEY);
		AddSwitch(gi, 0, 7, 27, MB_RED_SWITCH);

		AddThing(gi, 0, 11, 16, MB_BLACK_KEY);
		AddSwitch(gi, 0, 6, 28, MB_BLACK_SWITCH);

		AddThing(gi, 0, 9, 22, MB_YELLOW_KEY);
		AddSwitch(gi, 0, 8, 28, MB_YELLOW_SWITCH);

		AddRock(gi, 0, 2, 32);
		AddRock(gi, 0, 8, 35);
		AddRockTar(gi, 0, 5, 37);
		AddRockTar(gi, 0, 5, 38);

		AddSwitch(gi, 0, 5, 47, MB_STEPON_TAR);
		AddSwitch(gi, 0, 11, 44, MB_STEPON_SWITCH, 0, 5, 47);

		AddSwitch(gi, 0, 9, 47, MB_STEPON_TAR);
		AddSwitch(gi, 0, 12, 56, MB_STEPON_SWITCH, 0, 9, 47);

		AddOW(gi, 0, 13, 53, DIR_RIGHT, false);
		AddOW(gi, 0, 13, 54, DIR_RIGHT, false);

		AddThing(gi, 0, 22, 55, MB_PASS);
		AddSwitch(gi, 0, 19, 46, MB_PASS_SWITCH);

		AddPortal(gi, 0, 22, 39, 33, 31, 2);
		AddPortal(gi, 0, 33, 31, 22, 39, 3);

		AddTrap(gi, 0, 16, 36, 3);

		AddSwitch(gi, 0, 18, 26, MB_WATER_SWITCH);
		AddSwitch(gi, 0, 29, 36, MB_WATER_SWITCH);

		AddThing(gi, 0, 7, 56, MB_GEM);
		AddThing(gi, 0, 58, 38, MB_GEM);
		AddThing(gi, 0, 47, 15, MB_GEM);

		AddSwitch(gi, 0, 17, 7, MB_END);

		//gi->m_Map.SaveMap(_T("��ͼ.txt"));
		//////////////////////////////////////////////////////////////////////////
		as->MP3Buffers.Add(new Buffer(_T("��ԭ"), _T("Data\\��ԭ.mp3")));//ͨ��.mp3
		as->MP3Buffers.Add(new Buffer(_T("ͨ��"), _T("Data\\ͨ��.mp3")));
		as->Play(_T("��ԭ"));

		SHOW_TIPS(_T("��һ��"));
	}

}